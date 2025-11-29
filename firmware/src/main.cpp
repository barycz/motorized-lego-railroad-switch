#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/uart.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"

#include "RailwayProtocol.h"

const uint LedYellow = 1;

const uint ServoPwmOut = 0;
const uint ServoPeriodUs = 20000;
const uint ServoDutyMinUs = 1300;
const uint ServoDutyMaxUs = 1700;
const uint ServoDutyMidUs = (ServoDutyMaxUs + ServoDutyMinUs) / 2;
const uint ServoClkDivider = SYS_CLK_KHZ / 1000;

static uint ServoPwmSlice = 0;
static uint UpdateCounter = 0;
static uint32_t LastBeaconUs = 0;
static uint32_t LastStatusUs = 0;
static udp_pcb* Udp = nullptr;
static RailwayProtocol::ESwitchDirection SwitchDir;

void setServoDuty(uint dutyUs) {
	pwm_set_chan_level(ServoPwmSlice, 0, dutyUs);
}

void setSwitch(RailwayProtocol::ESwitchDirection dir) {
	printf("setSwitch %u\n", static_cast<uint>(dir));
	SwitchDir = dir;
	switch (dir) {
		case RailwayProtocol::ESwitchDirection::Center:
			setServoDuty(ServoDutyMidUs);
			break;
		case RailwayProtocol::ESwitchDirection::Left:
			setServoDuty(ServoDutyMinUs);
			break;
		case RailwayProtocol::ESwitchDirection::Right:
			setServoDuty(ServoDutyMaxUs);
			break;
	}
}

void handleRailwayProtocolPacket(const RailwayProtocol::Packet& packet) {
	switch (packet.MsgType) {
		case RailwayProtocol::Packet::EMsgType::SetSwitch:
			setSwitch(static_cast<RailwayProtocol::ESwitchDirection>(packet.Data[0]));
			break;
		default:
			printf("unhandled RailwayProtocol packet\n");
			break;
	}
}

void udpBroadcastBeacon() {
	static const size_t BeaconSize = 32;
	uint8_t buffer[BeaconSize];
	const size_t packetSize = RailwayProtocol::Packet::NewBeacon(buffer, BeaconSize, "Switch");
	struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, packetSize, PBUF_RAM);
	memcpy(p->payload, buffer, packetSize);
	const err_t er = udp_sendto(Udp, p, IP_ADDR_BROADCAST, RailwayProtocol::UdpPort);
	if (er != ERR_OK) {
		printf("Failed to send beacon.\n");
	}
	pbuf_free(p);
}

void udpBroadcastStatus() {
	static const size_t StatusSize = 32;
	uint8_t buffer[StatusSize];
	const size_t packetSize = RailwayProtocol::Packet::NewStatus(buffer, StatusSize, SwitchDir);
	struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, packetSize, PBUF_RAM);
	memcpy(p->payload, buffer, packetSize);
	const err_t er = udp_sendto(Udp, p, IP_ADDR_BROADCAST, RailwayProtocol::UdpPort);
	if (er != ERR_OK) {
		printf("Failed to send beacon.\n");
	}
	pbuf_free(p);
}

void udpReceiveCallback(void* arg, udp_pcb* upcb, pbuf* p, const ip_addr_t* addr, u16_t port) {
	printf("received %u bytes in update iteration %u\n", p->len, UpdateCounter);
	const RailwayProtocol::Packet* packet = RailwayProtocol::Packet::FromBuffer(p->payload, p->len);
	if (packet) {
		handleRailwayProtocolPacket(*packet);
	} else {
		printf("skipping incorrect data\n");
	}
	pbuf_free(p);
}

void init() {
	stdio_init_all();

	gpio_init(LedYellow);
	gpio_set_dir(LedYellow, GPIO_OUT);

	gpio_set_function(ServoPwmOut, GPIO_FUNC_PWM);
	ServoPwmSlice = pwm_gpio_to_slice_num(ServoPwmOut);
	pwm_set_clkdiv_int_frac(ServoPwmSlice, ServoClkDivider, 0);
	pwm_set_wrap(ServoPwmSlice, ServoPeriodUs);
	setSwitch(RailwayProtocol::ESwitchDirection::Left);
	pwm_set_enabled(ServoPwmSlice, true);

	if (cyw43_arch_init()) {
		printf("Wi-Fi init failed");
		exit(1);
	}

	cyw43_arch_enable_sta_mode();

	printf("Connecting to Wi-Fi '%s'...\n", WIFI_SSID);
	while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
		printf("failed to connect, retrying ...\n");
	}
	printf("Connected.\n");

	Udp = udp_new();
	if (udp_bind(Udp, IP_ADDR_ANY, RailwayProtocol::UdpPort) != ERR_OK) {
		printf("failed to bind.\n");
		exit(3);
	}
	udp_recv(Udp, udpReceiveCallback, nullptr);
	printf("Listening on %u...\n", RailwayProtocol::UdpPort);
}

void update() {
	gpio_put(LedYellow, 1);
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

	if (time_us_32() >= LastBeaconUs + RailwayProtocol::BeaconIntervalUs) {
		udpBroadcastBeacon();
		LastBeaconUs = time_us_32();
	}

	if (time_us_32() >= LastStatusUs + RailwayProtocol::StatusIntervalUs) {
		udpBroadcastStatus();
		LastStatusUs = time_us_32();
	}

	cyw43_arch_poll();

	gpio_put(LedYellow, 0);
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
	sleep_ms(50);

	++UpdateCounter;
}

int main() {
	init();

	while(true) {
		update();
	}

	cyw43_arch_deinit();
}

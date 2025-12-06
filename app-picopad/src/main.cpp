
#include "DebugLog.h"

#include "RailwayProtocol.h"
#include "RailwayDeviceManager.h"
#include "RailwayDeviceController.h"
#include "Ui.h"

#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/uart.h"
#include "hardware/timer.h"

const uint LedUsr = 22;

static uint UpdateCounter = 0;
static udp_pcb* Udp = nullptr;

static RailwayProtocol::DeviceManager DeviceManager;
RailwayDeviceController DeviceController(DeviceManager, Udp);

void udpReceiveCallback(void* arg, udp_pcb* upcb, pbuf* p, const ip_addr_t* addr, u16_t port) {
	const RailwayProtocol::Packet* packet = RailwayProtocol::Packet::FromBuffer(p->payload, p->len);
	if (packet) {
		DeviceManager.OnPacketReceived(*packet, addr, port);
	} else {
		g_dbgLog.Log("[%u] incorrect %u bytes from %s", UpdateCounter, p->len, ipaddr_ntoa(addr));
	}
	pbuf_free(p);
}

void init() {
	stdio_init_all();

	gpio_init(LedUsr);
	gpio_set_dir(LedUsr, GPIO_OUT);
	gpio_put(LedUsr, 0);

	Ui::Init();

	g_dbgLog.Log("Initializing ...");

	if (cyw43_arch_init()) {
		g_dbgLog.Log("Wi-Fi init failed");
		exit(1);
	}

	cyw43_arch_enable_sta_mode();

	g_dbgLog.Log("Connecting to Wi-Fi '%s'...", WIFI_SSID);
	while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
		g_dbgLog.Log("failed to connect, retrying ...");
	}
	g_dbgLog.Log("Connected.");

	Udp = udp_new();
	if (udp_bind(Udp, IP_ADDR_ANY, RailwayProtocol::UdpPort) != ERR_OK) {
		g_dbgLog.Log("failed to bind.");
		exit(3);
	}
	udp_recv(Udp, udpReceiveCallback, nullptr);
	g_dbgLog.Log("Listening on %u...", RailwayProtocol::UdpPort);
	g_dbgLog.SetAutoFlush(false); // from now on the debug log is drawed as a part of the main UI
}

void update() {
	gpio_put(LedUsr, 1);
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

	Ui::UpdateInputs();
	Ui::BeginWidget();

	cyw43_arch_poll();
	g_dbgLog.Flush();

	Ui::SetColor(Ui::Color::Gray());
	Ui::Separator();

	DeviceController.Update();

	Ui::EndWidget();

	gpio_put(LedUsr, 0);
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
	sleep_ms(10);

	++UpdateCounter;
}

int main() {
	init();

	while(true) {
		update();
	}

	cyw43_arch_deinit();
	Ui::Deinit();
}

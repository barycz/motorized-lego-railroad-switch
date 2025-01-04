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

const uint LedYellow = 1;

const uint ServoPwmOut = 0;
const uint ServoPeriodUs = 20000;
const uint ServoDutyMinUs = 1300;
const uint ServoDutyMaxUs = 1700;
const uint ServoDutyMidUs = (ServoDutyMaxUs + ServoDutyMinUs) / 2;
const uint ServoClkDivider = SYS_CLK_KHZ / 1000;

const uint16_t UdpPort = 57890;

static uint ServoPwmSlice = 0;
static uint UpdateCounter = 0;
static udp_pcb* Udp = nullptr;

void setServoDuty(uint dutyUs) {
	pwm_set_chan_level(ServoPwmSlice, 0, dutyUs);
}

void udpReceiveCallback(void* arg, udp_pcb* upcb, pbuf* p, const ip_addr_t* addr, u16_t port) {
	printf("received %u bytes in update iteration %u\n", p->len, UpdateCounter);
	pbuf_free(p);
}

void init() {
	stdio_init_all();

	if (cyw43_arch_init()) {
		printf("Wi-Fi init failed");
		exit(1);
	}

	cyw43_arch_enable_sta_mode();

	printf("Connecting to Wi-Fi '%s'...\n", WIFI_SSID);
	if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
		printf("failed to connect.\n");
		exit(2);
	} else {
		printf("Connected.\n");
	}

	Udp = udp_new();
	if (udp_bind(Udp, IP_ADDR_ANY, UdpPort) != ERR_OK) {
		printf("failed to bind.\n");
		exit(3);
	}
	udp_recv(Udp, udpReceiveCallback, nullptr);
	printf("Listenint on %u...\n", UdpPort);

	gpio_init(LedYellow);
	gpio_set_dir(LedYellow, GPIO_OUT);

	gpio_set_function(ServoPwmOut, GPIO_FUNC_PWM);
	ServoPwmSlice = pwm_gpio_to_slice_num(ServoPwmOut);
	pwm_set_clkdiv_int_frac(ServoPwmSlice, ServoClkDivider, 0);
	pwm_set_wrap(ServoPwmSlice, ServoPeriodUs);
	pwm_set_chan_level(ServoPwmSlice, 0, ServoDutyMidUs);
	pwm_set_enabled(ServoPwmSlice, true);
}

void update() {
	gpio_put(LedYellow, 1);
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
	setServoDuty(ServoDutyMinUs);
	sleep_ms(1000);

	gpio_put(LedYellow, 0);
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
	setServoDuty(ServoDutyMaxUs);
	sleep_ms(1000);

	cyw43_arch_poll();
	++UpdateCounter;
}

int main() {
	init();

	while(true) {
		update();
	}

	cyw43_arch_deinit();
}

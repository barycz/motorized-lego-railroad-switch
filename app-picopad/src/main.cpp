#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/uart.h"
#include "hardware/timer.h"

#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"

const uint LedUsr = 22;
const uint16_t UdpPort = 57890;

static uint UpdateCounter = 0;
static udp_pcb* Udp = nullptr;

void udpReceiveCallback(void* arg, udp_pcb* upcb, pbuf* p, const ip_addr_t* addr, u16_t port) {
	printf("received %u bytes in update iteration %u\n", p->len, UpdateCounter);
	pbuf_free(p);
}

void init() {
	stdio_init_all();

	gpio_init(LedUsr);
	gpio_set_dir(LedUsr, GPIO_OUT);
	gpio_put(LedUsr, 0);

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
	if (udp_bind(Udp, IP_ADDR_ANY, UdpPort) != ERR_OK) {
		printf("failed to bind.\n");
		exit(3);
	}
	udp_recv(Udp, udpReceiveCallback, nullptr);
	printf("Listening on %u...\n", UdpPort);
}

void update() {
	gpio_put(LedUsr, 1);
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
	sleep_ms(50);

	gpio_put(LedUsr, 0);
	cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
	sleep_ms(50);

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

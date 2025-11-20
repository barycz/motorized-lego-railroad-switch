#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/uart.h"
#include "hardware/timer.h"

#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"

#include "hagl_hal.h"
#include "hagl.h"
#include "font6x9.h"

const uint LedUsr = 22;
const uint16_t UdpPort = 57890;

static uint UpdateCounter = 0;
static udp_pcb* Udp = nullptr;

static hagl_backend_t *Display = nullptr;

void udpReceiveCallback(void* arg, udp_pcb* upcb, pbuf* p, const ip_addr_t* addr, u16_t port) {
	printf("received %u bytes in update iteration %u\n", p->len, UpdateCounter);
	pbuf_free(p);
}

void init() {
	stdio_init_all();

	gpio_init(LedUsr);
	gpio_set_dir(LedUsr, GPIO_OUT);
	gpio_put(LedUsr, 0);

	Display = hagl_init();
	hagl_clear(Display);

	hagl_put_text(Display, L"Initializing ...", 0, 0, hagl_color(Display, 255, 255, 255), font6x9);
	hagl_flush(Display);

#if 0
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
#endif
}

void hagl_demo() {
	hagl_clear(Display);
	for (uint16_t i = 1; i < 500; i++) {
		int16_t x0 = rand() % Display->width;
		int16_t y0 = rand() % Display->height;
		int16_t radius = rand() % 100;
		hagl_color_t color = rand() % 0xffff;

		hagl_fill_circle(Display, x0, y0, radius, color);
	}
	hagl_flush(Display);
}

void update() {
	gpio_put(LedUsr, 1);
	//cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
	//sleep_ms(50);
	hagl_demo();

	gpio_put(LedUsr, 0);
	//cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
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
	hagl_close(Display);
}

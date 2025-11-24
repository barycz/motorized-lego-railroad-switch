
#include "DebugLog.h"

#include "RailwayProtocol.h"

#include "hagl_hal.h"
#include "hagl.h"

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

static hagl_backend_t *Display = nullptr;
DebugLog dbgLog(&Display);

void udpReceiveCallback(void* arg, udp_pcb* upcb, pbuf* p, const ip_addr_t* addr, u16_t port) {
	dbgLog.Log("[%u] received %u bytes from %s", UpdateCounter, p->len, ipaddr_ntoa(addr));
	pbuf_free(p);
}

void init() {
	stdio_init_all();

	gpio_init(LedUsr);
	gpio_set_dir(LedUsr, GPIO_OUT);
	gpio_put(LedUsr, 0);

	Display = hagl_init();
	hagl_clear(Display);

	dbgLog.Log("Initializing ...");

	if (cyw43_arch_init()) {
		dbgLog.Log("Wi-Fi init failed");
		exit(1);
	}

	cyw43_arch_enable_sta_mode();

	dbgLog.Log("Connecting to Wi-Fi '%s'...", WIFI_SSID);
	while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
		dbgLog.Log("failed to connect, retrying ...");
	}
	dbgLog.Log("Connected.");

	Udp = udp_new();
	if (udp_bind(Udp, IP_ADDR_ANY, RailwayProtocol::UdpPort) != ERR_OK) {
		dbgLog.Log("failed to bind.");
		exit(3);
	}
	udp_recv(Udp, udpReceiveCallback, nullptr);
	dbgLog.Log("Listening on %u...", RailwayProtocol::UdpPort);
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
	hagl_close(Display);
}

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/uart.h"
#include "hardware/timer.h"

#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"

#include "RailwayProtocol.h"

#include "hagl_hal.h"
#include "hagl.h"
#include "font6x9.h"
#include "fontx.h"

#include <stdarg.h>

const uint LedUsr = 22;

static uint UpdateCounter = 0;
static udp_pcb* Udp = nullptr;

static hagl_backend_t *Display = nullptr;

// draw text to display using round robin line buffer
struct DebugLog {
	void Log(const char* format, ...);
	void Flush();

private:
	uint16_t DrawLine(uint16_t y, const char* text);

	static const uint8_t LineCount = 4;
	char buffer[LineCount][256];
	uint8_t lineIndex;
};

void DebugLog::Log(const char* format, ...) {
	va_list args;
	va_start(args, format);
	vsnprintf(buffer[lineIndex], sizeof(buffer[lineIndex]), format, args);
	va_end(args);

	printf("%s\n", buffer[lineIndex]);
	lineIndex = (lineIndex + 1) % LineCount;

	Flush();
}

uint16_t DebugLog::DrawLine(uint16_t y, const char* text) {
	fontx_meta_t meta;
	fontx_meta(&meta, font6x9);

	uint16_t x = 0;

	do {
		wchar_t temp = *text++;
		if (13 == temp || 10 == temp) {
			x = 0;
			y += meta.height;
		} else {
			x += hagl_put_char(Display, temp, x, y,  hagl_color(Display, 255, 255, 255), font6x9);
		}
	} while (*text != 0);

	return y + meta.height;
}

void DebugLog::Flush() {
	hagl_clear(Display);
	uint16_t y = 0;
	for (uint8_t i = 0; i < LineCount; i++) {
		y = DrawLine(y, buffer[(i + lineIndex) % LineCount]);
	}
	hagl_flush(Display);
}

DebugLog dbgLog;

void udpReceiveCallback(void* arg, udp_pcb* upcb, pbuf* p, const ip_addr_t* addr, u16_t port) {
	dbgLog.Log("received %u bytes in update iteration %u", p->len, UpdateCounter);
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

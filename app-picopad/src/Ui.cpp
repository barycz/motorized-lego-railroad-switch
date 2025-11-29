
#include "Ui.h"

#include "hagl_hal.h"
#include "hagl.h"
#include "font6x9.h"
#include "fontx.h"

#include <stdarg.h>
#include <stdio.h>

namespace Ui {

struct Context {
	hagl_backend_t* Display;
	size_t WidgetStackSize;
	uint16_t X;
	uint16_t Y;
};

static Context g_Context;

#define UI_CHECK_CONTEXT() \
	if (g_Context.Display == nullptr) { return; }

void Text(const char* format, ...) {
	UI_CHECK_CONTEXT();

	char buffer[32];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	fontx_meta_t meta;
	fontx_meta(&meta, font6x9);

	uint16_t x = g_Context.X;
	uint16_t y = g_Context.Y;

	char* it = buffer;
	do {
		wchar_t temp = *it++;
		if (13 == temp || 10 == temp) {
			x = g_Context.X;
			y += meta.height;
		} else {
			x += hagl_put_char(g_Context.Display, temp, x, y, hagl_color(g_Context.Display, 255, 255, 255), font6x9);
		}
	} while (*it != 0);

	g_Context.Y = y + meta.height;
}

void Separator() {
	UI_CHECK_CONTEXT();
	hagl_draw_hline(g_Context.Display, g_Context.X, g_Context.Y, g_Context.Display->width - 1, hagl_color(g_Context.Display, 255, 255, 255));
	g_Context.Y += 1;
}

void Init() {
	g_Context.Display = hagl_init();
	g_Context.WidgetStackSize = 0;
	g_Context.X = 0;
	g_Context.Y = 0;
	hagl_clear(g_Context.Display);
}

void BeginWidget() {
	UI_CHECK_CONTEXT();
	if (g_Context.WidgetStackSize == 0) {
		hagl_clear(g_Context.Display);
		g_Context.X = 0;
		g_Context.Y = 0;
	}
	g_Context.WidgetStackSize++;
}

void EndWidget() {
	UI_CHECK_CONTEXT();
	if (g_Context.WidgetStackSize == 0) {
		printf("Ui::EndWidget() stack underflow");
		return;
	}
	g_Context.WidgetStackSize--;

	if (g_Context.WidgetStackSize == 0) {
		hagl_flush(g_Context.Display);
	}
}

void Deinit() {
	UI_CHECK_CONTEXT();
	hagl_close(g_Context.Display);
	g_Context.Display = nullptr;
}

}

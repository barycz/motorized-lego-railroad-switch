#include "DebugLog.h"

#include "hagl_hal.h"
#include "hagl.h"
#include "font6x9.h"
#include "fontx.h"

#include <cstdio>
#include <stdarg.h>

DebugLog::DebugLog(hagl_backend_t** display)
	: _lineIndex(0)
	, _display(display) {
}

void DebugLog::Log(const char* format, ...) {
	va_list args;
	va_start(args, format);
	vsnprintf(_buffer[_lineIndex], sizeof(_buffer[_lineIndex]), format, args);
	va_end(args);

	printf("%s\n", _buffer[_lineIndex]);
	_lineIndex = (_lineIndex + 1) % LineCount;

	Flush();
}

uint16_t DebugLog::DrawLine(uint16_t y, const char* text) {
	if (_display == nullptr || *_display == nullptr) {
		return y;
	}

	fontx_meta_t meta;
	fontx_meta(&meta, font6x9);

	uint16_t x = _x0;

	do {
		wchar_t temp = *text++;
		if (13 == temp || 10 == temp) {
			x = _x0;
			y += meta.height;
		} else {
			x += hagl_put_char(*_display, temp, x, y,  hagl_color(*_display, 255, 255, 255), font6x9);
		}
	} while (*text != 0);

	return y + meta.height;
}

void DebugLog::Flush() {
	if (_display == nullptr || *_display == nullptr) {
		return;
	}

	hagl_clear(*_display);
	uint16_t y = _y0;
	for (uint8_t i = 0; i < LineCount; i++) {
		y = DrawLine(y, _buffer[(i + _lineIndex) % LineCount]);
	}
	hagl_flush(*_display);
}

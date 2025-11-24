#pragma once

#include "hagl/backend.h"

class DebugLog {
public:
	DebugLog(hagl_backend_t** display);
	void Log(const char* format, ...);
	void Flush();

private:
	uint16_t DrawLine(uint16_t y, const char* text);

	static const uint8_t LineCount = 4;
	char _buffer[LineCount][256];
	static const uint16_t _x0 = 0;
	static const uint16_t _y0 = 0;
	uint8_t _lineIndex;
	hagl_backend_t** _display;
};

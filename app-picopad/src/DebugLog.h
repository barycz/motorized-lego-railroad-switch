#pragma once

#include <stdint.h>

class DebugLog {
public:
	void Log(const char* format, ...);
	void Flush();

private:
	static const uint8_t LineCount = 4;
	char _buffer[LineCount][256];
	uint8_t _lineIndex = 0;
};

extern DebugLog g_dbgLog;

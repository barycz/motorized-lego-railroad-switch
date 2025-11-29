#pragma once

#include <stdint.h>

class DebugLog {
public:
	void Log(const char* format, ...);
	void Flush();
	void SetAutoFlush(bool autoFlush) { _autoFlush = autoFlush; }

private:
	static const uint8_t LineCount = 4;
	char _buffer[LineCount][256];
	uint8_t _lineIndex = 0;
	bool _autoFlush = true;
};

extern DebugLog g_dbgLog;

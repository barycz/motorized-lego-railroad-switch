#include "DebugLog.h"

#include "Ui.h"

#include <cstdio>
#include <stdarg.h>

DebugLog g_dbgLog;

void DebugLog::Log(const char* format, ...) {
	va_list args;
	va_start(args, format);
	vsnprintf(_buffer[_lineIndex], sizeof(_buffer[_lineIndex]), format, args);
	va_end(args);

	printf("%s\n", _buffer[_lineIndex]);
	_lineIndex = (_lineIndex + 1) % LineCount;

	if (_autoFlush) {
		Flush();
	}
}

void DebugLog::Flush() {
	Ui::BeginWidget();
	for (uint8_t i = 0; i < LineCount; ++i) {
		Ui::Text("%s", _buffer[(i + _lineIndex) % LineCount]);
	}
	Ui::EndWidget();
}

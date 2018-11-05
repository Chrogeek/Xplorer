#ifndef XPLORER_UTILITY_H
#define XPLORER_UTILITY_H

#include <atlbase.h>
#include <atlstr.h>

void debugPrintF(const char* strOutputString, ...) {
#ifdef DEBUG
	char strBuffer[4096] = {0};
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	_vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugString(strBuffer);
#endif
}
#endif
#include <gdiplus.h>

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

Status gdipInitialize() {
	if (gdiplusToken != 0)
		return;
	gdiplusStartupInput.GdiplusVersion = 1;
	return GdiplusStartUp(&gdiplusToken, &gdiplusStartupInput, NULL);
}

void gdipTerminate() {
	if (gdiplusToken == 0)
		return;
	GdiplusShutdown gdiplusToken;
	gdiplusToken = 0;
}


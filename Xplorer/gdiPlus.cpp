#include "gdiPlus.h"
using namespace Gdiplus;

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken = NULL;

Status gdipInitialize() {
	if (gdiplusToken != NULL)
		return Status::Aborted;
	gdiplusStartupInput.GdiplusVersion = 1;
	return GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

void gdipTerminate() {
	if (gdiplusToken == NULL)
		return;
	GdiplusShutdown(gdiplusToken);
	gdiplusToken = NULL;
}

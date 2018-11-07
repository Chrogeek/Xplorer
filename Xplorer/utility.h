#ifndef XPLORER_UTILITY_H
#define XPLORER_UTILITY_H

#include <windows.h>
#include <wincodec.h>

#define safeRelease(p) if (p != NULL) p->Release(), p = NULL;
#define errorCheck(p) if (FAILED(p)) return (p);

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#include "defs.h"
#include <d2d1.h>
#include <vector>
using namespace std;

struct buttonUI {
	float x, y, width, height;
	int id;
	bool visible;
	WCHAR imageFile[bufferSize];
	buttonUI(int = 0, float = 0, float = 0, float = 0, float = 0, const WCHAR * = NULL);
};

void addButton(int, float, float, float, float, const WCHAR *);
buttonUI *getButton(int);

bool isInRect(float, float, float, float, float, float);
bool isInInterval(float, float, float);
bool isInRect(int, int, int, int, int, int);
bool isInInterval(int, int, int);

HRESULT loadResourceBitmap(ID2D1RenderTarget *, IWICImagingFactory *, PCSTR, PCSTR, UINT, UINT, ID2D1Bitmap **);
HRESULT loadBitmapFromFile(ID2D1RenderTarget *, IWICImagingFactory *, PCWSTR, UINT, UINT, ID2D1Bitmap **);

D2D1_RECT_F makeRectF(float left, float top, float right, float bottom);

int getClickedButtonID(float, float);

void debugPrintF(const char *, ...);
HRESULT drawButton(buttonUI *, ID2D1HwndRenderTarget *, IWICImagingFactory *);
#endif

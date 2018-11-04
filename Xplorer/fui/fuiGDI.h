#ifndef FUIGDI_H
#define FUIGDI_H

#include <windows.h>
#include <wingdi.h>
#include <algorithm>
#include "fuiDefs.h"
using namespace std;
HDC gdiCreateDC(HDC hDC) {
	return CreateCompatibleDC(hDC);
}
HBITMAP gdiCreateBitmap(HDC hDC, int width, int height, VOID **lplpVoid = NULL) {
	class tmpclass {
	public:
		BITMAPINFO b;
	};
	static tmpclass b;
	BITMAPINFOHEADER &bm = b.b.bmiHeader;
	bm.biSize = sizeof bm;
	bm.biWidth = width;
	bm.biHeight = height;
	bm.biPlanes = 1;
	bm.biBitCount = 32;
	bm.biSizeImage = bm.biWidth * bm.biHeight * 4;
	bm.biCompression = 0;
	bm.biXPelsPerMeter = 0;
	bm.biYPelsPerMeter = 0;
	bm.biClrUsed = 0;
	bm.biClrImportant = 0;
	b.b.bmiColors[0].rgbBlue = b.b.bmiColors[0].rgbGreen = b.b.bmiColors[0].rgbRed = b.b.bmiColors[0].rgbReserved = 0;
	b.b.bmiColors[1].rgbBlue = b.b.bmiColors[1].rgbGreen = b.b.bmiColors[1].rgbRed = b.b.bmiColors[1].rgbReserved = 0;
	HBITMAP ret = CreateDIBSection(hDC, &b.b, 0, lplpVoid, NULL, 0);
	DeleteObject(SelectObject(hDC, ret));
	return ret;
}
void gdiDeleteObject(HDC hDC, HBITMAP bitmap) {
	DeleteDC(hDC);
	DeleteObject(bitmap);
}
void gdiRecreateBitmap(HDC &hDC, HBITMAP &bitmap, int width, int height, VOID **lplpVoid = NULL) {
	HDC tmp_hdc = gdiCreateDC(NULL);
	HBITMAP tmp_bitmap = gdiCreateBitmap(tmp_hdc, width, height, lplpVoid);
	BitBlt(tmp_hdc, 0, 0, width, height, hDC, 0, 0, SRCCOPY);
	gdiDeleteObject(hDC, bitmap);
	hDC = tmp_hdc;
	bitmap = tmp_bitmap;
}
void gdiBitmapClear(HDC hDC, int left, int top, int width, int height) {
	BitBlt(hDC, left, top, width, height, 0, 0, 0, BLACKNESS);
}
HPEN gdiCreatePen(HDC hDC, int color, int width = 1, int style = 0) {
	HPEN ret = CreatePen(style, width, color);
	DeleteObject(SelectObject(hDC, ret));
	return ret;
}
HBRUSH gdiCreateBrush(HDC hDC, int color) {
	HBRUSH ret = CreateSolidBrush(color);
	DeleteObject(SelectObject(hDC, ret));
	return ret;
}
void gdiDrawLine(HDC hDC, int x1, int y1, int x2, int y2) {
	POINT vPoint;
	MoveToEx(hDC, x1, y1, &vPoint);
	LineTo(hDC, x2, y2);
}
void gdiDrawRect(HDC hDC, HBRUSH brush, int x, int y, int w, int h) {
	RECT r;
	r.left = x;
	r.top = y;
	r.right = x + w;
	r.bottom = y + h;
	if (w < 0) swap(r.left, r.right);
	if (h < 0) swap(r.top, r.bottom);
	FrameRect(hDC, &r, brush);
}
void gdiFillRect(HDC hDC, HBRUSH brush, int x, int y, int w, int h) {
	RECT r;
	r.left = x;
	r.top = y;
	r.right = x + w;
	r.bottom = y + h;
	if (w < 0) swap(r.left, r.right);
	if (h < 0) swap(r.top, r.bottom);
	FillRect(hDC, &r, brush);
}

#endif

#ifndef FUIGDI_H
#define FUIGDI_H

#include <windows.h>
#include <wingdi.h>
#include <algorithm>
#include "fuiDefs.h"
using namespace std;
HDC gdiCreateDC(HDC);
HBITMAP gdiCreateBitmap(HDC, int, int, VOID ** = NULL);
void gdiDeleteObject(HDC, HBITMAP);
void gdiRecreateBitmap(HDC &, HBITMAP &, int, int, VOID ** = NULL);
void gdiBitmapClear(HDC, int, int, int, int);
HPEN gdiCreatePen(HDC, int, int = 1, int = 0);
HBRUSH gdiCreateBrush(HDC, int);
void gdiDrawLine(HDC, int, int, int, int);
void gdiDrawRect(HDC, HBRUSH, int, int, int, int);
void gdiFillRect(HDC, HBRUSH, int, int, int, int);

#endif

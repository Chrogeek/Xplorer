/*
	File Name: utility.h

	Header file correlated to utility.cpp.
	------------------------------------------------------------
	Xplorer, yet another 2D jumping game
	Copyright (C) 2018 Chrogeek

	<https://github.com/Chrogeek/Xplorer>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef XPLORER_UTILITY_H
#define XPLORER_UTILITY_H

#include "defs.h"
#include <windows.h>
#include <wincodec.h>
#include <d2d1.h>
#include <string>
#include <vector>
#include "geometry.h"

#define safeRelease(p) if (p != nullptr) { p->Release(); p = nullptr; }

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

struct buttonUI {
	double x, y, width, height;
	bool visible;
	ID2D1Bitmap *buttonImage;
	buttonUI(double = 0.0, double = 0.0, double = 0.0, double = 0.0, const WCHAR * = nullptr);
};

struct rectReal {
	double left;
	double top;
	double right;
	double bottom;
};

void limitVelocity(pointVector &);

void disableAllButtons();

bool isInRect(double, double, double, double, double, double);
bool isInInterval(double, double, double);
bool isInRect(int, int, int, int, int, int);
bool isInInterval(int, int, int);

bool isIntervalIntersect(double, double, double, double);
bool isRectIntersect(double, double, double, double, double, double, double, double);

bool isRectIntersect(rectReal, rectReal);

pointVector rectCenter(rectReal);

bool isIntervalEquivalent(double, double, double, double);

double intervalIntersectionLength(double, double, double, double);

bool isIntervalIntersect(int, int, int, int);
bool isRectIntersect(int, int, int, int, int, int, int, int);

bool isIntervalEquivalent(int, int, int, int);

int intervalIntersectionLength(int, int, int, int);

HRESULT loadResourceBitmap(ID2D1RenderTarget *, IWICImagingFactory *, PCSTR, PCSTR, UINT, UINT, ID2D1Bitmap **);
HRESULT loadBitmapFromFile(ID2D1RenderTarget *, IWICImagingFactory *, PCWSTR, UINT, UINT, ID2D1Bitmap **);

rectFloat makeRectF(float left, float top, float right, float bottom);
rectReal makeRectR(double left, double top, double right, double bottom);
rectFloat rectR2F(rectReal);
rectReal rectF2R(rectFloat);

D2D1_SIZE_U makeSizeU(int, int);
D2D1_SIZE_F makeSizeF(float, float);

int getClickedButtonID(double, double);

void debugPrintF(const char *, ...);
void drawButton(buttonUI *);

int dcmp(double);
int dcmp(double, double);

std::string intToString(int);
std::wstring stringToWidestring(std::string);
#endif

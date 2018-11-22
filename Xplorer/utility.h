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

#include <windows.h>
#include <wincodec.h>
#include <d2d1.h>
#include <vector>
#include "defs.h"

#define safeRelease(p) if (p != NULL) { p->Release(); p = NULL; }

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif
using namespace std;

struct buttonUI {
	float x, y, width, height;
	bool visible;
	ID2D1Bitmap *buttonImage;
	buttonUI(float = 0, float = 0, float = 0, float = 0, const WCHAR * = NULL);
};

struct pointVector {
	float vX, vY;
	pointVector(float = 0.f, float = 0.f);
	pointVector operator+(pointVector) const;
	pointVector operator-(pointVector) const;
	pointVector operator-() const;
	pointVector operator*(float) const;
	pointVector operator/(float) const;
	pointVector &operator+=(pointVector);
	pointVector &operator-=(pointVector);
};

float minValue(float, float);
float maxValue(float, float);

int minValue(int, int);
int maxValue(int, int);

void limitVelocity(pointVector &);

pointVector operator*(float, pointVector);

void disableAllButtons();

bool isInRect(float, float, float, float, float, float);
bool isInInterval(float, float, float);
bool isInRect(int, int, int, int, int, int);
bool isInInterval(int, int, int);

bool isIntervalIntersect(float, float, float, float);
bool isRectIntersect(float, float, float, float, float, float, float, float);

bool isRectIntersect(D2D1_RECT_F, D2D1_RECT_F);

pointVector rectCenter(D2D1_RECT_F);

bool isIntervalEquivalent(float, float, float, float);

float intervalIntersectionLength(float, float, float, float);

bool isIntervalIntersect(int, int, int, int);
bool isRectIntersect(int, int, int, int, int, int, int, int);

bool isIntervalEquivalent(int, int, int, int);

int intervalIntersectionLength(int, int, int, int);

HRESULT loadResourceBitmap(ID2D1RenderTarget *, IWICImagingFactory *, PCSTR, PCSTR, UINT, UINT, ID2D1Bitmap **);
HRESULT loadBitmapFromFile(ID2D1RenderTarget *, IWICImagingFactory *, PCWSTR, UINT, UINT, ID2D1Bitmap **);

D2D1_RECT_F makeRectF(float left, float top, float right, float bottom);

int getClickedButtonID(float, float);

void debugPrintF(const char *, ...);
void drawButton(buttonUI *);

int dcmp(float);
int dcmp(float, float);
#endif

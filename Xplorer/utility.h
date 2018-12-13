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
#include <dwrite.h>
#include <string>
#include <vector>
#include "json.h"
#include "gameFrame.h"
#include "geometry.h"
#include "particles.h"

#define safeRelease(p) if (p != nullptr) { p->Release(); p = nullptr; }
#define safeNew(p, q) if (p != nullptr) delete p; p = new q;

#define increase(p, q, r) p = (q)(p) + r;
#define decrease(p, q, r) p = (q)(p) - r;

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

using json = nlohmann::json;

struct buttonUI {
	double x, y, width, height;
	bool enabled;
	std::wstring caption;
	ID2D1Bitmap *buttonImage;
	buttonUI(double = 0.0, double = 0.0, double = 0.0, double = 0.0, std::wstring = L"", const WCHAR * = nullptr);
};

struct rectReal {
	double left;
	double top;
	double right;
	double bottom;
};

void clearParticles(std::vector<particle *> &);

void doEvents();

int dcmp(double);
int dcmp(double, double);

bool isInRect(double, double, double, double, double, double);
bool isInRect(int, int, int, int, int, int);

bool isInInterval(double, double, double);
bool isInInterval(int, int, int);

bool isIntervalIntersect(double, double, double, double);

bool isRectIntersect(double, double, double, double, double, double, double, double);
bool isRectIntersect(rectReal, rectReal);

bool isIntervalEquivalent(double, double, double, double);
bool isIntervalEquivalent(int, int, int, int);

double intervalIntersectionLength(double, double, double, double);
int intervalIntersectionLength(int, int, int, int);

bool isIntervalIntersect(int, int, int, int);

bool isRectIntersect(int, int, int, int, int, int, int, int);

pointVector rectCenter(rectReal);

rectFloat makeRectF(float left, float top, float right, float bottom);
rectReal makeRectR(double left, double top, double right, double bottom);

D2D1_SIZE_U makeSizeU(int, int);
D2D1_SIZE_F makeSizeF(float, float);

D2D1_ELLIPSE makeEllipse(pointVector, double, double);

rectFloat rectR2F(rectReal);
rectReal rectF2R(rectFloat);
D2D1_POINT_2F pointToD2Point(pointVector);

int getNumberFromString(std::string);
std::string intToString(longint);
std::wstring intToWideString(longint);
std::wstring stringToWidestring(std::string);

std::wstring secondsToWideString(double);
std::wstring doubleToWideString(double);

bool isStringEndIn(std::string, std::string);

void getFontFamilyWeight(std::string, std::string &, DWRITE_FONT_WEIGHT &);

void drawButton(ID2D1RenderTarget *, buttonUI *);
void disableAllButtons();
int getClickedButtonID(double, double);

HRESULT loadResourceBitmap(ID2D1RenderTarget *, IWICImagingFactory *, PCSTR, PCSTR, UINT, UINT, ID2D1Bitmap **);
HRESULT loadBitmapFromFile(ID2D1RenderTarget *, IWICImagingFactory *, PCWSTR, UINT, UINT, ID2D1Bitmap **);

void limitVelocity(pointVector &);

void debugPrintF(const char *, ...);

gameResult loadJSONFromFile(std::string, json &);

HRESULT drawText(ID2D1RenderTarget *, IDWriteFactory *, std::wstring, DWRITE_FONT_WEIGHT, bool, float, std::wstring, rectFloat, DWRITE_TEXT_ALIGNMENT, DWRITE_PARAGRAPH_ALIGNMENT, ID2D1Brush *);

void switchToFrame(gameFrame *);
void loadNextFrame();

double randomDouble(double, double);

std::string toLower(std::string);

void makeRain();
void renderRain(float);

HRESULT getTextSize(IDWriteFactory *, std::wstring, DWRITE_FONT_WEIGHT, bool, float, std::wstring, DWRITE_TEXT_ALIGNMENT, DWRITE_PARAGRAPH_ALIGNMENT, float, float, D2D1_SIZE_F &);
#endif

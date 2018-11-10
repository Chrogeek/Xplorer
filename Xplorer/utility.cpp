/*
	File Name: utility.cpp

	This file provides useful tools to help with implementation.
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

#include <windows.h>
#include <wincodec.h>
#include <map>
#include <algorithm>
#include "defs.h"
#include "utility.h"
using namespace std;

#define DEBUG

extern ID2D1Factory *d2dFactory;
extern IWICImagingFactory *imageFactory;
extern ID2D1DCRenderTarget *renderTarget;
extern buttonUI *buttons[maxStage + 1];
/*
extern pointVector maxVelocity;
extern pointVector minVelocity;
extern pointVector jumpVelocityDelta;
extern pointVector gravityAcceleration;
extern pointVector moveAcceleration;*/

pointVector::pointVector(float x, float y) : vX(x), vY(y) {}

pointVector pointVector::operator+(pointVector b) const {
	return pointVector(vX + b.vX, vY + b.vY);
}

pointVector pointVector::operator-(pointVector b) const {
	return pointVector(vX - b.vX, vY - b.vY);
}

pointVector pointVector::operator-() const {
	return pointVector(-vX, -vY);
}

pointVector pointVector::operator*(float b) const {
	return pointVector(vX * b, vY * b);
}

pointVector pointVector::operator/(float b) const {
	return pointVector(vX / b, vY / b);
}

pointVector &pointVector::operator+=(pointVector b) {
	vX += b.vX, vY += b.vY;
	return *this;
}

pointVector &pointVector::operator-=(pointVector b) {
	vX -= b.vX, vY -= b.vY;
	return *this;
}

pointVector operator*(float a, pointVector b) {
	return pointVector(a * b.vX, a * b.vY);
}

float minValue(float a, float b) {
	return a < b ? a : b;
}

float maxValue(float a, float b) {
	return a < b ? b : a;
}

void limitVelocity(pointVector &x) {
	x.vX = minValue(maxVelocity.vX, maxValue(minVelocity.vX, x.vX));
	x.vY = minValue(maxVelocity.vY, maxValue(minVelocity.vY, x.vY));
}

int getClickedButtonID(float X, float Y) {
	for (int i = 0; i <= maxButton; ++i) {
		buttonUI *btn = buttons[i];
		if (btn == NULL) continue;
		if (isInRect(X, Y, btn->x, btn->y, btn->x + btn->width, btn->y + btn->height)) return i;
	}
	return buttonNull;
}

bool isInRect(float x, float y, float x1, float y1, float x2, float y2) {
	return dcmp(x1 - x) <= 0 && dcmp(x - x2) < 0 && dcmp(y1 - y) <= 0 && dcmp(y - y2) < 0;
}

bool isInInterval(float x, float x1, float x2) {
	return dcmp(x1 - x) <= 0 && dcmp(x - x2) < 0;
}

bool isInInterval(int x, int x1, int x2) {
	return x1 <= x && x < x2;
}

bool isInRect(int x, int y, int x1, int y1, int x2, int y2) {
	return isInInterval(x, x1, x2) && isInInterval(y, y1, y2);
}

bool isIntervalIntersect(float l1, float r1, float l2, float r2) {
	return dcmp(maxValue(l1, l2) - minValue(r1, r2)) < 0;
}

bool isIntervalEquivalent(float l1, float r1, float l2, float r2) {
	return dcmp(l1, l2) == 0 && dcmp(r1, r2) == 0;
}

bool isRectIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
	return isIntervalIntersect(x1, x2, x3, x4) && isIntervalIntersect(y1, y2, y3, y4);
}

float intervalIntersectionLength(float l1, float r1, float l2, float r2) {
	return minValue(r1, r2) - maxValue(l1, l2);
}

buttonUI::buttonUI(float x, float y, float w, float h, const WCHAR *fileName) {
	this->x = x, this->y = y, this->width = w, this->height = h;
	this->visible = false;
	this->buttonImage = NULL;
	loadBitmapFromFile(renderTarget, imageFactory, fileName, (UINT)w, (UINT)h, &this->buttonImage);
}
// Load bitmap from app's resource
HRESULT loadResourceBitmap(
	ID2D1RenderTarget* pRendertarget,
	IWICImagingFactory* pIWICFactory,
	PCSTR resourceName,
	PCSTR resourceType,
	UINT destinationWidth,
	UINT destinationHeight,
	ID2D1Bitmap** ppBitmap
) {
	HRESULT hr = S_OK;

	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICStream* pStream = NULL;
	IWICFormatConverter* pConverter = NULL;
	IWICBitmapScaler* pScaler = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void* pImageFile = NULL;
	DWORD imageFileSize = 0;

	// Find the resource then load it
	imageResHandle = FindResource(HINST_THISCOMPONENT, resourceName, resourceType);
	hr = imageResHandle ? S_OK : E_FAIL;

	if (SUCCEEDED(hr)) {
		imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);
	}
	hr = imageResDataHandle ? S_OK : E_FAIL;

	// Lock the resource and calculate the image's size
	if (SUCCEEDED(hr)) {
		// Lock it to get the system memory pointer
		pImageFile = LockResource(imageResDataHandle);
	}
	hr = pImageFile ? S_OK : E_FAIL;
	if (SUCCEEDED(hr)) {
		// Calculate the size
		imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);
	}
	hr = imageFileSize ? S_OK : E_FAIL;

	// Create an IWICStream object
	if (SUCCEEDED(hr)) {
		// Create a WIC stream to map onto the memory
		hr = pIWICFactory->CreateStream(&pStream);
	}
	if (SUCCEEDED(hr)) {
		// Initialize the stream with the memory pointer and size
		hr = pStream->InitializeFromMemory(
			reinterpret_cast<BYTE*>(pImageFile),
			imageFileSize
		);
	}
	// Create IWICBitmapDecoder
	if (SUCCEEDED(hr)) {
		// Create a decoder for the stream
		hr = pIWICFactory->CreateDecoderFromStream(
			pStream,
			NULL,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
		);
	}
	// Retrieve a frame from the image and store it in an IWICBitmapFrameDecode object
	if (SUCCEEDED(hr)) {
		// Create the initial frame
		hr = pDecoder->GetFrame(0, &pSource);
	}
	// Before Direct2D can use the image, it must be converted to the 32bppPBGRA pixel format.
	// To convert the image format, use the IWICImagingFactory::CreateFormatConverter method to create an IWICFormatConverter object, then use the IWICFormatConverter object's Initialize method to perform the conversion.
	if (SUCCEEDED(hr)) {
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}
	if (SUCCEEDED(hr)) {
		// If a new width or height was specified, create and
		// IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0) {
			UINT originalWidth;
			UINT originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (destinationWidth == 0) {
				FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
				destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
			} else if (destinationHeight == 0) {
				FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
				destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
			}

			hr = pIWICFactory->CreateBitmapScaler(&pScaler);
			if (SUCCEEDED(hr)) {
				hr = pScaler->Initialize(
					pSource,
					destinationWidth,
					destinationHeight,
					WICBitmapInterpolationModeCubic
				);
			}
			if (SUCCEEDED(hr)) {
				hr = pConverter->Initialize(
					pScaler,
					GUID_WICPixelFormat32bppPBGRA,
					WICBitmapDitherTypeNone,
					NULL,
					0.f,
					WICBitmapPaletteTypeMedianCut
				);
			}
		} else { // use default width and height
			hr = pConverter->Initialize(
				pSource,
				GUID_WICPixelFormat32bppPBGRA,
				WICBitmapDitherTypeNone,
				NULL,
				0.f,
				WICBitmapPaletteTypeMedianCut
			);
		}
	}
	if (SUCCEEDED(hr)) {
		// Finally, Create an ID2D1Bitmap object, that can be drawn by a render target and used with other Direct2D objects
			// Create a Direct2D bitmap from the WIC bitmap
		hr = pRendertarget->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
		);
	}
	safeRelease(pDecoder);
	safeRelease(pSource);
	safeRelease(pStream);
	safeRelease(pConverter);
	safeRelease(pScaler);

	return hr;
}
//
// Creates a Direct2D bitmap from the specified
// file name.
//
HRESULT loadBitmapFromFile(ID2D1RenderTarget *pRenderTarget, IWICImagingFactory *pIWICFactory, PCWSTR uri, UINT destinationWidth, UINT destinationHeight, ID2D1Bitmap **ppBitmap) {
	HRESULT hr = S_OK;

	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICStream *pStream = NULL;
	IWICFormatConverter *pConverter = NULL;
	IWICBitmapScaler *pScaler = NULL;

	hr = pIWICFactory->CreateDecoderFromFilename(uri, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);
	if (SUCCEEDED(hr)) {
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr)) {
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}
	if (SUCCEEDED(hr)) {
		// If a new width or height was specified, create an
		// IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0) {
			UINT originalWidth, originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr)) {
				if (destinationWidth == 0) {
					FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
					destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				} else if (destinationHeight == 0) {
					FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
					destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = pIWICFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr)) {
					hr = pScaler->Initialize(pSource, destinationWidth, destinationHeight, WICBitmapInterpolationModeCubic);
				}
				if (SUCCEEDED(hr)) {
					hr = pConverter->Initialize(pScaler, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
				}
			}
		} else { // Don't scale the image.
			hr = pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
		}
	}
	if (SUCCEEDED(hr)) {
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, ppBitmap);
	}

	safeRelease(pDecoder);
	safeRelease(pSource);
	safeRelease(pStream);
	safeRelease(pConverter);
	safeRelease(pScaler);

	return hr;
}

void disableAllButtons() {
	for (int i = 0; i <= maxButton; ++i) {
		buttonUI *btn = buttons[i];
		if (btn == NULL) continue;
		btn->visible = false;
	}
}

int dcmp(float x) {
	if (fabs(x) < epsilon) return 0;
	return x < 0.f ? -1 : 1;
}

int dcmp(float x, float y) {
	if (fabs(x - y) < epsilon) return 0;
	return x < y ? -1 : 1;
}

D2D1_RECT_F makeRectF(float left, float top, float right, float bottom) {
	D2D1_RECT_F ret;
	ret.left = left;
	ret.top = top;
	ret.right = right;
	ret.bottom = bottom;
	return ret;
}

void debugPrintF(const char *strOutputString, ...) {
#ifdef DEBUG
	char strBuffer[4096] = {0};
	va_list vlArgs;
	va_start(vlArgs, strOutputString);
	_vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
	va_end(vlArgs);
	OutputDebugString(strBuffer);
#endif
}

void drawButton(buttonUI *button) {
	if (button == NULL) return;
	renderTarget->DrawBitmap(button->buttonImage, makeRectF(button->x, button->y, button->x + button->width, button->y + button->height));
}

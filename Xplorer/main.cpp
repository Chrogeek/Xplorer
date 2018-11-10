/*
	File Name: main.cpp

	This file defines program entrance and initialization.
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
#include <d2d1.h>
#include <dwrite.h>
#include <cstdio>
#include <cassert>
#include <vector>
#include <map>
#include "defs.h"
#include "utility.h"
#include "handler.h"
using namespace std;

extern ID2D1Factory *d2dFactory;
extern IDWriteFactory *writeFactory;
extern IDWriteTextFormat *textFormatNormal;
extern IWICImagingFactory *imageFactory;
extern ID2D1DCRenderTarget *renderTarget;
extern buttonUI *buttons[maxButton + 1];
extern float dpiX, dpiY;
extern ID2D1Bitmap *bkgImage, *wallImage, *heroImage, *bitmapBackground;
extern ID2D1SolidColorBrush *brushBlack;

HRESULT initializeGraphics() {
	HRESULT result = S_OK;
	if (SUCCEEDED(result)) {
		result = CoInitialize(NULL);
	}
	if (SUCCEEDED(result)) {
		result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);
	}
	if (SUCCEEDED(result)) {
		result = CoCreateInstance(CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void **>(&imageFactory));
	}
	if (SUCCEEDED(result)) {
		result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));
	}
	if (SUCCEEDED(result)) {
		result = writeFactory->CreateTextFormat(gameFontName, NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20, L"en-us", &textFormatNormal);
	}
	if (SUCCEEDED(result)) {
		D2D1_RENDER_TARGET_PROPERTIES *renderTargetProperties = NULL;
		renderTargetProperties = new D2D1_RENDER_TARGET_PROPERTIES;
		renderTargetProperties->dpiX = 0.f;
		renderTargetProperties->dpiY = 0.f;
		renderTargetProperties->type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		renderTargetProperties->usage = D2D1_RENDER_TARGET_USAGE_NONE;
		renderTargetProperties->minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
		renderTargetProperties->pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
		renderTargetProperties->pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
		result = d2dFactory->CreateDCRenderTarget(renderTargetProperties, &renderTarget);
	}
	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(renderTarget, imageFactory, L"images/bg_blue.png", windowClientWidth, windowClientHeight, &bitmapBackground);
	}
	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(renderTarget, imageFactory, wallImageName, heroSize, heroSize, &wallImage);
	}
	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(renderTarget, imageFactory, heroImageName, heroSize * 24, heroSize, &heroImage);
	}
	if (SUCCEEDED(result)) {
		result = renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brushBlack);
	}
	buttons[buttonStart] = new buttonUI(300, 300, 200, 80, L"images/startButton.png");
	buttons[buttonExit] = new buttonUI(windowClientWidth - 135, windowClientHeight - 60, 120, 45, L"images/exitButton.png");
/*	if (FAILED(result)) {
		safeRelease(imageFactory);
		safeRelease(renderTarget);
		safeRelease(writeFactory);
		safeRelease(d2dFactory);
	}*/
	return result;
}

void terminateGraphics() {
	safeRelease(bkgImage);
	safeRelease(wallImage);
	safeRelease(heroImage);
	safeRelease(bitmapBackground);
	safeRelease(textFormatNormal);
	safeRelease(brushBlack);
	safeRelease(imageFactory);
	safeRelease(renderTarget);
	safeRelease(writeFactory);
	safeRelease(d2dFactory);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
		case WM_CREATE:
		{
			if (FAILED(initializeGraphics())) {
				MessageBox(hwnd, "Graphics initialization failed!", "Error!", MB_ICONSTOP | MB_OK);
				PostQuitMessage(0);
				break;
			}
			d2dFactory->GetDesktopDpi(&dpiX, &dpiY);

			// Adjust window size to make its internal area (client area) just the size of windowClientWidth * windowClientHeight.
			RECT rect1, rect2;
			int screenWidth, screenHeight, newWidth, newHeight;

			GetWindowRect(hwnd, &rect1);  // rect1: left/top/right/bottom of the whole window
			GetClientRect(hwnd, &rect2);  // rect2: left/top/right/bottom of client area

			screenWidth = GetSystemMetrics(SM_CXSCREEN);
			screenHeight = GetSystemMetrics(SM_CYSCREEN);

			newWidth = int(windowClientWidth * 1.0 * dpiX / 96.f) + (rect1.right - rect1.left) - (rect2.right - rect2.left);
			newHeight = int(windowClientHeight * 1.0 * dpiY / 96.f) + (rect1.bottom - rect1.top) - (rect2.bottom - rect2.top);

			MoveWindow(hwnd, (screenWidth - newWidth) / 2, (screenHeight - newHeight) / 2, newWidth, newHeight, true);
			initializeGame();
			UpdateWindow(hwnd);
			SetTimer(hwnd, gameTimerID, timerInterval, NULL);
			break;
		}
		case WM_TIMER:
		{
			gameTimer(hwnd, (UINT)wparam);
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDC = BeginPaint(hwnd, &ps);
			if (SUCCEEDED(renderTarget->BindDC(hDC, &ps.rcPaint))) {
				renderWindow(hwnd);
			}
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_ERASEBKGND:
		{
			break;
		}
		case WM_DESTROY:
		{
			KillTimer(hwnd, gameTimerID);
			terminateGraphics();
			PostQuitMessage(0);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			gameMouseDown(hwnd, XplorerLeftButton, LOWORD(lparam), HIWORD(lparam));
			break;
		}
		case WM_RBUTTONDOWN:
		{
			MessageBox(hwnd, "You clicked the right mouse button down.", "Info", 0);
			gameMouseDown(hwnd, XplorerRightButton, LOWORD(lparam), HIWORD(lparam));
			break;
		}
		case WM_LBUTTONUP:
		{
			gameMouseUp(hwnd, XplorerLeftButton, LOWORD(lparam), HIWORD(lparam));
			break;
		}
		case WM_RBUTTONUP:
		{
			gameMouseUp(hwnd, XplorerRightButton, LOWORD(lparam), HIWORD(lparam));
			break;
		}
		case WM_MOUSEMOVE:
		{
			gameMouseMove(hwnd, 0, LOWORD(lparam), HIWORD(lparam));
			break;
		}
		case WM_KEYDOWN:
		{
			gameKeyDown(hwnd, (int)wparam);
			break;
		}
		case WM_KEYUP:
		{
			gameKeyUp(hwnd, (int)wparam);
			break;
		}
		default:
		{
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;

	memset(&wc, 0, sizeof wc);
	wc.cbSize = sizeof wc;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	char titleBuffer[bufferSize];
	sprintf_s(titleBuffer, bufferSize, "%s %s %s", appName, appVersionString, appEdition);

	HWND gameWindow = CreateWindowEx(WS_EX_CLIENTEDGE, "WindowClass", titleBuffer, WS_VISIBLE | WS_OVERLAPPED | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, windowClientWidth, windowClientHeight, NULL, NULL, hInstance, NULL);

	if (gameWindow == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

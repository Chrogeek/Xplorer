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

#define _CRT_SECURE_NO_WARNINGS

#include "defs.h"
#include <windows.h>
#include <d2d1.h>
#include "utility.h"
#include "handler.h"
#include "main.h"

extern ID2D1DCRenderTarget *mainRenderer;
extern ID2D1Factory *d2dFactory;
extern float dpiX, dpiY;

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

			newWidth = int(windowClientWidth * dpiX / 96.f) + (rect1.right - rect1.left) - (rect2.right - rect2.left);
			newHeight = int(windowClientHeight * dpiY / 96.f) + (rect1.bottom - rect1.top) - (rect2.bottom - rect2.top);

			MoveWindow(hwnd, (screenWidth - newWidth) / 2, (screenHeight - newHeight) / 2, newWidth, newHeight, true);
			initializeGame();
			UpdateWindow(hwnd);
			SetTimer(hwnd, gameTimerID, timerInterval, nullptr);
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
			if (SUCCEEDED(mainRenderer->BindDC(hDC, &ps.rcPaint))) {
				gamePaint(hwnd);
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
			terminateGame();
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
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	if (!RegisterClassEx(&wc)) {
		MessageBox(nullptr, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	char titleBuffer[bufferSize];
	sprintf_s(titleBuffer, bufferSize, "%s %s %s", appName, appVersionString, appEdition);

	HWND gameWindow = CreateWindowEx(WS_EX_CLIENTEDGE, "WindowClass", titleBuffer, WS_VISIBLE | WS_OVERLAPPED | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, windowClientWidth, windowClientHeight, nullptr, nullptr, hInstance, nullptr);

	if (gameWindow == nullptr) {
		MessageBox(nullptr, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

#ifndef FUI_H
#define FUI_H

#include "fuiTimer.h"
#include "fuiObject.h"

HINSTANCE fuiInstance;
HINSTANCE fuiPreviousInstance;
LPSTR fuiCommandLine;
int fuiCommandShow;
HWND fuiMainhWindow;
void fuiMain();

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	::fuiInstance = hInstance;
	::fuiPreviousInstance = hPrevInstance;
	::fuiCommandLine = lpCmdLine;
	::fuiCommandShow = nCmdShow;

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
	fuiMainhWindow = CreateWindowEx(WS_EX_CLIENTEDGE, "WindowClass", "FUI In C++ Demo", WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInstance, NULL);
	if (fuiMainhWindow == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	fuiMain();

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

#endif

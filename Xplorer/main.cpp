#include <windows.h>
#include <wincodec.h>
#include <d2d1.h>
#include <cstdio>
#include <cassert>
#include <vector>
#include <map>
#include "defs.h"
#include "utility.h"
#include "handler.h"
using namespace std;

extern ID2D1Factory *d2dFactory;
extern IWICImagingFactory *imageFactory;
extern vector<buttonUI> buttons;
extern map<int, int> buttonID;

void initializeGraphics() {
	CoInitialize(NULL);
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);
	HRESULT result = CoCreateInstance(CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void **>(&imageFactory));
	addButton(buttonStart, 300, 300, 200, 80, L"images/startButton.png");
	addButton(buttonExit, windowClientWidth - 135, windowClientHeight - 60, 120, 45, L"images/exitButton.png");
}

void terminateGraphics() {
	safeRelease(imageFactory);
	safeRelease(d2dFactory);
}
/*
void drawRectangle(HWND hwnd) {
	ID2D1RenderTarget *d2dRenderTarget = NULL;
	ID2D1SolidColorBrush *d2dBlackBrush = NULL;
	HRESULT hr;
	if (d2dRenderTarget != NULL) return;
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);
	RECT rc;
	GetClientRect(hwnd, &rc);
	hr = d2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)), &d2dRenderTarget);

	char s[100];
	sprintf_s(s, 100, "%d", hr);
	OutputDebugStringA(s);

	d2dRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &d2dBlackBrush);
	d2dRenderTarget->BeginDraw();
	d2dRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	d2dRenderTarget->DrawRectangle(D2D1::RectF(100.f, 100.f, 500.f, 500.f), d2dBlackBrush);
	d2dRenderTarget->EndDraw();
}
*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
		case WM_CREATE:
		{
			initializeGraphics();
			FLOAT dpiX, dpiY;
			d2dFactory->GetDesktopDpi(&dpiX, &dpiY);

			// Adjust window size to make its internal area (client area) just the size of windowClientWidth * windowClientHeight.
			RECT rect1, rect2;
			GetWindowRect(hwnd, &rect1);  // rect1: left/top/right/bottom of the whole window
			GetClientRect(hwnd, &rect2);  // rect2: left/top/right/bottom of client area

			MoveWindow(hwnd, rect1.left, rect1.top, (rect1.right - rect1.left) - (rect2.right - rect2.left) + (int)(windowClientWidth * 1.0 * dpiX / 96.f), (rect1.bottom - rect1.top) - (rect2.bottom - rect2.top) + (int)(windowClientHeight * 1.0 * dpiY / 96.f), true);
			initializeGame();
		}
		case WM_PAINT:
		{
			gamePaint(hwnd);
			break;
		}
		case WM_ERASEBKGND:
		{
			break;
		}
		case WM_DESTROY:
		{
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

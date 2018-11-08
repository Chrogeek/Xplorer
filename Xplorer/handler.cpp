#include <d2d1.h>
#include <windows.h>
#include <wincodec.h>
#include "defs.h"
#include "game.h"
#include "handler.h"
#include "utility.h"

extern int currentStage;
extern ID2D1Factory *d2dFactory;
extern IWICImagingFactory *imageFactory;
extern vector<buttonUI> buttons;
extern HRESULT renderGame(ID2D1HwndRenderTarget *, IWICImagingFactory *);
extern XplorerResult newStage(const WCHAR *);
extern bool isTimerOn;
extern XplorerDirection face;

bool isKeyDown[128];

void gameTimer(HWND hwnd, UINT timerID) {
	updateHero();
	//gamePaint(hwnd);
	UpdateWindow(hwnd);
}

void initializeGame() {
	currentStage = stageMainMenu;
}

void gameKeyDown(HWND hwnd, int keyCode) {
	if (!isInInterval(keyCode, 0, 127)) return;
	isKeyDown[keyCode] = true;
	if (currentStage >= stageTutorial) {
		if (keyCode == VK_LEFT) face = directionLeft;
		else if (keyCode == VK_RIGHT) face = directionRight;
	}
}

void gameKeyUp(HWND hwnd, int keyCode) {
	if (!isInInterval(keyCode, 0, 127)) return;
	isKeyDown[keyCode] = false;
}

void gameMouseDown(HWND hwnd, int button, int X, int Y) {}

void gameMouseUp(HWND hwnd, int button, int X, int Y) {
	int buttonClicked = getClickedButtonID(1.f * X, 1.f * Y);
	switch (currentStage) {
		case stageMainMenu:
		{
			if (buttonClicked == buttonExit) {
				PostQuitMessage(0);
			} else if (buttonClicked == buttonStart) {
				MessageBox(NULL, "Ready to start the game!", "Info", 0);
				disableAllButtons();
				newStage(L"levels/1.txt");
				currentStage = 1;
				isTimerOn = true;
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

void gameMouseMove(HWND hwnd, int button, int X, int Y) {}

HRESULT gamePaint(HWND hwnd) {
	HRESULT result = S_OK;
	RECT rect;
	ID2D1HwndRenderTarget *renderTarget = NULL;
	GetClientRect(hwnd, &rect);
	result = d2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top)), &renderTarget);
	if (SUCCEEDED(result)) {
		renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
		renderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
		switch (currentStage) {
			case stageMainMenu:
			{
				ID2D1Bitmap *bitmapBackground = NULL;
				result = loadBitmapFromFile(renderTarget, imageFactory, L"images/bg_blue.png", windowClientWidth, windowClientHeight, &bitmapBackground);
				if (SUCCEEDED(result)) {
					renderTarget->BeginDraw();
					renderTarget->DrawBitmap(bitmapBackground, makeRectF(0, 0, windowClientWidth, windowClientHeight));
					getButton(buttonStart)->visible = true;
					getButton(buttonExit)->visible = true;
					drawButton(getButton(buttonStart), renderTarget, imageFactory);
					drawButton(getButton(buttonExit), renderTarget, imageFactory);
					renderTarget->EndDraw();
				}
				safeRelease(bitmapBackground);
				break;
			}
			default:
			{
				renderTarget->BeginDraw();
				renderGame(renderTarget, imageFactory);
				renderTarget->EndDraw();
				break;
			}
		}
	}
	safeRelease(renderTarget);
	return result;
}

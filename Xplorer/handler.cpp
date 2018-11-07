#include <d2d1.h>
#include <windows.h>
#include <wincodec.h>
#include "defs.h"
#include "handler.h"
#include "utility.h"

extern int currentStage;
extern ID2D1Factory *d2dFactory;
extern IWICImagingFactory *imageFactory;
extern vector<buttonUI> buttons;

bool isKeyDown[128];

void initializeGame() {
	currentStage = stageMainMenu;
}

void gameKeyDown(HWND hwnd, int keyCode) {
	if (!isInInterval(keyCode, 0, 127)) return;
	isKeyDown[keyCode] = true;
}

void gameKeyUp(HWND hwnd, int keyCode) {
	if (!isInInterval(keyCode, 0, 127)) return;
	isKeyDown[keyCode] = false;
}

void gameMouseDown(HWND hwnd, int button, int X, int Y) {}

void gameMouseUp(HWND hwnd, int button, int X, int Y) {
	int buttonClicked = getClickedButtonID(X, Y);
	switch (currentStage) {
		case stageMainMenu:
		{
			if (buttonClicked == buttonExit) {
				PostQuitMessage(0);
			} else if (buttonClicked == buttonStart) {
			//	MessageBox(hwnd, "You clicked the start button!", "Info", MB_SYSTEMMODAL);
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
	ID2D1HwndRenderTarget *renderTarget;
	GetClientRect(hwnd, &rect);
	result = d2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, D2D1::SizeU(rect.right - rect.left, rect.bottom - rect.top)), &renderTarget);
	errorCheck(result);
	switch (currentStage) {
		case stageMainMenu:
		{
			ID2D1Bitmap *bitmapBackground = NULL;
			result = loadBitmapFromFile(renderTarget, imageFactory, L"images/bg_blue.png", windowClientWidth, windowClientHeight, &bitmapBackground);
			renderTarget->BeginDraw();
			renderTarget->DrawBitmap(bitmapBackground, makeRectF(0, 0, windowClientWidth, windowClientHeight));
			getButton(buttonStart)->visible = true;
			getButton(buttonExit)->visible = true;
			drawButton(getButton(buttonStart), renderTarget, imageFactory);
			drawButton(getButton(buttonExit), renderTarget, imageFactory);
			renderTarget->EndDraw();
			break;
		}
		default:
		{
			break;
		}
	}
	return result;
}

#include "defs.h"
#include <d2d1.h>
#include <dwrite.h>
#include "game.h"
#include "utility.h"
#include "animation.h"
#include "gameFrame.h"
#include "gameLevel.h"
#include "uiMainMenu.h"

extern gameFrame *mainFrame, *currentFrame;
extern ID2D1Factory *d2dFactory;
extern IWICImagingFactory *imageFactory;
extern IDWriteFactory *writeFactory;
extern ID2D1DCRenderTarget *mainRenderer;
extern animation animator;
extern buttonUI *buttons[maxButton + 1];
extern int currentChapter, currentLevel;

void loadMainFrame() {
	HRESULT result = S_OK;

	safeNew(mainFrame, gameFrame);
	if (SUCCEEDED(result)) {
		result = mainRenderer->CreateCompatibleRenderTarget(makeSizeF((float)windowClientWidth, (float)windowClientHeight), &mainFrame->renderer);
	}
	
	ID2D1Bitmap *background = nullptr;
	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(mainFrame->renderer, imageFactory, L"images/bg_blue.png", windowClientWidth, windowClientHeight, &background);
	}
	if (SUCCEEDED(result)) {
		mainFrame->renderer->BeginDraw();
		mainFrame->renderer->DrawBitmap(background, makeRectF(0.f, 0.f, (float)windowClientWidth, (float)windowClientHeight));
		disableAllButtons();
		buttonUI *btnStart = buttons[buttonStart], *btnExit = buttons[buttonExit], *btnLoad = buttons[buttonLoad], *btnTutorial = buttons[buttonTutorial];
		if (btnStart) btnStart->enabled = true;
		if (btnExit) btnExit->enabled = true;
		if (btnLoad) btnLoad->enabled = true;
		if (btnTutorial) btnTutorial->enabled = true;
		drawButton(mainFrame->renderer, btnStart);
		drawButton(mainFrame->renderer, btnExit);
		drawButton(mainFrame->renderer, btnLoad);
		drawButton(mainFrame->renderer, btnTutorial);
		result = mainFrame->renderer->GetBitmap(&mainFrame->bitmap);
	}
	if (SUCCEEDED(result)) {
		mainRenderer->CreateBitmapBrush(mainFrame->bitmap, &mainFrame->brush);
		mainFrame->renderer->EndDraw();
	}
	mainFrame->enter = showMainMenu;
	mainFrame->exit = leaveMainMenu;
}

void showMainMenuFinish() {
	currentFrame = mainFrame;
}

void showMainMenu() {
	currentChapter = currentLevel = -1;
	animator.startAnimation(mainFrame->bitmap, new linearAnimation(timeGetTime(), longAnimation), crossIn, showMainMenuFinish);
}

void leaveMainMenu() {
	animator.startAnimation(mainFrame->bitmap, new linearAnimation(timeGetTime(), longAnimation), crossOut, leaveMainMenuFinish);
}

void leaveMainMenuFinish() {
	mainRenderer->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	loadNextFrame();
}

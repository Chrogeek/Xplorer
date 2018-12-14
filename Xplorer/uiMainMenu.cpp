#include "defs.h"
#include <cmath>
#include <d2d1.h>
#include <dwrite.h>
#include "game.h"
#include "utility.h"
#include "animation.h"
#include "gameFrame.h"
#include "gameLevel.h"
#include "uiMainMenu.h"
#include "music.h"
#include "particles.h"

extern gameFrame *mainFrame, *currentFrame;
extern ID2D1Factory *d2dFactory;
extern IWICImagingFactory *imageFactory;
extern IDWriteFactory *writeFactory;
extern ID2D1DCRenderTarget *mainRenderer;
extern animation animator;
extern buttonUI *buttons[maxButton + 1];
extern int currentChapter, currentLevel;
extern ID2D1SolidColorBrush *brushWhite;

void loadMainMenuButtons() {
	disableAllButtons();
	buttonUI *btnStart = buttons[buttonNew], *btnExit = buttons[buttonExit], *btnLoad = buttons[buttonLoad],
		*btnStatistics = buttons[buttonStatistics], *btnOptions = buttons[buttonOptions], *btnAbout = buttons[buttonAbout];
	if (btnStart) btnStart->enabled = true;
	if (btnExit) btnExit->enabled = true;
	if (btnLoad) btnLoad->enabled = true;
	if (btnStatistics) btnStatistics->enabled = true;
	if (btnOptions) btnOptions->enabled = true;
	if (btnAbout) btnAbout->enabled = true;
}

void loadMainFrame() {
	HRESULT result = S_OK;

	safeNew(mainFrame, gameFrame);
	if (SUCCEEDED(result)) {
		result = mainRenderer->CreateCompatibleRenderTarget(makeSizeF((float)windowClientWidth, (float)windowClientHeight), &mainFrame->renderer);
	}

	ID2D1Bitmap *background = nullptr;
	ID2D1Bitmap *bigLogo = nullptr;
	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(mainFrame->renderer, imageFactory, L"images/bg_main.jpg", windowClientWidth, windowClientHeight, &background);
	}
	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(mainFrame->renderer, imageFactory, L"images/xplorer.png", 453, 198, &bigLogo);
	}
	if (SUCCEEDED(result)) {
		mainFrame->renderer->BeginDraw();
		mainFrame->renderer->DrawBitmap(background, makeRectF(0.f, 0.f, (float)windowClientWidth, (float)windowClientHeight));
		mainFrame->renderer->DrawBitmap(bigLogo, makeRectF((windowClientWidth - 453.f) / 2.f, 40.f, (windowClientWidth + 453.f) / 2.f, 238.f));
		safeRelease(background);
		loadMainMenuButtons();
		drawButton(mainFrame->renderer, buttons[buttonNew]);
		drawButton(mainFrame->renderer, buttons[buttonExit]);
		drawButton(mainFrame->renderer, buttons[buttonLoad]);
		drawButton(mainFrame->renderer, buttons[buttonStatistics]);
		drawButton(mainFrame->renderer, buttons[buttonOptions]);
		drawButton(mainFrame->renderer, buttons[buttonAbout]);
		result = mainFrame->renderer->GetBitmap(&mainFrame->bitmap);
	}
	if (SUCCEEDED(result)) {
		mainRenderer->CreateBitmapBrush(mainFrame->bitmap, &mainFrame->brush);
		mainFrame->renderer->EndDraw();
	}
	mainFrame->enter = showMainMenu;
	mainFrame->leave = leaveMainMenu;
	mainFrame->render = renderMainMenu;
}

void renderMainMenu() {
	renderRain(0.08f);
}

void showMainMenuFinish() {
	currentFrame = mainFrame;
}

void showMainMenu() {
	makeRain();
	playMusic(musicMain);
	stopMusic(musicAbout);
	stopMusic(musicGame);
	loadMainMenuButtons();
	currentChapter = currentLevel = -1;
	animator.startAnimation(mainFrame->bitmap, new linearAnimation(timeGetTime(), longAnimation), showMainMenuFrame, showMainMenuFinish);
}

void showMainMenuFrame(ID2D1Bitmap *bitmap, double progress) {
	crossIn(bitmap, progress);
	renderRain(0.08f * (float)progress);
}

void leaveMainMenuFrame(ID2D1Bitmap *bitmap, double progress) {
	crossOut(bitmap, progress);
	renderRain(0.08f * float(1.0 - progress));
}

void leaveMainMenu() {
	animator.startAnimation(mainFrame->bitmap, new linearAnimation(timeGetTime(), longAnimation), leaveMainMenuFrame, leaveMainMenuFinish);
}

void leaveMainMenuFinish() {
	mainRenderer->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	loadNextFrame();
}

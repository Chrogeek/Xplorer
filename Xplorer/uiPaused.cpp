#include "defs.h"
#include <string>
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include "game.h"
#include "utility.h"
#include "animation.h"
#include "gameFrame.h"
#include "uiPaused.h"
#include "gameLevel.h"

extern gameFrame *pausedFrame, *nextFrame;
extern buttonUI *buttons[maxButton + 1];
extern ID2D1Factory *d2dFactory;
extern IWICImagingFactory *imageFactory;
extern IDWriteFactory *writeFactory;
extern ID2D1SolidColorBrush *brushWhite, *brushBlack;
extern ID2D1DCRenderTarget *mainRenderer;

extern gameFrame *currentFrame;
extern gameManager gameMaster;

extern int currentChapter, currentLevel;
extern ID2D1Bitmap *heroImage;
extern gameHero hero;
extern animation animator;

//ID2D1Bitmap *currentGameFrame = nullptr;
//ID2D1BitmapRenderTarget *tmpGameRenderer = nullptr;
// extern ID2D1Bitmap *currentGameFrame;

void loadPausedFrame() {
	HRESULT result = S_OK;
	safeNew(pausedFrame, gameFrame);
	pausedFrame->render = renderPaused;
	if (SUCCEEDED(result)) {
		result = mainRenderer->CreateCompatibleRenderTarget(makeSizeF((float)windowClientWidth, (float)windowClientHeight), &pausedFrame->renderer);
	}
	if (SUCCEEDED(result)) {
		pausedFrame->renderer->BeginDraw();

		drawButton(pausedFrame->renderer, buttons[buttonContinue]);
		drawButton(pausedFrame->renderer, buttons[buttonRetry]);
		drawButton(pausedFrame->renderer, buttons[buttonHome]);

		drawText(pausedFrame->renderer, writeFactory, std::wstring(gameFontName) + L" Light", false, false, 70, std::wstring(L"Paused"), makeRectF(200.f, 20.f, windowClientWidth - 200.f, 95.f), brushWhite);

		pausedFrame->renderer->EndDraw();
	}
	if (SUCCEEDED(result)) {
		result = pausedFrame->renderer->GetBitmap(&pausedFrame->bitmap);
	}
	pausedFrame->enter = showPaused;
	pausedFrame->exit = leavePaused;
}

void renderPaused() {
	mainRenderer->DrawBitmap(pausedFrame->bitmap, makeRectF(0.f, 0.f, (float)windowClientWidth, (float)windowClientHeight));
}

void showPausedFinish() {
	currentFrame = pausedFrame;
}

void showPaused() {
	animator.startAnimation(pausedFrame->bitmap, new linearAnimation(timeGetTime(), shortAnimation), showPausedFrame, showPausedFinish);
}

void showPausedFrame(ID2D1Bitmap *bitmap, double progress) {
	crossIn(bitmap, progress);
}

void leavePaused() {
	animator.startAnimation(pausedFrame->bitmap, new linearAnimation(timeGetTime(), shortAnimation), leavePausedFrame, leavePausedFinish);
}

void leavePausedFrame(ID2D1Bitmap *bitmap, double progress) {
	crossOut(bitmap, progress);
}

void leavePausedFinish() {
	loadNextFrame();
}

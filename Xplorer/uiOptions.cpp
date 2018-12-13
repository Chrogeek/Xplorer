#include "defs.h"
#include <string>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include "game.h"
#include "utility.h"
#include "animation.h"
#include "gameFrame.h"
#include "uiOptions.h"
#include "gameLevel.h"
#include "particles.h"
#include "music.h"
#include "resource.h"

extern gameFrame *optionsFrame, *nextFrame;
extern buttonUI *buttons[maxButton + 1];
extern ID2D1Factory *d2dFactory;
extern IWICImagingFactory *imageFactory;
extern IDWriteFactory *writeFactory;
extern ID2D1SolidColorBrush *brushWhite, *brushBlack;
extern ID2D1DCRenderTarget *mainRenderer;

extern gameFrame *currentFrame;

extern animation animator;
extern std::vector<particle *> particles;

extern gameManager gameMaster;

bool saveDeleted;

void loadOptionsFrame() {
	HRESULT result = S_OK;
	safeNew(optionsFrame, gameFrame);
	optionsFrame->render = renderOptions;
	if (SUCCEEDED(result)) {
		result = mainRenderer->CreateCompatibleRenderTarget(makeSizeF((float)windowClientWidth, (float)windowClientHeight), &optionsFrame->renderer);
	}
	if (SUCCEEDED(result)) {
		optionsFrame->renderer->BeginDraw();
		drawText(optionsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_EXTRA_LIGHT, false, 70.f,
			std::wstring(L"Options"),
			makeRectF(0.f, 20.f, windowClientWidth, 95.f), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
		);
		drawText(optionsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_REGULAR, false, 24.f, L"Volume", makeRectF(40.f, 160.f, 180.f, 190.f), DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite);
		drawButton(optionsFrame->renderer, buttons[buttonOptionsBack]);
		drawButton(optionsFrame->renderer, buttons[buttonDeleteSave]);
	}
	if (SUCCEEDED(result)) {
		result = optionsFrame->renderer->GetBitmap(&optionsFrame->bitmap);
		optionsFrame->renderer->EndDraw();
	}
	optionsFrame->enter = showOptions;
	optionsFrame->leave = leaveOptions;

	makeRain();
}

void renderOptionsFrame(float opacity) {
	ID2D1SolidColorBrush *brush = nullptr;
	if (mainRenderer->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, opacity), &brush) == S_OK) {
		mainRenderer->DrawRectangle(makeRectF(190.f, 160.f, windowClientWidth - 110.f, 190.f), brush);
		mainRenderer->FillRectangle(makeRectF(190.f, 160.f, (float)((windowClientWidth - 300.f) * (double)gameMaster.saveData[itemVolume]) + 190.f, 190.f), brush);
		drawText(mainRenderer, writeFactory, gameFontName, saveDeleted ? DWRITE_FONT_WEIGHT_LIGHT : DWRITE_FONT_WEIGHT_SEMI_BOLD, true, 18.f, saveDeleted ? L"Deleted." : L"Warning!\nThis cannot be undone.", makeRectF(225.f, windowClientHeight - 55.f, windowClientWidth / 2.f, windowClientHeight - 15.f), DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brush);
	}
	safeRelease(brush);
}

void renderOptions() {
	mainRenderer->DrawBitmap(optionsFrame->bitmap, makeRectF(0.f, 0.f, (float)windowClientWidth, (float)windowClientHeight));
	renderOptionsFrame(1.f);
	renderRain(0.08f);
}

void showOptionsFinish() {
	currentFrame = optionsFrame;
}

void showOptions() {
	saveDeleted = false;
	disableAllButtons();
	buttons[buttonOptionsBack]->enabled = true;
	buttons[buttonDeleteSave]->enabled = true;
	buttons[buttonVolumeSlider]->enabled = true;
	animator.startAnimation(optionsFrame->bitmap, new linearAnimation(timeGetTime(), shortAnimation), showOptionsFrame, showOptionsFinish);
}

void showOptionsFrame(ID2D1Bitmap *bitmap, double progress) {
	fadeIn(bitmap, progress);
	renderOptionsFrame((float)progress);
	renderRain(0.08f * (float)progress);
}

void leaveOptions() {
	animator.startAnimation(optionsFrame->bitmap, new linearAnimation(timeGetTime(), shortAnimation), leaveOptionsFrame, leaveOptionsFinish);
}

void leaveOptionsFrame(ID2D1Bitmap *bitmap, double progress) {
	fadeOut(bitmap, progress);
	renderOptionsFrame((float)(1.0 - progress));
	renderRain(0.08f * float(1.0 - progress));
}

void leaveOptionsFinish() {
	loadNextFrame();
}
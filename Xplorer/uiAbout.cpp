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
#include "uiAbout.h"
#include "gameLevel.h"
#include "particles.h"
#include "music.h"
#include "resource.h"

extern gameFrame *aboutFrame, *nextFrame;
extern buttonUI *buttons[maxButton + 1];
extern ID2D1Factory *d2dFactory;
extern IWICImagingFactory *imageFactory;
extern IDWriteFactory *writeFactory;
extern ID2D1SolidColorBrush *brushWhite, *brushBlack;
extern ID2D1DCRenderTarget *mainRenderer;

extern gameFrame *currentFrame;

extern animation animator;
extern std::vector<particle *> particles;

extern bool isKeyDown[128];

double position;
DWORD lastTime;
std::string text;
std::wstring wtext;
D2D1_SIZE_F textSize;
float fullHeight;

void loadAboutFrame() {
	HRESULT result = S_OK;
	safeNew(aboutFrame, gameFrame);
	aboutFrame->render = renderAbout;
	if (SUCCEEDED(result)) {
		result = mainRenderer->CreateCompatibleRenderTarget(makeSizeF((float)windowClientWidth, (float)windowClientHeight), &aboutFrame->renderer);
	}
	if (SUCCEEDED(result)) {
		aboutFrame->renderer->BeginDraw();
		drawButton(aboutFrame->renderer, buttons[buttonAboutContinue]);
		drawButton(aboutFrame->renderer, buttons[buttonViewOnGitHub]);
		ID2D1Bitmap *logo = nullptr;
		result = loadBitmapFromFile(aboutFrame->renderer, imageFactory, L"images/logo.png", 256, 256, &logo);
		if (SUCCEEDED(result)) {
			aboutFrame->renderer->DrawBitmap(logo, makeRectF((windowClientWidth / 2.f - 256.f) / 2.f, 25.f, (windowClientWidth / 2.f - 256.f) / 2.f + 256.f, 281.f), 1.f);
			safeRelease(logo);
		}
	}
	if (SUCCEEDED(result)) {
		result = drawText(aboutFrame->renderer, writeFactory, gameFontName, DWRITE_FONT_WEIGHT_SEMI_BOLD, false, 36.f, stringToWidestring(appName), makeRectF(10.f, 316.f, windowClientWidth / 2.f - 10.f, (float)windowClientHeight), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, brushWhite);
	}
	if (SUCCEEDED(result)) {
		result = drawText(aboutFrame->renderer, writeFactory, gameFontName, DWRITE_FONT_WEIGHT_SEMI_BOLD, true, 24.f, appCopyright, makeRectF(10.f, 370.f, windowClientWidth / 2.f - 10.f, (float)windowClientHeight), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, brushWhite);
		aboutFrame->renderer->EndDraw();
	}
	if (SUCCEEDED(result)) {
		result = aboutFrame->renderer->GetBitmap(&aboutFrame->bitmap);
	}
	aboutFrame->enter = showAbout;
	aboutFrame->leave = leaveAbout;

	std::ifstream fin("data/about.txt");
	if (fin) {
		std::stringstream buffer;
		buffer << fin.rdbuf();
		text = std::string(buffer.str());
	} else {
		text = "";
	}
	wtext = stringToWidestring(text);
	fin.close();

	getTextSize(writeFactory, gameFontName, DWRITE_FONT_WEIGHT_SEMI_LIGHT, false, 20.f, wtext, DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, windowClientWidth / 2.f - 20.f, 0.f, textSize);
	fullHeight = textSize.height + windowClientHeight;

	makeRain();
}

void renderAboutFrame(float opacity) {
	DWORD thisTime = timeGetTime();
	if (lastTime == thisTime) return;
	double interval = (thisTime - lastTime) / 1000.0;
	position += interval * 80.0 * ((isKeyDown[VK_UP] || isKeyDown[VK_SPACE]) ? 2.5 : 1.0) * (isKeyDown[VK_DOWN] ? 0.5 : 1.0);
	if (position > fullHeight) position -= fullHeight;

	ID2D1SolidColorBrush *brush = nullptr;
	if (mainRenderer->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, opacity), &brush) == S_OK) {
		drawText(mainRenderer, writeFactory, gameFontName, DWRITE_FONT_WEIGHT_SEMI_LIGHT, false, 20.f, wtext, makeRectF(windowClientWidth / 2.f + 10.f, -(float)position + windowClientHeight, windowClientWidth - 10.f, windowClientHeight * 2.f), DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, brush);
	}
	safeRelease(brush);
	renderRain(opacity * 0.08f);
	lastTime = thisTime;
}

void renderAbout() {
	mainRenderer->DrawBitmap(aboutFrame->bitmap, makeRectF(0.f, 0.f, (float)windowClientWidth, (float)windowClientHeight));
	renderAboutFrame(1.f);
}

void showAboutFinish() {
	currentFrame = aboutFrame;
}

void showAbout() {
	lastTime = timeGetTime();
	position = 0.0;
	disableAllButtons();
	buttons[buttonAboutContinue]->enabled = true;
	buttons[buttonViewOnGitHub]->enabled = true;
	animator.startAnimation(aboutFrame->bitmap, new linearAnimation(timeGetTime(), shortAnimation), showAboutFrame, showAboutFinish);
}

void showAboutFrame(ID2D1Bitmap *bitmap, double progress) {
	fadeIn(bitmap, progress);
	renderAboutFrame((float)progress);
}

void leaveAbout() {
	animator.startAnimation(aboutFrame->bitmap, new linearAnimation(timeGetTime(), shortAnimation), leaveAboutFrame, leaveAboutFinish);
}

void leaveAboutFrame(ID2D1Bitmap *bitmap, double progress) {
	fadeOut(bitmap, progress);
	renderAboutFrame(float(1.0 - progress));
}

void leaveAboutFinish() {
	loadNextFrame();
}

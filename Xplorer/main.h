#ifndef XPLORER_MAIN_H
#define XPLORER_MAIN_H

#include "defs.h"
#include <windows.h>
#include <wincodec.h>
#include <d2d1.h>
#include <dwrite.h>
#include <cstdio>
#include <fstream>
#include <cassert>
#include <vector>
#include <map>
#include "json.h"
#include "game.h"
#include "utility.h"
#include "handler.h"
#include "gameFrame.h"
#include "uiMainMenu.h"

using json = nlohmann::json;

extern ID2D1Factory *d2dFactory;
extern IDWriteFactory *writeFactory;
extern IDWriteTextFormat *textFormatNormal;
extern IWICImagingFactory *imageFactory;
extern ID2D1DCRenderTarget *mainRenderer;
extern buttonUI *buttons[maxButton + 1];
extern float dpiX, dpiY;
extern ID2D1Bitmap *bkgImage, *wallImage, *heroImage, *bitmapBackground, *savedImage;
extern ID2D1SolidColorBrush *brushBlack;
extern gameFrame *mainFrame, *inGameFrame;
//extern gameFrame *startAnimationFrame, *deathAnimationFrame;
extern gameFrame /**exitAnimationFrame*/ *animationFrame, *currentFrame;
extern json saveData;

HRESULT initializeGraphics() {
	HRESULT result = S_OK;
	if (SUCCEEDED(result)) {
		result = CoInitialize(nullptr);
	}
	if (SUCCEEDED(result)) {
		result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);
	}
	if (SUCCEEDED(result)) {
		result = CoCreateInstance(CLSID_WICImagingFactory1, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, reinterpret_cast<void **>(&imageFactory));
	}
	if (SUCCEEDED(result)) {
		result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown **>(&writeFactory));
	}
	if (SUCCEEDED(result)) {
		result = writeFactory->CreateTextFormat(gameFontName, nullptr, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20, L"en-us", &textFormatNormal);
	}
	if (SUCCEEDED(result)) {
		D2D1_RENDER_TARGET_PROPERTIES *renderTargetProperties = new D2D1_RENDER_TARGET_PROPERTIES;
		renderTargetProperties->dpiX = dpiX;
		renderTargetProperties->dpiY = dpiY;
		renderTargetProperties->type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		renderTargetProperties->usage = D2D1_RENDER_TARGET_USAGE_NONE;
		renderTargetProperties->minLevel = D2D1_FEATURE_LEVEL_DEFAULT;
		renderTargetProperties->pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
		renderTargetProperties->pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
		result = d2dFactory->CreateDCRenderTarget(renderTargetProperties, &mainRenderer);
	}
	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(mainRenderer, imageFactory, heroImageName, (int)(heroSize * 24), (int)heroSize, &heroImage);
	}
	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(mainRenderer, imageFactory, savedImageName, unitSize, unitSize, &savedImage);
	}
	if (SUCCEEDED(result)) {
		result = mainRenderer->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brushBlack);
	}

	buttons[buttonStart] = new buttonUI(300, 300, 200, 80, L"images/startButton.png");
	buttons[buttonExit] = new buttonUI(windowClientWidth - 135, windowClientHeight - 60, 120, 45, L"images/exitButton.png");

	if (SUCCEEDED(result)) {
		loadMainFrame();
		currentFrame = mainFrame;
	//	safeNew(startAnimationFrame, gameFrame);
	//	startAnimationFrame->render = renderStartAnimation;
		safeNew(inGameFrame, gameFrame);
		inGameFrame->render = renderGame;
	//	safeNew(exitAnimationFrame, gameFrame);
	//	exitAnimationFrame->render = renderExitAnimation;
		safeNew(animationFrame, gameFrame);
	}
	return result;
}

void terminateGraphics() {
	safeRelease(bkgImage);
	safeRelease(wallImage);
	safeRelease(heroImage);
	safeRelease(savedImage);
	safeRelease(bitmapBackground);
	safeRelease(textFormatNormal);
	safeRelease(brushBlack);
	safeRelease(imageFactory);
	safeRelease(mainRenderer);
	safeRelease(writeFactory);
	safeRelease(d2dFactory);
}

void terminateGame() {
	std::ofstream fout("./data/save.json");
	if (!fout.bad()) {
		fout << saveData;
		fout.close();
	}
}

#endif

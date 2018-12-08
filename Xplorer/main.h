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
#include "uiPaused.h"
#include "gameLevel.h"

using json = nlohmann::json;

extern ID2D1Factory *d2dFactory;
extern IDWriteFactory *writeFactory;
extern IDWriteTextFormat *textFormatNormal;
extern IWICImagingFactory *imageFactory;
extern ID2D1DCRenderTarget *mainRenderer;
extern buttonUI *buttons[maxButton + 1];
extern float dpiX, dpiY;
extern ID2D1Bitmap *bkgImage, *wallImage, *heroImage, *bitmapBackground, *savedImage;
extern ID2D1SolidColorBrush *brushBlack, *brushWhite;
extern gameFrame *mainFrame, *inGameFrame;
//extern gameFrame *startAnimationFrame, *deathAnimationFrame;
extern gameFrame /**exitAnimationFrame*/ *animationFrame, *currentFrame;
//extern json saveData;
extern gameManager gameMaster;

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
	d2dFactory->GetDesktopDpi(&dpiX, &dpiY);
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
	if (SUCCEEDED(result)) {
		result = mainRenderer->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &brushWhite);
	}

	buttons[buttonStart] = new buttonUI(300, 300, 200, 80, L"images/startButton.png");
	buttons[buttonExit] = new buttonUI(windowClientWidth - 135, windowClientHeight - 60, 120, 45, L"images/exitButton.png");
	buttons[buttonContinue] = new buttonUI((windowClientWidth - 140) / 2, 200, 280, 105, L"images/continueButton.png");
	buttons[buttonRetry] = new buttonUI((windowClientWidth - 140) / 2, 335, 280, 105, L"images/retryButton.png");
	buttons[buttonHome] = new buttonUI((windowClientWidth - 140) / 2, 470, 280, 105, L"images/homeButton.png");
	buttons[buttonLoad] = new buttonUI(300, 420, 200, 75, L"images/loadButton.png");
	buttons[buttonTutorial] = new buttonUI(300, 520, 200, 75, L"images/tutorialButton.png");

	if (SUCCEEDED(result)) {
		loadMainFrame();

		safeNew(inGameFrame, gameFrame);
		inGameFrame->render = renderGame;
		inGameFrame->enter = gameFrameEnter;
		inGameFrame->exit = gameFrameLeave;

		safeNew(animationFrame, gameFrame);

		loadPausedFrame();
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
	safeRelease(brushWhite);
	safeRelease(imageFactory);
	safeRelease(mainRenderer);
	safeRelease(writeFactory);
	safeRelease(d2dFactory);
}

void terminateGame() {
	//std::ofstream fout("./data/save.json");
	//if (!fout.bad()) {
		//fout << saveData;
		//fout.close();
//	}
	std::string p1 = "./chapters";
	gameMaster.save(p1);
	for (int i = 0; i < (int)gameMaster.chapters.size(); ++i) {
		std::string p2 = p1 + "/" + intToString(i);
		gameMaster.chapters[i].save(p2);
		for (int j = 0; j < (int)gameMaster.chapters[i].levels.size(); ++j) {
			std::string p3 = p2 + "/" + intToString(j);
			gameMaster.chapters[i].levels[j].save(p3);
		}
	}
}

#endif

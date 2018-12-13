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

	buttons[buttonNew] = new buttonUI(100.0, 395.0, 200.0, 75.0, L"New", L"images/newButton.png");
	buttons[buttonLoad] = new buttonUI(windowClientWidth - 300.0, 395.0, 200.0, 75.0, L"Load", L"images/loadButton.png");

	double gap = (windowClientWidth - 480.f) / 5.f;

	buttons[buttonAbout] = new buttonUI(gap, windowClientHeight - 60.0, 120.0, 45.0, L"About", L"images/aboutButton.png");
	buttons[buttonOptions] = new buttonUI(gap * 2 + 120.0, windowClientHeight - 60.0, 120.0, 45.0, L"Options", L"images/optionsButton.png");
	buttons[buttonStatistics] = new buttonUI(gap * 3 + 240.0, windowClientHeight - 60.0, 120.0, 45.0, L"Statistics", L"images/statisticsButton.png");
	buttons[buttonExit] = new buttonUI(gap * 4 + 360.0, windowClientHeight - 60.0, 120.0, 45.0, L"Exit", L"images/exitButton.png");

	buttons[buttonContinue] = new buttonUI((windowClientWidth - 240.0) / 2.0, 250.0, 240.0, 90.0, L"Continue", L"images/continueButton.png");
	buttons[buttonRetry] = new buttonUI((windowClientWidth - 240.0) / 2.0, 385.0, 240.0, 90.0, L"Retry", L"images/retryButton.png");
	buttons[buttonHome] = new buttonUI((windowClientWidth - 240.0) / 2.0, 520.0, 240.0, 90.0, L"Home", L"images/homeButton.png");
	buttons[buttonStatisticsContinue] = new buttonUI(windowClientWidth / 2.0 - 100.0, (windowClientHeight + 505.0 - 75.0) / 2.0, 200.0, 75.0, L"Continue", L"images/continueButton.png");

	buttons[buttonAboutContinue] = new buttonUI((windowClientWidth / 2.0 - 200.0) / 2.0, windowClientHeight - 90.0, 200.0, 75.0, L"Back", L"images/homeButton.png");
	buttons[buttonViewOnGitHub] = new buttonUI((windowClientWidth / 2.0 - 200.0) / 2.0, windowClientHeight - 145.0, 200.0, 40.0, L"View on GitHub", L"images/viewGitHub.png");

	buttons[buttonOptionsBack] = new buttonUI(windowClientWidth - 215.0, windowClientHeight - 90.0, 200.0, 75.0, L"Back", L"images/homeButton.png");
	buttons[buttonDeleteSave] = new buttonUI(15.0, windowClientHeight - 55.0, 200.0, 40.0, L"Delete Saved Game", L"images/deleteSave.png");
	buttons[buttonVolumeSlider] = new buttonUI(190.0, 160.0, windowClientWidth - 300.0, 30.0);

	if (SUCCEEDED(result)) {
		loadMainFrame();

		safeNew(inGameFrame, gameFrame);
		inGameFrame->render = renderGame;
		inGameFrame->enter = gameFrameEnter;
		inGameFrame->leave = gameFrameLeave;

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

#include "defs.h"
#include <d2d1.h>
#include <dwrite.h>
#include "utility.h"
#include "gameFrame.h"
#include "uiMainMenu.h"

extern gameFrame *mainFrame;
extern ID2D1Factory *d2dFactory;
extern IWICImagingFactory *imageFactory;
extern IDWriteFactory *writeFactory;
extern ID2D1DCRenderTarget *mainRenderer;

extern buttonUI *buttons[maxButton + 1];

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
		buttonUI *btnStart = buttons[buttonStart], *btnExit = buttons[buttonExit];
		if (btnStart) btnStart->enabled = true;
		if (btnExit) btnExit->enabled = true;
		drawButton(mainFrame->renderer, btnStart);
		drawButton(mainFrame->renderer, btnExit);
		result = mainFrame->renderer->GetBitmap(&mainFrame->bitmap);
	}
	if (SUCCEEDED(result)) {
		mainRenderer->CreateBitmapBrush(mainFrame->bitmap, &mainFrame->brush);
		mainFrame->renderer->EndDraw();
	}
}

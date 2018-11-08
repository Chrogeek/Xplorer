#include <d2d1.h>
#include <stdio.h>
#include <algorithm>
#include "defs.h"
#include "utility.h"
#include "game.h"
using namespace std;

extern bool isKeyDown[128];
extern int currentStage;

const float heroVelocityX = maxVelocityX;

int blocks[mapWidth][mapHeight];
WCHAR backgroundImage[bufferSize];
int heroDirection;
float heroVelocityY;
float heroX, heroY;
XplorerDirection face;

void updateHero() {
	if (currentStage < stageTutorial) return;
	static UINT lastTime = timeGetTime();
	UINT thisTime = timeGetTime();
	if (thisTime == lastTime) return;
	float interval = (thisTime - lastTime) / 1000.0f;
	lastTime = thisTime;
	if (isKeyDown[VK_RIGHT]) {
		heroX += heroVelocityX * interval;
	} else if (isKeyDown[VK_LEFT]) {
		heroX -= heroVelocityX * interval;
	}
	/*float newVelocityY = min(maxVelocityY, heroVelocityY + interval * accelerationY);
	heroY += (newVelocityY + heroVelocityY) / 2.0f * interval;
	heroVelocityY = newVelocityY;*/
}

int getHeroState(XplorerDirection face, float vY, int idx) {
	if (dcmp(vY) != 0) return (face == directionLeft ? heroLeft : heroRight) + heroJumping + (idx % 2);
	int ans = (face == directionLeft ? heroLeft : heroRight);
	if (!isKeyDown[VK_LEFT] && !isKeyDown[VK_RIGHT]) ans += heroStanding;
	else ans += heroWalking;
	ans += (idx % 4);
	return ans;
}

HRESULT renderGame(ID2D1HwndRenderTarget *renderTarget, IWICImagingFactory *imageFactory) {
	static int heroCounter = 0;
	ID2D1Bitmap *bkgImage = NULL, *wallImage = NULL, *heroImage = NULL;
	HRESULT result = loadBitmapFromFile(renderTarget, imageFactory, backgroundImage, windowClientWidth, windowClientHeight, &bkgImage);
	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(renderTarget, imageFactory, wallImageName, heroSize, heroSize, &wallImage);
	}
	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(renderTarget, imageFactory, heroImageName, heroSize * 24, heroSize, &heroImage);
	}
	if (SUCCEEDED(result)) {
		renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
		for (int i = 0; i < mapWidth; ++i) for (int j = 0; j < mapHeight; ++j) {
			D2D1_RECT_F destRect = makeRectF(1.f * i * heroSize, 1.f * j * heroSize, 1.f * (i + 1) * heroSize, 1.f * (j + 1) * heroSize);
			switch (blocks[i][j]) {
				case blockWall:
				{
					renderTarget->DrawBitmap(wallImage, destRect);
					break;
				}
			}
			int heroStyle = getHeroState(face, heroVelocityY, heroCounter++);
			destRect = makeRectF(heroX, heroY, heroX + heroSize, heroY + heroSize);
			D2D1_RECT_F srcRect = makeRectF(1.f * heroStyle * heroSize, 0.f, 1.f * (heroStyle + 1) * heroSize, 1.f * heroSize);
			renderTarget->DrawBitmap(heroImage, destRect, 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, srcRect);
		}
	}
	safeRelease(bkgImage);
	safeRelease(wallImage);
	safeRelease(heroImage);
	return result;
}

XplorerResult loadMap(const WCHAR *fileName) {
	FILE *file = NULL;
	_wfopen_s(&file, fileName, L"r");
	if (file == NULL) return XplorerResult::fileNotFound;
	int n, m;
	fscanf_s(file, "%d%d", &n, &m);
	if (n == mapHeight && m == mapWidth) {
		for (int i = 0; i < n; ++i) for (int j = 0; j < m; ++j) {
			if (fscanf_s(file, "%d", blocks[j] + i) != 1) {
				fclose(file);
				return XplorerResult::fileBroken;
			}
			if (blocks[j][i] == blockStartingPoint) {
				heroX = 1.f * j * heroSize;
				heroY = 1.f * i * heroSize;
			}
		}
		fwscanf_s(file, L"%ls", backgroundImage, bufferSize);
		fclose(file);
	} else {
		return XplorerResult::fileBroken;
	}
	return XplorerResult::okay;
}

XplorerResult newStage(const WCHAR *fileName) {
	XplorerResult result;
	result = loadMap(fileName);
	heroVelocityY = 0.f;
	face = directionRight;
	return result;
}

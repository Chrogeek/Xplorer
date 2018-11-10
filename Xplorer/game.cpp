/*
	File Name: game.cpp

	This file implements main process of the game.
	------------------------------------------------------------
	Xplorer, yet another 2D jumping game
	Copyright (C) 2018 Chrogeek

	<https://github.com/Chrogeek/Xplorer>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <d2d1.h>
#include <dwrite.h>
#include <stdio.h>
#include <algorithm>
#include "defs.h"
#include "utility.h"
#include "game.h"

extern bool isKeyDown[128];
extern int currentStage;
extern ID2D1Bitmap *wallImage, *heroImage;
extern gameStage gameStages[maxStage + 1];

extern int heroDirection;
extern XplorerDirection face;

extern ID2D1DCRenderTarget *renderTarget;
extern IWICImagingFactory *imageFactory;
extern IDWriteFactory *writeFactory;
extern IDWriteTextFormat *textFormatNormal;
extern ID2D1SolidColorBrush *brushBlack;

extern pointVector heroPosition, heroVelocity;
extern int jumpCount;

void updateHero() {
	if (currentStage < stageTutorial) return;
	static UINT lastTime = timeGetTime();
	UINT thisTime = timeGetTime();
	if (thisTime == lastTime) return;
	float interval = (thisTime - lastTime) / 1000.0f;
	lastTime = thisTime;
	pointVector newVelocity = heroVelocity + gravityAcceleration * interval;
	if (face == directionRight) {
		if (isKeyDown[VK_RIGHT]) {
					newVelocity += interval * moveAcceleration;
			//newVelocity.vX = maxVelocity.vX;
		} else {
			//newVelocity.vX = 0.f;
					newVelocity -= interval * moveAcceleration;
					newVelocity.vX = maxValue(0.f, newVelocity.vX);
		}
	} else {
		if (isKeyDown[VK_LEFT]) {
					newVelocity -= interval * moveAcceleration;
			//newVelocity.vX = minVelocity.vX;
		} else {
			//newVelocity.vX = 0.f;
					newVelocity += interval * moveAcceleration;
					newVelocity.vX = minValue(0.f, newVelocity.vX);
		}
	}
	limitVelocity(newVelocity);
	heroPosition += (newVelocity + heroVelocity) / 2.f * interval;
	heroVelocity = newVelocity;
	heroPositionAdjust(heroPosition, heroVelocity);
}
int heroPositionAdjust(pointVector &position, pointVector &velocity) {
	// Fix the hero's position with the map data.
	// Return value shows additional info (reserved for more usage).
	gameStage &game = gameStages[currentStage];
	int ret = 0;
	for (int j = (int)floor(position.vY / heroSize); j < mapHeight && j <= (int)ceil(position.vY / heroSize); ++j)
		for (int i = (int)floor(position.vX / heroSize); i < mapWidth && i <= (int)ceil(position.vX / heroSize); ++i) {
			// We need to check the squares the hero involves only (no more than 4)
			switch (game.blocks[i][j]) {
				case blockStartingPoint: case blockCheckpoint: case blockEmpty:
				{
					continue;
				}
			}
			if (!isRectIntersect(position.vX, position.vY, position.vX + heroSize, position.vY + heroSize, (i * 1.f) * heroSize, (j * 1.f) * heroSize, (i + 1.f) * heroSize, (j + 1.f) * heroSize)) continue;
			pointVector delta = position - pointVector(i * 1.f * heroSize, j * 1.f * heroSize);
			ret = 1;
			switch (dcmp(abs(delta.vX), abs(delta.vY))) {
				case 0: // Crashed into a corner. Fix in both axises.
				{
					if (isInInterval(position.vX, (i + 0.5f) * heroSize + 0.5f, (i + 1.f) * heroSize - 0.5f)) position.vX = (i + 1.f) * heroSize, velocity.vX = 0.f, ret = 1;
					if (isInInterval(position.vX + heroSize, (i * 1.f) * heroSize + 0.5f, (i + 0.5f) * heroSize - 0.5f)) position.vX = (i - 1.f) * heroSize, velocity.vX = 0.f, ret = 1;
					if (isInInterval(position.vY, (j + 0.5f) * heroSize + 0.5f, (j + 1.f) * heroSize - 0.5f)) position.vY = (j + 1.f) * heroSize, velocity.vY = 0.f, ret = 1;
					if (isInInterval(position.vY + heroSize, (j * 1.f) * heroSize + 0.5f, (j + 0.5f) * heroSize - 0.5f)) position.vY = (j - 1.f) * heroSize, velocity.vY = 0.f, ret = 1, jumpCount = 0;
					break;
				}
				case 1: // Crashed in x-axis. Fix in x-coordinate only.
				{
					if (isInInterval(position.vX, (i + 0.5f) * heroSize + 0.5f, (i + 1.f) * heroSize - 0.5f)) position.vX = (i + 1.f) * heroSize, velocity.vX = 0.f, ret = 1;
					if (isInInterval(position.vX + heroSize, (i * 1.f) * heroSize + 0.5f, (i + 0.5f) * heroSize - 0.5f)) position.vX = (i - 1.f) * heroSize, velocity.vX = 0.f, ret = 1;
					break;
				}
				case -1: // Crashed in y-axis. Fix in x-coordinate only.
				{
					if (isInInterval(position.vY, (j + 0.5f) * heroSize + 0.5f, (j + 1.f) * heroSize - 0.5f)) position.vY = (j + 1.f) * heroSize, velocity.vY = 0.f, ret = 1;
					if (isInInterval(position.vY + heroSize, (j * 1.f) * heroSize + 0.5f, (j + 0.5f) * heroSize - 0.5f)) position.vY = (j - 1.f) * heroSize, velocity.vY = 0.f, ret = 1, jumpCount = 0;
					break;
				}
			}
		}
	if (position.vX < 0.f) { // x-position fix into the game area
		position.vX = 0.f;
		velocity.vX = 0.f;
		ret = 1;
	} else if (position.vX > (mapWidth - 1.f) * heroSize) {
		position.vX = (mapWidth - 1.f) * heroSize;
		velocity.vX = 0.f;
		ret = 1;
	}
	if (position.vY < 0.f) { // y-position fix into the game area
		position.vY = 0.f;
		velocity.vY = 0.f;
		ret = 1;
	} else if (position.vY > (mapHeight - 1.f) * heroSize) {
		position.vY = (mapHeight - 1.f) * heroSize;
		velocity.vY = 0.f;
		jumpCount = 0;
		ret = 1;
	}
	return ret;
}
/*
int heroPositionFixX(pointVector &position, pointVector &velocity) {
	// Fix the hero's x-position with the map data.
	// Return value is reserved for additional info (game fail/other status, etc. to be implemented)
	gameStage &game = gameStages[currentStage];
	//position.vX = maxValue(0.f, minValue(position.vX, (mapWidth - 1) * 1.f * heroSize));
	if (position.vX < 0.f) {
		position.vX = 0.f;
		velocity.vX = 0.f;
	} else if (position.vX > (mapWidth - 1) * 1.f * heroSize) {
		position.vX = (mapWidth - 1) * 1.f * heroSize;
		velocity.vX = 0.f;
	}
	for (int j = (int)floor(position.vY / heroSize); j < mapHeight && j <= (int)ceil(position.vY / heroSize); ++j)
		for (int i = (int)floor(position.vX / heroSize); i < mapWidth && i <= (int)ceil(position.vX / heroSize); ++i) {
			switch (game.blocks[i][j]) {
				case blockEmpty: case blockStartingPoint: case blockCheckpoint: { continue; }
				case blockWall:
				{
					if (!isRectIntersect(i * heroSize * 1.f, j * heroSize * 1.f, (i + 1.f) * heroSize, (j + 1.f) * heroSize, position.vX, position.vY, position.vX + heroSize, position.vY + heroSize)) continue;
					if (velocity.vX >= 0.f) {
						if (i * heroSize * 1.f <= position.vX) continue;
						if (!isIntervalIntersect(j * heroSize * 1.f, (j + 1.f) * heroSize, position.vY + 1.f, position.vY + heroSize - 1.f)) continue;
						// fix to left
						float newX = minValue(position.vX, (i - 1.f) * heroSize), oldX = position.vX;
						position.vX = newX;
						if (dcmp(newX - oldX) != 0) {
							velocity.vX = 0.f;
							//	velocity.vY = 0.f;
							//	face = directionRight;
							return 1;
						}
					} else {
						if ((i + 1.f) * heroSize >= position.vX) continue;
						if (!isIntervalIntersect(j * heroSize * 1.f, (j + 1.f) * heroSize, position.vY + 1.f, position.vY + heroSize - 1.f)) continue;
						// fix to right
						float newX = maxValue(position.vX, (i + 1.f) * heroSize), oldX = position.vX;
						position.vX = newX;
						if (dcmp(newX - oldX) != 0) {
							velocity.vX = 0.f;
							//	velocity.vY = 0.f;
							//	face = directionLeft;
							return 1;
						}
					}
					break;
				}
			}
		}
	return 0;
}

int heroPositionFixY(pointVector &position, pointVector &velocity) {
	// Fix the hero's y-position with the map data.
	// Return value is reserved for additional info (game fail/other status, etc. to be implemented)
	gameStage &game = gameStages[currentStage];
	//position.vY = maxValue(0.f, minValue(position.vY, (mapHeight - 1) * 1.f * heroSize));
	if (position.vY < 0.f) {
		position.vY = 0.f;
		velocity.vY = 0.f;
	} else if (position.vY > (mapHeight - 1) * 1.f * heroSize) {
		position.vY = (mapHeight - 1) * 1.f * heroSize;
		velocity.vY = 0.f;
	}
	for (int j = (int)floor(position.vY / heroSize); j < mapHeight && j <= (int)ceil(position.vY / heroSize); ++j)
		for (int i = (int)floor(position.vX / heroSize); i < mapWidth && i <= (int)ceil(position.vX / heroSize); ++i) {
			switch (game.blocks[i][j]) {
				case blockEmpty: case blockStartingPoint: case blockCheckpoint: { continue; }
				case blockWall:
				{
					if (!isRectIntersect(i * heroSize * 1.f, j * heroSize * 1.f, (i + 1.f) * heroSize, (j + 1.f) * heroSize, position.vX, position.vY, position.vX + heroSize, position.vY + heroSize)) continue;
					if (velocity.vY >= 0.f) {
						if (j * heroSize * 1.f <= position.vY) continue;
						if (!isIntervalIntersect(i * heroSize * 1.f, (i + 1.f) * heroSize, position.vX + 1.f, position.vX + heroSize - 1.f)) continue;
						// fix to top
						float newY = minValue(position.vY, (j - 1.f) * heroSize), oldY = position.vY;
						position.vY = newY;
						if (dcmp(newY - oldY) != 0) {
							velocity.vY = 0.f;
							//	velocity.vX = dcmp(velocity.vX) * 2 * epsilon;
							//	face = dcmp(velocity.vX) > 0 ? directionRight : directionLeft;
							return 1;
						}
					} else {
						if ((j + 1.f) * heroSize >= position.vY) continue;
						if (!isIntervalIntersect(i * heroSize * 1.f, (i + 1.f) * heroSize, position.vX + 1.f, position.vX + heroSize - 1.f)) continue;
						// fix to right
						float newY = maxValue(position.vY, (j + 1.f) * heroSize), oldY = position.vY;
						position.vY = newY;
						if (dcmp(newY - oldY) != 0) {
							velocity.vY = 0.f;
							//	velocity.vX = dcmp(velocity.vX) * 2 * epsilon;
							//	face = dcmp(velocity.vX) > 0 ? directionRight : directionLeft;
							return 1;
						}
					}
					break;
				}
			}
		}
	return 0;
}
*/
int getHeroState(pointVector velocity, int idx) {
	if (dcmp(velocity.vY) != 0) return (face == directionLeft ? heroLeft : heroRight) + heroJumping + (idx % 2);
	int ans = (face == directionLeft ? heroLeft : heroRight);
	if (!isKeyDown[VK_LEFT] && !isKeyDown[VK_RIGHT]) ans += heroStanding;
	else if ((isKeyDown[VK_LEFT] && velocity.vX < 0.f) || (isKeyDown[VK_RIGHT] && velocity.vX >= 0.f))
		ans += heroWalking;
	ans += (idx % 4);
	//	debugPrintF("%d\n", idx % 4);
	return ans;
}

void renderGame() {
	static int heroCounter = 0;
	gameStage &game = gameStages[currentStage];
	renderTarget->DrawBitmap(game.bkgImage, makeRectF(0.f, 0.f, windowClientWidth * 1.f, windowClientHeight * 1.f));
	for (int i = 0; i < mapWidth; ++i) for (int j = 0; j < mapHeight; ++j) {
		switch (game.blocks[i][j]) {
			case blockWall:
			{
				renderTarget->DrawBitmap(wallImage, makeRectF(1.f * i * heroSize, 1.f * j * heroSize, (i + 1.f) * heroSize, (j + 1.f) * heroSize));
				break;
			}
		}
		int heroStyle = getHeroState(heroVelocity, heroCounter++);
		renderTarget->DrawBitmap(heroImage, makeRectF(heroPosition.vX, heroPosition.vY, heroPosition.vX + heroSize, heroPosition.vY + heroSize), 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, makeRectF(1.f * heroStyle * heroSize, 0.f, (heroStyle + 1.f) * heroSize, 1.f * heroSize));
	}

	WCHAR tmp[bufferSize];
	int len = swprintf_s(tmp, L"Hero position: (%3.3f, %3.3f), velocity: (%3.3f, %3.3f)", heroPosition.vX, heroPosition.vY, heroVelocity.vX, heroVelocity.vY);
	renderTarget->DrawTextA(tmp, len, textFormatNormal, makeRectF(0.f, 0.f, windowClientWidth * 1.f, 40.f), brushBlack);


	if (face == directionRight) {
		if (isKeyDown[VK_RIGHT]) {
			renderTarget->DrawTextA(L"Moving right, keyRight: pressed", 31, textFormatNormal, makeRectF(0.f, windowClientHeight - 40.f, windowClientWidth * 1.f, 40.f), brushBlack);
		} else {
			renderTarget->DrawTextA(L"Moving right, keyRight: released", 32, textFormatNormal, makeRectF(0.f, windowClientHeight - 40.f, windowClientWidth * 1.f, 40.f), brushBlack);
		}
	} else {
		if (isKeyDown[VK_LEFT]) {
			renderTarget->DrawTextA(L"Moving left, keyLeft: pressed", 30, textFormatNormal, makeRectF(0.f, windowClientHeight - 40.f, windowClientWidth * 1.f, 40.f), brushBlack);
		} else {
			renderTarget->DrawTextA(L"Moving left, keyRight: released", 31, textFormatNormal, makeRectF(0.f, windowClientHeight - 40.f, windowClientWidth * 1.f, 40.f), brushBlack);
		}
	}
}

XplorerResult gameStage::loadFromFile(const WCHAR *fileName) {
	WCHAR backgroundImage[bufferSize];
	FILE *file = NULL;
	_wfopen_s(&file, fileName, L"r");
	if (file == NULL) {
		n = mapHeight, m = mapWidth;
		return XplorerResult::fileNotFound;
	}
	if (fscanf_s(file, "%d%d", &n, &m) != 2) {
		fclose(file);
		return XplorerResult::fileBroken;
	}
	if (n == mapHeight && m == mapWidth) {
		for (int i = 0; i < n; ++i) for (int j = 0; j < m; ++j) {
			if (fscanf_s(file, "%d", blocks[j] + i) != 1) {
				fclose(file);
				return XplorerResult::fileBroken;
			}
			if (blocks[j][i] == blockStartingPoint) {
				initialPosition.vX = 1.f * j * heroSize;
				initialPosition.vY = 1.f * i * heroSize;
			}
		}
		fwscanf_s(file, L"%ls", backgroundImage, bufferSize);
		fclose(file);
		safeRelease(bkgImage);
		loadBitmapFromFile(renderTarget, imageFactory, backgroundImage, windowClientWidth, windowClientHeight, &bkgImage);
	} else {
		fclose(file);
		return XplorerResult::fileBroken;
	}
	return XplorerResult::okay;
}

gameStage::~gameStage() {
	safeRelease(bkgImage);
}

void newStage(int stageID) {
	currentStage = stageID;
	heroVelocity = pointVector(0.f, 0.f);
	heroPosition = gameStages[stageID].initialPosition;
	face = directionRight;
	jumpCount = 0;
}

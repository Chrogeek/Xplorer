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
#include <windows.h>
#include <stdio.h>
#include <algorithm>
#include "defs.h"
#include "game.h"
#include "utility.h"

extern bool isKeyDown[128];
extern int currentStage;
extern ID2D1Bitmap *wallImage, *heroImage;
extern gameStage gameStages[maxStage + 1];

//extern int heroDirection;
//extern XplorerDirection face;

extern gameHero hero;

extern ID2D1DCRenderTarget *renderTarget;
extern IWICImagingFactory *imageFactory;
extern IDWriteFactory *writeFactory;
extern IDWriteTextFormat *textFormatNormal;
extern ID2D1SolidColorBrush *brushBlack;

//extern pointVector heroPosition, heroVelocity;
//extern int jumpCount;

D2D1_RECT_F getHeroRect(gameHero hero) {
	D2D1_RECT_F heroRect;
	heroRect.left = hero.position.vX + heroSideMargin, heroRect.right = hero.position.vX + heroSize - heroSideMargin;
	heroRect.top = hero.position.vY + heroTopMargin, heroRect.bottom = hero.position.vY + heroSize - heroBottomMargin;
	return heroRect;
}

bool isOutOfMap(float x, float y) {
	return x < 0.f || x >= mapWidth * heroSize * 1.f || y < 0.f || y >= mapHeight * heroSize * 1.f;
}

bool isOutOfMap(D2D1_RECT_F rect) {
	return isOutOfMap(rect.left, rect.top) || isOutOfMap(rect.left, rect.bottom) ||
		isOutOfMap(rect.right, rect.top) || isOutOfMap(rect.right, rect.bottom);
}

bool isOutOfMap(pointVector point) {
	return isOutOfMap(point.vX, point.vY);
}

bool isOutOfMap() {
	return isOutOfMap(makeRectF(hero.position.vX + heroSideMargin, hero.position.vY + heroTopMargin, hero.position.vX + heroSize - heroSideMargin, hero.position.vY + heroSize - heroBottomMargin));
}

bool isWall(float x, float y) {
	gameStage &game = gameStages[currentStage];
	return game.blocks[(int)(x / heroSize)][(int)(y / heroSize)] == blockWall;
}

bool isWall(pointVector point) {
	return isWall(point.vX, point.vY);
}

int xWall(pointVector position) {
	float left = position.vX + heroSideMargin, right = position.vX + heroSize - heroSideMargin;
	float top = position.vY + heroTopMargin, bottom = position.vY + heroSize - heroBottomMargin;
	if (isWall(left - maxDelta, top) || isWall(left - maxDelta, bottom)) return -1;
	if (isWall(right + maxDelta, top) || isWall(right + maxDelta, bottom)) return 1;
	return 0;
}

int yWall(pointVector position) {
	float left = position.vX + heroSideMargin, right = position.vX + heroSize - heroSideMargin;
	float top = position.vY + heroTopMargin, bottom = position.vY + heroSize - heroBottomMargin;
	if (isWall(left, top - maxDelta) || isWall(left, bottom - maxDelta)) return -1;
	if (isWall(right, top + maxDelta) || isWall(right, bottom + maxDelta)) return 1;
	return 0;
}

void updateHero() {
	if (currentStage < stageTutorial) return;
	static UINT lastTime = timeGetTime();
	UINT thisTime = timeGetTime();
	if (thisTime == lastTime) return;
	float interval = (thisTime - lastTime) / 1000.0f;
	lastTime = thisTime;
//	pointVector newVelocity = heroVelocity + gravityAcceleration * interval;
	//pointVector acceleration;
	if (hero.face == directionRight) {
		if (isKeyDown[VK_RIGHT]) {
			//		newVelocity += interval * moveAcceleration;
			//newVelocity.vX = maxVelocity.vX;
			//hero.move(pointVector(maxVelocity.vX, ), interval);
			//acceleration.vX = maxVelocity.vX;
			if (!isWall(hero.position.vX + heroSize - heroSideMargin + maxDelta, hero.position.vY)) {
			//	hero.velocity.vX = maxVelocity.vX;
				hero.lockX = false;
			//	hero.move(pointVector(0.f, 0.f), interval);
				hero.move(moveAcceleration, interval);
			}
		} else {
			//newVelocity.vX = 0.f;
			hero.velocity.vX = 0.f;
			//		newVelocity -= interval * moveAcceleration;
			//		newVelocity.vX = maxValue(0.f, newVelocity.vX);
		}
	} else {
		if (isKeyDown[VK_LEFT]) {
			//		newVelocity -= interval * moveAcceleration;
		//	newVelocity.vX = minVelocity.vX;
			if (!isWall(hero.position.vX + heroSideMargin - maxDelta, hero.position.vY)) {
				//hero.velocity.vX = minVelocity.vX;
				hero.lockX = false;
				//hero.move(pointVector(0.f, 0.f), interval);
				hero.move(-moveAcceleration, interval);
			}
		} else {
			//newVelocity.vX = 0.f;
			hero.velocity.vX = 0.f;
			//		newVelocity += interval * moveAcceleration;
			//		newVelocity.vX = minValue(0.f, newVelocity.vX);
		}
	}
	if (!yWall(hero.position + pointVector(0.f, maxDelta)) && !isOutOfMap(hero.position + pointVector(0.f, heroSize - heroBottomMargin + maxDelta))) hero.lockY = false;
	if (!hero.lockY) hero.move(gravityAcceleration, interval);
//	if (xWall(heroPosition) * dcmp(newVelocity.vX) == 1) heroVelocity.vX = newVelocity.vX = 0;
//	if (yWall(heroPosition) * dcmp(newVelocity.vY) == 1) heroVelocity.vY = newVelocity.vY = 0;
//	limitVelocity(newVelocity);
//	pointVector newPosition = heroPosition + (newVelocity + heroVelocity) / 2.f * interval;
//	heroPositionAdjust(newPosition, newVelocity);
//	heroVelocity = newVelocity;
//	heroPosition = newPosition;
	heroPositionAdjust(hero);
//	if (hero.lockX) printf("X locked\n");
//	if (hero.lockY) printf("Y locked\n");
}

int heroPositionAdjust(gameHero &hero) {
	// Fix the hero's position with the map data. (Collision detection & adjustment)
	// Return value shows additional info (reserved for more usage).
	gameStage &game = gameStages[currentStage];
	int ret = 0;
//	for (int j = (int)floor(hero.position.vY / heroSize); j < mapHeight && j <= (int)ceil(hero.position.vY / heroSize); ++j) {
	//	for (int i = (int)floor(hero.position.vX / heroSize); i < mapWidth && i <= (int)ceil(hero.position.vX / heroSize); ++i) {
	for (int j = 0; j < mapHeight; ++j) {
		for (int i = 0; i < mapWidth; ++i) {
			// We only need to check the squares the hero involves (no more than 4)
			switch (game.blocks[i][j]) {
				case blockStartingPoint: case blockCheckpoint: case blockEmpty:
				{
					continue;
				}
			}
			D2D1_RECT_F wallRect, heroRect;
			wallRect.left = (float)i * heroSize, wallRect.right = (i + 1.f) * heroSize;
			wallRect.top = (float)j * heroSize, wallRect.bottom = (j + 1.f) * heroSize;
			heroRect.left = hero.position.vX + heroSideMargin, heroRect.right = hero.position.vX + heroSize - heroSideMargin;
			heroRect.top = hero.position.vY + heroTopMargin, heroRect.bottom = hero.position.vY + heroSize - heroBottomMargin;
			//if (!isRectIntersect((int)position.vX + heroSideMargin, (int)position.vY + heroTopMargin, (int)position.vX + heroSize - heroSideMargin, (int)position.vY + heroSize - heroBottomMargin, i * heroSize, j * heroSize, (i + 1) * heroSize, (j + 1) * heroSize)) continue;
			/*if (!isRectIntersect(heroRect.left, heroRect.top, heroRect.right, heroRect.bottom, wallRect.left, wallRect.top, wallRect.right, wallRect.bottom)) continue;*/
			if (!isRectIntersect(heroRect, wallRect)) continue;

		//	printf("Crashed into a wall ");
			
		//	pointVector squareCenter(i * heroSize + heroSize / 2, j * heroSize + (heroSize + heroTopMargin - heroBottomMargin) / 2);
			//pointVector delta = hero.position - pointVector((float)i * heroSize, (float)j * heroSize);
			pointVector delta = rectCenter(heroRect) - rectCenter(wallRect);
			// Adjustment is based on integer coordinates, for stronger robustness and better experience.
			if (dcmp(abs(delta.vX), abs(delta.vY)) >= 0) {
				// Crashed in x-axis. Fix in x-coordinate.
			/*	if (isInInterval((int)position.vX + heroSideMargin, i * heroSize + heroSize / 2, (i + 1) * heroSize))
					position.vX = (i + 1.f) * heroSize - heroSideMargin + 1, velocity.vX = 0.f, ret = 1;
				if (isInInterval((int)ceil(position.vX) + heroSize - heroSideMargin, i * heroSize, i * heroSize + heroSize / 2))
					position.vX = (i - 1.f) * heroSize + heroSideMargin - 1, velocity.vX = 0.f, ret = 1;
				//	break;*/
				ret |= heroFixLeft(hero, wallRect);
				ret |= heroFixRight(hero, wallRect);
			//	printf("Crashed X ");
			}
			if (dcmp(abs(delta.vX), abs(delta.vY)) <= 0) {
				// Crashed in y-axis. Fix in y-coordinate.
			/*	if (isInInterval((int)position.vY + heroTopMargin, j * heroSize + heroSize / 2, (j + 1) * heroSize))
					position.vY = (j + 1.f) * heroSize - heroTopMargin + 1, velocity.vY = 0.f, ret = 1;
				if (isInInterval((int)ceil(position.vY) + heroSize - heroBottomMargin, j * heroSize, j * heroSize + heroSize / 2))
					position.vY = (j - 1.f) * heroSize + heroBottomMargin - 1, velocity.vY = 0.f, ret = 1, hero.jumpCount = 0;
				//	break;*/
				ret |= heroFixTop(hero, wallRect);
				ret |= heroFixBottom(hero, wallRect);
		//		printf("Crashed Y ");
			}
			//if (ret) break;
		/*	if (isIntervalIntersect(heroRect.left, heroRect.right, wallRect.left, wallRect.right)) {
				ret |= heroFixTop(hero, wallRect);
				ret |= heroFixBottom(hero, wallRect);
				printf("Crashed Y ");
			}
			heroRect = getHeroRect(hero);
			if (isIntervalIntersect(heroRect.top, heroRect.bottom, wallRect.top, wallRect.bottom)) {
				ret |= heroFixLeft(hero, wallRect);
				ret |= heroFixRight(hero, wallRect);
				printf("Crashed X ");
			}*/
		}
		//if (ret) break;
	}
	if (hero.position.vX < -heroSideMargin * 1.f) { // x-position fix into the game area
		hero.position.vX = -heroSideMargin;
		hero.velocity.vX = 0.f;
		hero.lockX = true;
		ret = 1;
	} else if (hero.position.vX > (mapWidth - 1.f) * heroSize + heroSideMargin) {
		hero.position.vX = (mapWidth - 1.f) * heroSize + heroSideMargin;
		hero.velocity.vX = 0.f;
		hero.lockX = true;
		ret = 1;
	}
	if (hero.position.vY < -heroTopMargin * 1.f) { // y-position fix into the game area
		hero.position.vY = -heroTopMargin;
		hero.velocity.vY = 0.f;
		hero.lockY = true;
		ret = 1;
	} else if (hero.position.vY > (mapHeight - 1.f) * heroSize + heroBottomMargin) {
		hero.position.vY = (mapHeight - 1.f) * heroSize + heroBottomMargin;
		hero.velocity.vY = 0.f;
		hero.lockY = true;
		hero.jumpCount = 0;
		ret = 1;
	}
	//printf("Position-Y = %0.3lf, Velocity-Y = %0.3lf\n", hero.position.vY, hero.velocity.vY);
	printf("pos: (%0.2lf, %0.2lf), vel: (%0.2lf, %0.2lf) X: %s, Y: %s\n", hero.position.vX, hero.position.vY, hero.velocity.vX, hero.velocity.vY, hero.lockX ? "locked" : "free", hero.lockY ? "locked" : "free");
	//if (!ret) hero.lockX = hero.lockY = false;
	return ret;
}

int heroFixTop(gameHero &hero, D2D1_RECT_F object) {
	// Fix the hero's position so that it stands on an object
	D2D1_RECT_F heroRect;
	heroRect.top = hero.position.vY + heroTopMargin;
	heroRect.bottom = hero.position.vY + heroSize - heroBottomMargin;
	//if (heroRect.bottom >= object.top && heroRect.top + heroSize / 2) {
	if (isInInterval(heroRect.bottom, object.top, object.top + heroSize / 2.f)) {
		hero.position.vY = object.top + heroBottomMargin - heroSize - minDelta;
		hero.lockY = true;
		hero.velocity.vY = 0.f;
		hero.jumpCount = 0;
		return 1;
	}
	return 0;
}

int heroFixBottom(gameHero &hero, D2D1_RECT_F object) {
	// Fix the hero's position so that it is exactly below the object
	D2D1_RECT_F heroRect;
	heroRect.top = hero.position.vY + heroTopMargin;
	heroRect.bottom = hero.position.vY + heroSize - heroBottomMargin;
	//if (heroRect.top <= object.bottom - heroSize / 2) {
	if (isInInterval(heroRect.top, object.bottom - heroSize / 2.f, object.bottom)) {
		hero.position.vY = object.bottom - heroTopMargin + minDelta;
		hero.lockY = true;
		hero.velocity.vY = 0.f;
		return 1;
	}
	return 0;
}

int heroFixLeft(gameHero &hero, D2D1_RECT_F object) {
	// Fix the hero's position so that it is exactly on the left of the object
	D2D1_RECT_F heroRect;
	heroRect.left = hero.position.vX + heroSideMargin;
	heroRect.right = hero.position.vX + heroSize - heroSideMargin;
	//if (heroRect.right >= object.left + heroSize / 2) {
	if (isInInterval(heroRect.right, object.left, object.left + heroSize / 2.f)) {
		hero.position.vX = object.left + heroSideMargin - heroSize - minDelta;
		hero.lockX = true;
		hero.velocity.vX = 0.f;
		return 1;
	}
	return 0;
}

int heroFixRight(gameHero &hero, D2D1_RECT_F object) {
	// Fix the hero's position so that it is exactly below the object
	D2D1_RECT_F heroRect;
	heroRect.left = hero.position.vX + heroSideMargin;
	heroRect.right = hero.position.vX + heroSize - heroSideMargin;
	//if (heroRect.left <= object.right - heroSize / 2) {
	if (isInInterval(heroRect.left, object.right - heroSize / 2.f, object.right)) {
		hero.position.vX = object.right - heroSideMargin + minDelta;
		hero.lockX = true;
		hero.velocity.vX = 0.f;
		return 1;
	}
	return 0;
}

int getHeroState(pointVector velocity) {
	int index = timeGetTime() % 500 / 125;
	switch (dcmp(velocity.vY)) {
		case 1:
		{
			return (hero.face == directionLeft ? heroLeft : heroRight) + heroFalling + (index % 2);
			break;
		}
		case -1:
		{
			return (hero.face == directionLeft ? heroLeft : heroRight) + heroJumping + (index % 2);
			break;
		}
		case 0:
		{
			int ans = (hero.face == directionLeft ? heroLeft : heroRight);
			if (!isKeyDown[VK_LEFT] && !isKeyDown[VK_RIGHT]) ans += heroStanding;
			else if ((isKeyDown[VK_LEFT] && velocity.vX < 0.f) || (isKeyDown[VK_RIGHT] && velocity.vX >= 0.f))
				ans += heroWalking;
			ans += index;
			//	debugPrintF("%d\n", idx % 4);
			return ans;
		}
		default:
		{
			return 0;
		}
	}
}

void renderGame() {
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
		int heroStyle = getHeroState(hero.velocity);
		renderTarget->DrawBitmap(heroImage, makeRectF(hero.position.vX, hero.position.vY, hero.position.vX + heroSize, hero.position.vY + heroSize), 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, makeRectF(1.f * heroStyle * heroSize, 0.f, (heroStyle + 1.f) * heroSize, 1.f * heroSize));

		renderTarget->DrawRectangle(makeRectF(hero.position.vX + heroSideMargin, hero.position.vY + heroTopMargin, hero.position.vX + heroSize - heroSideMargin, hero.position.vY + heroSize - heroBottomMargin), brushBlack);
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
	hero.velocity = pointVector(0.f, 0.f);
	hero.position = gameStages[stageID].initialPosition;
	hero.face = directionRight;
	hero.jumpCount = 0;
	hero.lockX = hero.lockY = false;
}

void gameHero::move(pointVector dV, float dT) {
	pointVector newV = velocity + dV * dT;
	limitVelocity(newV);
	if (lockX) velocity.vX = 0.f;
	else {
		float vX = (velocity.vX + newV.vX) / 2.f;
		position.vX += vX * dT;
		if (dcmp(velocity.vX * newV.vX) == -1) face = (face == directionLeft ? directionRight : directionLeft);
		velocity.vX = newV.vX;
	}
	if (lockY) velocity.vY = 0.f;
	else {
		float vY = (velocity.vY + newV.vY) / 2.f;
		position.vY += vY * dT;
		velocity.vY = newV.vY;
	}
}

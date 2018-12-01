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

#include "defs.h"
#include <cstdio>
#include <fstream>
#include <algorithm>
#include <d2d1.h>
#include <dwrite.h>
#include <windows.h>
#include "json.h"
#include "game.h"
#include "utility.h"
#include "gameLevel.h"

extern bool isKeyDown[128];
//extern int currentStage;
extern UINT lastJumpTime;
extern int currentChapter, currentLevel;

extern ID2D1Bitmap *wallImage, *heroImage;
//extern gameStage gameStages[maxStage + 1];
extern gameManager gameMaster;

extern gameHero hero;

extern ID2D1DCRenderTarget *mainRenderer;
extern IWICImagingFactory *imageFactory;
extern IDWriteFactory *writeFactory;
extern IDWriteTextFormat *textFormatNormal;
extern ID2D1SolidColorBrush *brushBlack;

using json = nlohmann::json;

bool isOutOfMap(double x, double y) { // returns whether the point specified is in the game scene
	gameLevel &game = gameMaster.chapters[currentChapter].levels[currentLevel];
	return x < 0.0 || x >= (double)game.columns * unitSize || y < 0.0 || y >= (double)game.rows * unitSize;
}

bool isOutOfMap(rectReal rect) { // returns whether the rectangle is not completely contained in the game scene or not
	return isOutOfMap(rect.left, rect.top) || isOutOfMap(rect.left, rect.bottom) ||
		isOutOfMap(rect.right, rect.top) || isOutOfMap(rect.right, rect.bottom);
}

bool isOutOfMap(pointVector point) { // overloading of isOutOfMap(x, y)
	return isOutOfMap(point.vX, point.vY);
}

bool isWall(double x, double y) { // returns whether the point specified is a wall block
//	gameStage &game = gameStages[currentStage];
	gameLevel &game = gameMaster.chapters[currentChapter].levels[currentLevel];
	int boardX = (int)floor(x / unitSize), boardY = (int)floor(y / unitSize);
	if (boardX < 0 || boardX >= game.columns) return false;
	if (boardY < 0 || boardY >= game.rows) return false;
	return game.grid[boardX][boardY] == blockWall;
}

bool isWall(pointVector point) { // overloading of isWall(x, y)
	return isWall(point.vX, point.vY);
}

int xWall(gameHero hero) { // decides if the hero hits a block on the left/right
	rectReal heroRect = hero.rect();
	if (isWall(heroRect.left - maxDelta, heroRect.top) || isWall(heroRect.left - maxDelta, heroRect.bottom)) return -1;
	if (isWall(heroRect.right + maxDelta, heroRect.top) || isWall(heroRect.right + maxDelta, heroRect.bottom)) return 1;
	return 0;
}

int yWall(gameHero hero) { // decides if the hero hits a block on the top/bottom
	rectReal heroRect = hero.rect();
	if (isWall(heroRect.left, heroRect.top - maxDelta) || isWall(heroRect.left, heroRect.bottom - maxDelta)) return -1;
	if (isWall(heroRect.right, heroRect.top + maxDelta) || isWall(heroRect.right, heroRect.bottom + maxDelta)) return 1;
	return 0;
}

void updateHero() { // update hero data (position, velocity, ...)
//	if (currentStage < stageTutorial) return;
	if (currentChapter < stageTutorial) return;
	static UINT lastTime = timeGetTime();
	UINT thisTime = timeGetTime();
	if (thisTime == lastTime) return;
	double interval = (thisTime - lastTime) / 1000.0;
	lastTime = thisTime;
	
	pointVector deltaVelocity(0.0, 0.0);

	if (hero.face == directionRight) {
		if (isKeyDown[VK_RIGHT]) {
			if (!isWall(hero.right() + maxDelta, hero.top()) && !isWall(hero.right() + maxDelta, hero.bottom())) {
				hero.lockX = false;
				deltaVelocity += moveAcceleration;
			}
		} else {
			deltaVelocity -= moveAcceleration;
		}
	} else {
		if (isKeyDown[VK_LEFT]) {
			if (!isWall(hero.left() - maxDelta, hero.top()) && !isWall(hero.left() - maxDelta, hero.bottom())) {
				hero.lockX = false;
				deltaVelocity -= moveAcceleration;
			}
		} else {
			deltaVelocity += moveAcceleration;
		}
	}
	if (isKeyDown[jumpKey] && thisTime - lastJumpTime < maxJumpTime) {
		// The jump key is pressed down and it is still jumping
		hero.lockY = false;
		deltaVelocity += jumpAcceleration;
	}
	if ((!isWall(hero.left(), hero.bottom() + maxDelta) && !isWall(hero.right(), hero.bottom() + maxDelta)) && !isOutOfMap(hero.left(), hero.bottom() + maxDelta)) hero.lockY = false; // if the hero is not standing on a block, and the hero is not on the bottom of the scene, then release it in direction y
	deltaVelocity += gravityAcceleration;
	hero.move(deltaVelocity, interval);
	heroPositionAdjust(hero);
}

int heroPositionAdjust(gameHero &hero) {
	// Fix the hero's position with the map data. (Collision detection & adjustment)
	// Return value shows additional info (reserved for more usage).
//	gameStage &game = gameStages[currentStage];
	gameLevel &game = gameMaster.chapters[currentChapter].levels[currentLevel];
	int ret = 0;
	for (int j = std::max(0, (int)floor(hero.top() / unitSize)); j <= std::min(game.rows - 1, (int)ceil(hero.bottom() / unitSize)); ++j) {
		for (int i = std::max(0, (int)floor(hero.left() / unitSize)); i <= std::min(game.columns - 1, (int)ceil(hero.right() / unitSize)); ++i) {
			// We only need to check the squares the hero touches
			switch (game.grid[i][j]) {
				case blockStartingPoint: case blockCheckpoint: case blockEmpty:
				{
					continue;
				}
			}
			rectReal wallRect, heroRect;
			wallRect.left = (double)i * unitSize, wallRect.right = (i + 1.0) * unitSize;
			wallRect.top = (double)j * unitSize, wallRect.bottom = (j + 1.0) * unitSize;
			heroRect = hero.rect();
			if (!isRectIntersect(heroRect, wallRect)) continue;
			pointVector delta = rectCenter(heroRect) - rectCenter(wallRect);
			if (dcmp(abs(delta.vX), abs(delta.vY)) >= 0) {
				// Crashed in x-axis. Fix in x-coordinate.
				ret |= heroFixLeft(hero, wallRect);
				ret |= heroFixRight(hero, wallRect);
			}
			if (dcmp(abs(delta.vX), abs(delta.vY)) <= 0) {
				// Crashed in y-axis. Fix in y-coordinate.
				ret |= heroFixTop(hero, wallRect);
				ret |= heroFixBottom(hero, wallRect);
			}
		}
	}
	if (hero.position.vX < -heroSideMargin * 1.0) { // x-position fix into the game area
		hero.position.vX = -heroSideMargin;
		hero.velocity.vX = 0.0;
		hero.lockX = true;
		ret = 1;
	} else if (hero.position.vX > game.columns * unitSize - heroSize + heroSideMargin) {
		hero.position.vX = game.columns * unitSize - heroSize + heroSideMargin;
		hero.velocity.vX = 0.0;
		hero.lockX = true;
		ret = 1;
	}
	if (hero.position.vY < -heroTopMargin * 1.0) { // y-position fix into the game area
		hero.position.vY = -heroTopMargin;
		hero.velocity.vY = 0.0;
		hero.lockY = true;
		ret = 1;
	} else if (hero.position.vY > game.rows * unitSize - heroSize + heroBottomMargin) {
		hero.position.vY = game.rows * unitSize - heroSize + heroBottomMargin;
		hero.velocity.vY = 0.0;
		hero.lockY = true;
		hero.jumpCount = 0;
		ret = 1;
	}
	return ret;
}

int heroFixTop(gameHero &hero, rectReal object) {
	// Fix the hero's position so that it stands on an object
	if (isInInterval(hero.bottom(), object.top, object.top + unitSize / 2.0)) {
		hero.position.vY = object.top + heroBottomMargin - heroSize - minDelta;
		hero.lockY = true;
		hero.velocity.vY = 0.0;
		hero.jumpCount = 0;
		return 1;
	}
	return 0;
}

int heroFixBottom(gameHero &hero, rectReal object) {
	// Fix the hero's position so that it is exactly below the object
	if (isInInterval(hero.top(), object.bottom - unitSize / 2.0, object.bottom)) {
		hero.position.vY = object.bottom - heroTopMargin + minDelta;
		hero.lockY = true;
		hero.velocity.vY = 0.0;
		return 1;
	}
	return 0;
}

int heroFixLeft(gameHero &hero, rectReal object) {
	// Fix the hero's position so that it is exactly on the left of the object
	if (isInInterval(hero.right(), object.left, object.left + unitSize / 2.0)) {
		hero.position.vX = object.left + heroSideMargin - heroSize - minDelta;
		hero.lockX = true;
		hero.velocity.vX = 0.0;
		return 1;
	}
	return 0;
}

int heroFixRight(gameHero &hero, rectReal object) {
	// Fix the hero's position so that it is exactly below the object
	if (isInInterval(hero.left(), object.right - unitSize / 2.0, object.right)) {
		hero.position.vX = object.right - heroSideMargin + minDelta;
		hero.lockX = true;
		hero.velocity.vX = 0.0;
		return 1;
	}
	return 0;
}

int getHeroState(pointVector velocity) {
	// Calculate which picture to draw
	int index = timeGetTime() % 500 / 125; // Each frame lasts 125 ms, and each set contains 4 pictures
	switch (dcmp(velocity.vY)) {
		case 1: // v_y>0: falling
		{
			return (hero.face == directionLeft ? heroLeft : heroRight) + heroFalling + (index % 2); // we only have 2 graphs for falling
			break;
		}
		case -1: // v_y<0: jumping
		{
			return (hero.face == directionLeft ? heroLeft : heroRight) + heroJumping + (index % 2); // we only have 2 graphs for jumping
			break;
		}
		case 0: // v_y=0: walking or standing/slipping (these two states share the same set of pictures)
		{
			int ans = (hero.face == directionLeft ? heroLeft : heroRight); // the direction
			if (!isKeyDown[VK_LEFT] && !isKeyDown[VK_RIGHT]) ans += heroStanding; // standing/slipping
			else if ((isKeyDown[VK_LEFT] && velocity.vX < 0.0) || (isKeyDown[VK_RIGHT] && velocity.vX >= 0.0))
				ans += heroWalking; // walking
			ans += index;
			return ans;
		}
		default:
		{
			return 0;
		}
	}
}

void renderGame() {
//	gameStage &game = gameStages[currentStage];
	gameLevel &game = gameMaster.chapters[currentChapter].levels[currentLevel];

	float renderLeft, renderTop;
	renderLeft = std::max(0.f, std::min((float)hero.position.vX - windowClientWidth / 2.f, (float)game.columns * unitSize - windowClientWidth));
	renderTop = std::max(0.f, std::min((float)hero.position.vY - windowClientHeight / 2.f, (float)game.rows * unitSize - windowClientHeight));

	//mainRenderer->DrawBitmap(game.bitmap, makeRectF(0.0, 0.0, windowClientWidth * 1.0, windowClientHeight * 1.0));
	mainRenderer->DrawBitmap(game.bitmap, makeRectF(0.f, 0.f, (float)windowClientWidth, (float)windowClientHeight), 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, makeRectF(renderLeft, renderTop, renderLeft + windowClientWidth, renderTop + windowClientHeight));
	/*for (int i = 0; i < mapWidth; ++i) for (int j = 0; j < mapHeight; ++j) {
		switch (game.grid[i][j]) {
			case blockWall:
			{
				mainRenderer->DrawBitmap(wallImage, makeRectF((float)i * unitSize, (float)j * unitSize, float(i + 1) * unitSize, float(j + 1) * unitSize));
				break;
			}
		}
	}*/
	int heroStyle = getHeroState(hero.velocity);
	mainRenderer->DrawBitmap(heroImage, makeRectF((float)hero.position.vX - renderLeft, (float)hero.position.vY - renderTop, (float)(hero.position.vX + heroSize - renderLeft), (float)(hero.position.vY + heroSize - renderTop)), 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, makeRectF(float(heroStyle * heroSize), 0.f, float((heroStyle + 1) * heroSize), (float)heroSize));

	mainRenderer->DrawRectangle(makeRectF((float)hero.left() - renderLeft, (float)hero.top() - renderTop, (float)hero.right() - renderLeft, (float)hero.bottom() - renderTop), brushBlack);
}
/*
XplorerResult gameStage::loadFromFile(const WCHAR *fileName) {
	WCHAR backgroundImage[bufferSize];
	FILE *file = nullptr;
	_wfopen_s(&file, fileName, L"r");
	if (file == nullptr) {
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
				initialPosition.vX = (double)j * unitSize;
				initialPosition.vY = (double)i * unitSize;
			}
		}
		fwscanf_s(file, L"%ls", backgroundImage, bufferSize);
		fclose(file);
		safeRelease(bkgImage);
		loadBitmapFromFile(mainRenderer, imageFactory, backgroundImage, windowClientWidth, windowClientHeight, &bkgImage);
	} else {
		fclose(file);
		return XplorerResult::fileBroken;
	}
	return XplorerResult::okay;
}

XplorerResult gameStage::loadFromJSON(const WCHAR *fileName) {
	std::ifstream fin(fileName);
	fin >> gameData;
	fin.close();
	return XplorerResult::okay;
}

gameStage::~gameStage() {
	safeRelease(bkgImage);
}

void newStage(int stageID) {
	currentStage = stageID;
	hero.velocity = pointVector(0.0, 0.0);
	hero.position = gameStages[stageID].initialPosition;
	hero.face = directionRight;
	hero.jumpCount = 0;
	hero.lockX = hero.lockY = false;
}*/

void startLevel(int chapterID, int levelID) {
	currentChapter = currentLevel = -1;
	if (chapterID >= (int)gameMaster.chapters.size() || levelID >= (int)gameMaster.chapters[chapterID].levels.size()) return;
	currentChapter = chapterID, currentLevel = levelID;
	hero.velocity = pointVector(0.0, 0.0);
	hero.position = gameMaster.chapters[currentChapter].levels[currentLevel].initialPosition;
	hero.face = directionRight;
	hero.jumpCount = 0;
	hero.lockX = hero.lockY = false;
}

void gameHero::move(pointVector dV, double dT) {
	pointVector newV = velocity + dV * dT;
	limitVelocity(newV);
	if (face == directionLeft && !isKeyDown[VK_LEFT]) newV.vX = std::min(newV.vX, 0.0);
	if (face == directionRight && !isKeyDown[VK_RIGHT]) newV.vX = std::max(newV.vX, 0.0);
	if (lockX) velocity.vX = 0.0;
	else {
		double vX = (velocity.vX + newV.vX) / 2.0;
		position.vX += vX * dT;
		velocity.vX = newV.vX;
	}
	if (lockY) velocity.vY = 0.0;
	else {
		double vY = (velocity.vY + newV.vY) / 2.0;
		position.vY += vY * dT;
		velocity.vY = newV.vY;
	}
}

double gameHero::left() { return hero.position.vX + heroSideMargin; }
double gameHero::right() { return hero.position.vX + heroSize - heroSideMargin; }
double gameHero::top() { return hero.position.vY + heroTopMargin; }
double gameHero::bottom() { return hero.position.vY + heroSize - heroBottomMargin; }

rectReal gameHero::rect() { return makeRectR(left(), top(), right(), bottom()); }

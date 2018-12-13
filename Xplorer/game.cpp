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
#include "handler.h"
#include "utility.h"
#include "geometry.h"
#include "gameLevel.h"
#include "gameFrame.h"
#include "animation.h"
#include "uiMainMenu.h"
#include "particles.h"
#include "uiStatistics.h"
#include "music.h"

extern bool isKeyDown[128];
extern DWORD lastJumpTime;
extern bool newLevelStarted;
extern int currentChapter, currentLevel;
extern gameFrame *currentFrame, *pausedFrame, *lastFrame;
extern gameFrame *mainFrame, *inGameFrame, *statisticsFrame;
extern gameFrame *startAnimationFrame, *deathAnimationFrame, *exitAnimationFrame;

extern ID2D1Bitmap *wallImage, *heroImage, *savedImage;
extern gameManager gameMaster;

extern gameHero hero;

extern ID2D1DCRenderTarget *mainRenderer;
extern IWICImagingFactory *imageFactory;
extern IDWriteFactory *writeFactory;
extern IDWriteTextFormat *textFormatNormal;
extern ID2D1SolidColorBrush *brushBlack, *brushWhite;
extern animation animator;

extern DWORD lastSaveTime;

extern int saveX, saveY;
extern std::vector<particle *> particles;

using json = nlohmann::json;

void gameHero::move(pointVector dV, double dT) {
	pointVector newV = velocity + dV * dT;
	limitVelocity(newV);
	if (face == directionLeft && !isKeyDown[VK_LEFT]) newV.vX = std::min(newV.vX, 0.0);
	if (face == directionRight && !isKeyDown[VK_RIGHT]) newV.vX = std::max(newV.vX, 0.0);
	if (lockX) velocity.vX = 0.0;
	else {
		double vX = (velocity.vX + newV.vX) / 2.0;
		position.vX += std::max(-0.49 * unitSize, std::min(vX * dT, 0.49 * unitSize));
		velocity.vX = newV.vX;
	}
	if (lockY) velocity.vY = 0.0;
	else {
		double vY = (velocity.vY + newV.vY) / 2.0;
		position.vY += std::max(-0.49 * unitSize, std::min(vY * dT, 0.49 * unitSize));
		velocity.vY = newV.vY;
	}
}

double gameHero::left() {
	return hero.position.vX + heroSideMargin;
}

double gameHero::right() {
	return hero.position.vX + heroSize - heroSideMargin;
}

double gameHero::top() {
	return hero.position.vY + heroTopMargin;
}

double gameHero::bottom() {
	return hero.position.vY + heroSize - heroBottomMargin;
}

rectReal gameHero::rect() {
	return makeRectR(left(), top(), right(), bottom());
}

polygon gameHero::poly() {
	polygon ans;
	double l = left(), t = top(), r = right(), b = bottom();
	ans.vertex.push_back(pointVector(l, t));
	ans.vertex.push_back(pointVector(l, b));
	ans.vertex.push_back(pointVector(r, b));
	ans.vertex.push_back(pointVector(r, t));
	return ans;
}

bool isFragileTileExist(int x, int y) {
	DWORD delta = timeGetTime() - theLevel().touchTime[x][y];
	return delta < fragileTimeLast || delta >= fragileTimeLast + fragileTimeDisappear;
}

bool isOutOfMap(double x, double y) { // returns whether the point specified is in the game scene
	return x < 0.0 || x >= (double)theLevel().columns * unitSize || y < 0.0 || y >= (double)theLevel().rows * unitSize;
}

bool isOutOfMap(rectReal rect) { // returns whether the rectangle is not completely contained in the game scene or not
	return isOutOfMap(rect.left, rect.top) || isOutOfMap(rect.left, rect.bottom) ||
		isOutOfMap(rect.right, rect.top) || isOutOfMap(rect.right, rect.bottom);
}

bool isOutOfMap(pointVector point) { // overloading of isOutOfMap(x, y)
	return isOutOfMap(point.vX, point.vY);
}

bool isWall(double x, double y) { // returns whether the point specified is a wall block
	int boardX = (int)floor(x / unitSize), boardY = (int)floor(y / unitSize);
	if (boardX < 0 || boardX >= theLevel().columns) return false;
	if (boardY < 0 || boardY >= theLevel().rows) return false;
	if (theLevel().grid[boardX][boardY] == blockWall) return true;
	if (theLevel().grid[boardX][boardY] == blockFragile && isFragileTileExist(boardX, boardY)) return true;
	return false;
}

bool isWall(pointVector point) { // overloading of isWall(x, y)
	return isWall(point.vX, point.vY);
}

bool isFragile(double x, double y) {
	int boardX = (int)floor(x / unitSize), boardY = (int)floor(y / unitSize);
	if (boardX < 0 || boardX >= theLevel().columns) return false;
	if (boardY < 0 || boardY >= theLevel().rows) return false;
	if (theLevel().grid[boardX][boardY] == blockFragile) return true;
	return false;
}

bool isFragile(pointVector point) {
	return isFragile(point.vX, point.vY);
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

void updateFragile(int X, int Y, DWORD timeNow) {
	DWORD delta = timeNow - theLevel().touchTime[X][Y];
	if (delta >= fragileTimeLast + fragileTimeDisappear) {
		theLevel().touchTime[X][Y] = timeNow;
	}
}

void updateHero() { // update hero data (position, velocity, ...)
	if (currentFrame != inGameFrame) return;
	if (hero.dead) return;

	static DWORD lastTime = timeGetTime();
	DWORD thisTime = timeGetTime();

	if (newLevelStarted) lastTime = thisTime, newLevelStarted = false;
	if (thisTime == lastTime) return;

	double interval = (thisTime - lastTime) / 1000.0;

	increase(theChapter().saveData[itemTime], double, interval);
	increase(gameMaster.saveData[itemTime], double, interval);

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
	}
	if ((!isWall(hero.left(), hero.bottom() + maxDelta) && !isWall(hero.right(), hero.bottom() + maxDelta)) && !isOutOfMap(hero.left(), hero.bottom() + maxDelta)) {
		hero.lockY = false; // if the hero is not standing on a block, and the hero is not on the bottom of the scene, then release it in direction y
		hero.jumpCount = std::max(1, hero.jumpCount);
	}
	/*
					if (delta >= fragileTimeDisappear + fragileTimeLast) {
						// the fragile tile has reappeared, ignore this judge branch
						// but before we continue, update the last touched time
						theLevel().touchTime[i][j] = timeCurrent;
					} else if (delta >= fragileTimeLast) {
						// the fragile tile has been hidden, so it's empty now
						continue;
					} else {
						// the fragile tile has just been stepped on, it's still wall
						// do not update touched time in this branch
					}*/
	if (isFragile(hero.left(), hero.bottom() + maxDelta)) {
		int X = (int)floor(hero.left() / unitSize), Y = (int)floor((hero.bottom() + maxDelta) / unitSize);
		updateFragile(X, Y, thisTime);
	}
	if (isFragile(hero.right(), hero.bottom() + maxDelta)) {
		int X = (int)floor(hero.right() / unitSize), Y = (int)floor((hero.bottom() + maxDelta) / unitSize);
		updateFragile(X, Y, thisTime);
	}

	deltaVelocity += gravityAcceleration;

	pointVector oldPosition = hero.position;
	hero.move(deltaVelocity, interval);
	pointVector newPosition = hero.position;
	double dist = distance(oldPosition, newPosition);

	increase(theChapter().saveData[itemDistance], double, dist);
	increase(gameMaster.saveData[itemDistance], double, dist);

	int adjustResult = heroPositionAdjust(hero);

	if (adjustResult & heroHitWall) {
		playMusic(soundCollision, true);
	}

	if (adjustResult & heroDead) {
		playMusic(soundPenalty);
		increase(theChapter().saveData[itemDeaths], int, 1);
		increase(gameMaster.saveData[itemDeaths], int, 1);
		heroDie();
	} else if (adjustResult & heroLevelUp) {
		levelUp();
	} else if (adjustResult & heroCheckpoint) {
		playMusic(soundSave);
		theLevel().saveData[itemLastX] = hero.position.vX;
		theLevel().saveData[itemLastY] = hero.position.vY;
		lastSaveTime = timeGetTime();
		gameMaster.saveData[itemLastSaved] = currentChapter;
		theChapter().saveData[itemLastSaved] = currentLevel;
	}
	lastTime = thisTime;
}

int heroPositionAdjust(gameHero &hero) {
	// Fix the hero's position with the map data. (Collision detection & adjustment)
	// Return value shows additional info (reserved for more usage).
	int ret = heroNothing;
	DWORD timeCurrent = timeGetTime();
	for (int j = std::max(0, (int)floor(hero.top() / unitSize)); j <= std::min(theLevel().rows - 1, (int)ceil(hero.bottom() / unitSize)); ++j) {
		for (int i = std::max(0, (int)floor(hero.left() / unitSize)); i <= std::min(theLevel().columns - 1, (int)ceil(hero.right() / unitSize)); ++i) {
			// We only need to check the squares the hero touches
			rectReal wallRect, heroRect;
			wallRect.left = (double)i * unitSize, wallRect.right = (i + 1.0) * unitSize;
			wallRect.top = (double)j * unitSize, wallRect.bottom = (j + 1.0) * unitSize;
			heroRect = hero.rect();
			if (!isRectIntersect(heroRect, wallRect)) continue;

			switch (theLevel().grid[i][j]) {
				case blockStartingPoint: case blockEmpty:
				{
					continue;
				}
				case blockFragile:
				{
					DWORD delta = timeCurrent - theLevel().touchTime[i][j];
					if (delta >= fragileTimeDisappear + fragileTimeLast) {
						// the fragile tile has reappeared, ignore this judge branch
						// but before we continue, update the last touched time
					} else if (delta >= fragileTimeLast) {
						// the fragile tile has been hidden, so it's empty now
						continue;
					} else {
						// the fragile tile has just been stepped on, it's still wall
						// do not update touched time in this branch
					}
					// NOTICE: no 'break' or 'continue' statement here. Fragile tiles may be regarded as both empty place (continue;) or wall block (go on case blockWall)
				}
				case blockWall:
				{
					pointVector delta = rectCenter(heroRect) - rectCenter(wallRect);
					if (dcmp(abs(delta.vX), abs(delta.vY)) >= 0) {
						// Crashed in x-axis. Fix in x-coordinate.
						ret |= heroFixLeft(hero, wallRect);
						ret |= heroFixRight(hero, wallRect);
					}
					if (dcmp(abs(delta.vX), abs(delta.vY)) < 0) {
						// Crashed in y-axis. Fix in y-coordinate.
						ret |= heroFixTop(hero, wallRect);
						ret |= heroFixBottom(hero, wallRect);
					}
					continue;
				}
				case blockCheckpoint:
				{
					saveX = i, saveY = j;
					ret |= heroCheckpoint;
					continue;
				}
				case blockWormhole:
				{
					double dist = (rectCenter(heroRect) - rectCenter(wallRect)).length();
					if (dist < unitSize * heroSizeMultiplier * 0.25) ret |= heroLevelUp;
					continue;
				}
				case blockNeedleUp:
				{
					polygon triangle;
					triangle.vertex.push_back(pointVector((i + 0.5) * unitSize, (double)j * unitSize));
					triangle.vertex.push_back(pointVector((double)i * unitSize, (double)(j + 1) * unitSize));
					triangle.vertex.push_back(pointVector((double)(i + 1) * unitSize, (double)(j + 1) * unitSize));
					if (isPolygonIntersect(triangle, hero.poly())) ret |= heroDead;
					continue;
				}
				case blockNeedleDown:
				{
					polygon triangle;
					triangle.vertex.push_back(pointVector((i + 0.5) * unitSize, (double)(j + 1) * unitSize));
					triangle.vertex.push_back(pointVector((double)i * unitSize, (double)j * unitSize));
					triangle.vertex.push_back(pointVector((double)(i + 1) * unitSize, (double)j * unitSize));
					if (isPolygonIntersect(triangle, hero.poly())) ret |= heroDead;
					continue;
				}
				case blockNeedleLeft:
				{
					polygon triangle;
					triangle.vertex.push_back(pointVector((double)i * unitSize, (j + 0.5) * unitSize));
					triangle.vertex.push_back(pointVector((double)(i + 1) * unitSize, (double)j * unitSize));
					triangle.vertex.push_back(pointVector((double)(i + 1) * unitSize, (double)(j + 1) * unitSize));
					if (isPolygonIntersect(triangle, hero.poly())) ret |= heroDead;
					continue;
				}
				case blockNeedleRight:
				{
					polygon triangle;
					triangle.vertex.push_back(pointVector((double)(i + 1) * unitSize, (j + 0.5) * unitSize));
					triangle.vertex.push_back(pointVector((double)i * unitSize, (double)j * unitSize));
					triangle.vertex.push_back(pointVector((double)i * unitSize, (double)(j + 1) * unitSize));
					if (isPolygonIntersect(triangle, hero.poly())) ret |= heroDead;
					continue;
				}
			}
		}
	}
	if (hero.position.vX < -heroSideMargin * 1.0) { // x-position fix into the game area
		hero.position.vX = -heroSideMargin;
		hero.velocity.vX = 0.0;
		hero.lockX = true;
		ret |= heroHitWall;
	} else if (hero.position.vX > theLevel().columns * unitSize - heroSize + heroSideMargin) {
		hero.position.vX = theLevel().columns * unitSize - heroSize + heroSideMargin;
		hero.velocity.vX = 0.0;
		hero.lockX = true;
		ret |= heroHitWall;
	}
	if (hero.position.vY < -heroTopMargin * 1.0) { // y-position fix into the game area
		hero.position.vY = -heroTopMargin;
		hero.velocity.vY = 0.0;
		hero.lockY = true;
		ret |= heroHitWall;
	} else if (hero.position.vY > theLevel().rows * unitSize - heroSize + heroBottomMargin) {
		//hero.position.vY = theLevel().rows * unitSize - heroSize + heroBottomMargin;
		//hero.velocity.vY = 0.0;
		//hero.lockY = true;
		//hero.jumpCount = 0;
		//ret |= heroHitWall;
		ret |= heroDead;
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
		return heroHitWall;
	}
	return heroNothing;
}

int heroFixBottom(gameHero &hero, rectReal object) {
	// Fix the hero's position so that it is exactly below the object
	if (isInInterval(hero.top(), object.bottom - unitSize / 2.0, object.bottom)) {
		hero.position.vY = object.bottom - heroTopMargin + minDelta;
		hero.lockY = true;
		hero.velocity.vY = 0.0;
		return heroHitWall;
	}
	return heroNothing;
}

int heroFixLeft(gameHero &hero, rectReal object) {
	// Fix the hero's position so that it is exactly on the left of the object
	if (isInInterval(hero.right(), object.left, object.left + unitSize / 2.0)) {
		hero.position.vX = object.left + heroSideMargin - heroSize - minDelta;
		hero.lockX = true;
		hero.velocity.vX = 0.0;
		hero.jumpCount = std::min(1, hero.jumpCount);
		return heroHitWall;
	}
	return heroNothing;
}

int heroFixRight(gameHero &hero, rectReal object) {
	// Fix the hero's position so that it is exactly below the object
	if (isInInterval(hero.left(), object.right - unitSize / 2.0, object.right)) {
		hero.position.vX = object.right - heroSideMargin + minDelta;
		hero.lockX = true;
		hero.velocity.vX = 0.0;
		hero.jumpCount = std::min(1, hero.jumpCount);
		return heroHitWall;
	}
	return heroNothing;
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
	rectFloat srcRect, destRect;
	getRenderRect(theLevel().frame->bitmap, hero, srcRect, destRect);
	renderGameFrame(theLevel().frame->bitmap, hero, srcRect, destRect, 1.f);
}

void levelUp() {
	playMusic(soundLevelUp);

	if (currentLevel + 1 < (int)theChapter().levels.size()) {
		theChapter().saveData[itemLastSaved] = currentLevel + 1;
	} else if (currentChapter + 1 < (int)gameMaster.chapters.size()) {
		gameMaster.saveData[itemLastSaved] = currentChapter + 1;
		gameMaster.chapters[currentChapter + 1].saveData[itemLastSaved] = 0;
	}

	hero.velocity = pointVector(0.0, 0.0);
	animator.startAnimation(theLevel().frame->bitmap, new linearAnimation(timeGetTime(), longAnimation), circularShrink, levelUpAnimationFinish);
}

void startLevel(int chapterID, int levelID) {
	currentChapter = currentLevel = -1;
	if (chapterID >= (int)gameMaster.chapters.size() || levelID >= (int)gameMaster.chapters[chapterID].levels.size()) return;
	currentChapter = chapterID, currentLevel = levelID;
	hero.velocity = pointVector(0.0, 0.0);
	hero.position = theLevel().initialPosition;
	hero.face = directionRight;
	hero.jumpCount = 0;
	hero.lockX = false;
	hero.lockY = false;
	hero.dead = false;
	if (theLevel().saveData[itemLastX] >= -epsilon && theLevel().saveData[itemLastY] >= -epsilon) {
		hero.position = pointVector(theLevel().saveData[itemLastX], theLevel().saveData[itemLastY]);
	}
	DWORD timeCurrent = timeGetTime();
	for (int i = 0; i < theLevel().columns; ++i) for (int j = 0; j < theLevel().rows; ++j) {
		if (theLevel().grid[i][j] == blockFragile) {
			theLevel().touchTime[i][j] = timeCurrent - fragileTimeLast - fragileTimeDisappear;
			continue; // Fragile tiles are not prerendered
		}
	}
	animator.startAnimation(theLevel().frame->bitmap, new linearAnimation(timeGetTime(), longAnimation), circularExpand, startLevelAnimationFinish);
}

void startLevelAnimationFinish() {
	releaseAllKeys();
	currentFrame = inGameFrame;
	newLevelStarted = true;
}

void levelUpAnimationFinish() {
	mainRenderer->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	animator.startAnimation(nullptr, new linearAnimation(timeGetTime(), shortAnimation), levelUpHoldFrame, levelUpHoldFinish);
}

void levelUpHoldFrame(ID2D1Bitmap *bitmap, double progress) {
	mainRenderer->Clear(D2D1::ColorF(D2D1::ColorF::Black));
}

void nextChapter() {
	if (currentChapter + 1 >= (int)gameMaster.chapters.size()) {
		// the whole game has finished
		showMainMenu(); // return to main frame
	} else {
		// next chapter
		startLevel(currentChapter + 1, 0);
	}
}

void levelUpHoldFinish() {
	if (currentLevel + 1 < (int)gameMaster.chapters[currentChapter].levels.size()) {
		// next level in the same chapter
		startLevel(currentChapter, currentLevel + 1);
	} else {
		// show chapter statistics
		loadStatistics(currentChapter);
		loadStatisticsFrame();
		switchToFrame(statisticsFrame);
		lastFrame = inGameFrame;
	}
}

void getRenderRect(ID2D1Bitmap *bitmap, gameHero &hero, rectFloat &srcRect, rectFloat &destRect) {
	D2D1_SIZE_F bitmapSize = bitmap->GetSize();
	float &width = bitmapSize.width, &height = bitmapSize.height;
	if (width >= windowClientWidth) {
		srcRect.left = std::max(0.f, std::min((float)hero.position.vX - windowClientWidth / 2.f, width - windowClientWidth));
		srcRect.right = srcRect.left + windowClientWidth;
	} else {
		srcRect.left = 0.f;
		srcRect.right = srcRect.left + width;
	}

	if (height >= windowClientHeight) {
		srcRect.top = std::max(0.f, std::min((float)hero.position.vY - windowClientHeight / 2.f, height - windowClientHeight));
		srcRect.bottom = srcRect.top + windowClientHeight;
	} else {
		srcRect.top = 0.f;
		srcRect.bottom = srcRect.top + height;
	}

	destRect.left = std::max(0.f, (windowClientWidth - width) / 2.f);
	destRect.top = std::max(0.f, (windowClientHeight - height) / 2.f);
	destRect.right = destRect.left + std::min(width, (float)windowClientWidth);
	destRect.bottom = destRect.top + std::min(height, (float)windowClientHeight);
}

void renderGameFrame(ID2D1Bitmap *bitmap, gameHero &hero, rectFloat srcRect, rectFloat destRect, float opacity) {
	mainRenderer->DrawBitmap(bitmap, destRect, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, srcRect);

	pointVector rectDelta(srcRect.left - destRect.left, srcRect.top - destRect.top);

	if (timeGetTime() - lastSaveTime < 500) {
		mainRenderer->DrawBitmap(savedImage, makeRectF(saveX * unitSize - srcRect.left + destRect.left, saveY * unitSize - srcRect.top + destRect.top, (saveX + 1) * unitSize - srcRect.left + destRect.left, (saveY + 1)*unitSize - srcRect.top + destRect.top), opacity);
	}

	DWORD timeCurrent = timeGetTime();

	ID2D1Bitmap *objects = nullptr;

	for (int j = std::max(0, (int)floor(srcRect.top / unitSize)); j <= std::min(theLevel().rows - 1, (int)ceil(srcRect.bottom / unitSize)); ++j) {
		for (int i = std::max(0, (int)floor(srcRect.left / unitSize)); i <= std::min(theLevel().columns - 1, (int)ceil(srcRect.right / unitSize)); ++i) {
			// We only need to check the squares within the srcRect
			rectReal wallRect;
			wallRect.left = (double)i * unitSize, wallRect.right = (i + 1.0) * unitSize;
			wallRect.top = (double)j * unitSize, wallRect.bottom = (j + 1.0) * unitSize;
			wallRect.right = wallRect.left + unitSize;
			wallRect.bottom = wallRect.top + unitSize;
			if (theLevel().grid[i][j] == blockFragile) {
				DWORD delta = timeCurrent - theLevel().touchTime[i][j];
				if (delta < fragileTimeLast || delta >= fragileTimeLast + fragileTimeDisappear) {
					mainRenderer->DrawBitmap(theLevel().objects,
					//	wallRect - srcRect + destRect,
						makeRectF(wallRect.left - rectDelta.vX, wallRect.top - rectDelta.vY, wallRect.right - rectDelta.vX, wallRect.bottom - rectDelta.vY),
						opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, makeRectF((float)blockFragile * unitSize, 0.f, (float)(blockFragile + 1) * unitSize, (float)unitSize)
					);
					//debugPrintF("%0.2lf %0.2lf %0.2lf %0.2lf\n", wallRect.left - rectDelta.vX, wallRect.top - rectDelta.vY, wallRect.right - rectDelta.vX, wallRect.top - rectDelta.vY);
				}
			}
		}
	}

	if (!hero.dead) {
		int heroStyle = getHeroState(hero.velocity);
		mainRenderer->DrawBitmap(heroImage, makeRectF(destRect.left + (float)hero.position.vX - srcRect.left, destRect.top + (float)hero.position.vY - srcRect.top, (float)(destRect.left + hero.position.vX + heroSize - srcRect.left), (float)(destRect.top + hero.position.vY + heroSize - srcRect.top)), opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, makeRectF(float(heroStyle * heroSize), 0.f, float((heroStyle + 1) * heroSize), (float)heroSize));
	}

//	WCHAR text[bufferSize];
//	swprintf_s(text, L"Level: %d-%d, Deaths: %d, Time: %lld, Distance: %lld\n", currentChapter, currentLevel, (int)gameMaster.saveData[itemDeaths], (longint)gameMaster.saveData[itemTime], (longint)gameMaster.saveData[itemDistance]);
//	drawText(mainRenderer, writeFactory, std::wstring(L"Segoe UI Light"), false, false, 20.f, std::wstring(text), makeRectF(0.f, 0.f, (float)windowClientWidth, windowClientHeight - 40.f), DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR, brushWhite);
}

void gameFrameEnter() {
	stopMusic(musicMain);
	playMusic(musicGame);
	if (lastFrame == pausedFrame) {
		animator.startAnimation(theLevel().frame->bitmap, new linearAnimation(timeGetTime(), mediumAnimation), circularExpand, startLevelAnimationFinish);
	} else {
		int ch = gameMaster.saveData[itemLastSaved], lv = -1;
		if (ch >= 0 && ch < gameMaster.chapters.size()) {
			lv = gameMaster.chapters[ch].saveData[itemLastSaved];
			if (lv < 0 || lv >= gameMaster.chapters[ch].levels.size()) lv = -1;
		} else ch = -1;
		if (lv < 0 || ch < 0) {
			increase(gameMaster.saveData[itemXplorations], int, 1);
			startLevel(0, 0);
		} else {
			startLevel(ch, lv);
		}
	}
}

void gameFrameLeave() {
	animator.startAnimation(theLevel().frame->bitmap, new linearAnimation(timeGetTime(), mediumAnimation), circularShrink, gameFrameLeaveFinish);
}

void gameFrameLeaveFinish() {
	mainRenderer->Clear(D2D1::ColorF(D2D1::ColorF::Black));
	loadNextFrame();
}

void heroDieAnimationFinish() {
	animator.startAnimation(nullptr, new linearAnimation(timeGetTime(), shortAnimation), nullptr, levelUpHoldFinish);
	currentFrame = inGameFrame;
	switchToFrame(inGameFrame);
	lastFrame = mainFrame;
}

void heroDieAnimationFrame(ID2D1Bitmap *bitmap, double progress) {
	rectFloat srcRect, destRect;
	getRenderRect(bitmap, hero, srcRect, destRect);
//	mainRenderer->DrawBitmap(bitmap, destRect, 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, srcRect);
	renderGameFrame(bitmap, hero, srcRect, destRect, 1.f);
	for (auto &x : particles) {
	//	x->update(srcRect);
		x->updateAndRender(mainRenderer, srcRect, destRect, (float)(1.0 - progress));
	}
}

void heroDie() {
	const int deathParticles = 700;
	clearParticles(particles);
	particles.resize(deathParticles);
	hero.dead = true;
	for (auto &x : particles) {
		x = new particle(particleGravitational, D2D1::ColorF(D2D1::ColorF::Red));
		x->p = rectCenter(hero.rect()) + pointVector(randomDouble(-maxDelta, maxDelta), randomDouble(-maxDelta, maxDelta));
	}
	animator.startAnimation(theLevel().frame->bitmap, new linearAnimation(timeGetTime(), maxParticleLife), heroDieAnimationFrame, heroDieAnimationFinish);
}

/*
	File Name: handler.cpp

	Window event handler for the game.
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
#include <d2d1.h>
#include <fstream>
#include <windows.h>
#include <wincodec.h>
#include "json.h"
#include "game.h"
#include "handler.h"
#include "utility.h"
#include "gameLevel.h"
#include "animation.h"
#include "uiMainMenu.h"
#include "uiPaused.h"

using json = nlohmann::json;

extern int currentChapter, currentLevel;
extern ID2D1Factory *d2dFactory;
extern ID2D1DCRenderTarget *mainRenderer;
extern IWICImagingFactory *imageFactory;
extern ID2D1Bitmap *bitmapBackground;
extern buttonUI *buttons[maxButton + 1];

extern bool isKeyDown[128];
extern gameHero hero;
extern DWORD lastJumpTime;

extern gameManager gameMaster;
//extern json saveData;

extern gameFrame *currentFrame, *lastFrame;
extern gameFrame *mainFrame, *inGameFrame;
extern gameFrame *animationFrame, *pausedFrame;

extern animation animator;
extern float dpiX, dpiY;

void initializeGame() {
	currentFrame = mainFrame;
	gameMaster.load("./chapters");
}

void gameTimer(HWND hwnd, UINT timerID) {
	if (currentFrame == inGameFrame) {
		updateHero();
	}
	InvalidateRect(hwnd, nullptr, false);
	UpdateWindow(hwnd);
}

void gameKeyDown(HWND hwnd, int keyCode) {
	if (!isInInterval(keyCode, 0, 128)) return;
	isKeyDown[keyCode] = true;
	if (currentFrame == inGameFrame) {
		if (keyCode == VK_LEFT) hero.face = directionLeft, hero.lockX = false;
		else if (keyCode == VK_RIGHT) hero.face = directionRight, hero.lockX = false;
		else if (keyCode == jumpKey) {
			++hero.jumpCount;
			if (hero.jumpCount <= 2) { // we allow only two jumps in a row
				hero.lockY = false;
				lastJumpTime = timeGetTime();
				hero.velocity.vY = jumpVelocityDelta[hero.jumpCount - 1].vY;
			}
		} else if (keyCode == VK_ESCAPE) {
			switchToFrame(pausedFrame);
		} else if (keyCode == VK_R || keyCode == VK_r) {
			switchToFrame(inGameFrame);
			lastFrame = mainFrame;
		} else if (keyCode == VK_F2) {
			switchToFrame(mainFrame);
		}
	}
}

void gameKeyUp(HWND hwnd, int keyCode) {
	if (!isInInterval(keyCode, 0, 128)) return;
	isKeyDown[keyCode] = false;
	if (currentFrame == inGameFrame) {
		if (keyCode == jumpKey) {
			if (timeGetTime() - lastJumpTime <= maxJumpTime * 0.4 && hero.jumpCount == 1) {
				hero.velocity.vY = std::max(0.0, hero.velocity.vY);
			}
		}
	}
}

void gameMouseDown(HWND hwnd, int button, double X, double Y) {
	X /= dpiX / 96.f, Y /= dpiY / 96.f;
}

void gameMouseUp(HWND hwnd, int button, double X, double Y) {
	X /= dpiX / 96.f, Y /= dpiY / 96.f;
	int buttonClicked = getClickedButtonID(X, Y);
	if (currentFrame == mainFrame) {
		if (buttonClicked == buttonExit) {
			PostQuitMessage(0);
		} else if (buttonClicked == buttonLoad) {
			disableAllButtons();
			switchToFrame(inGameFrame);
		} else if (buttonClicked == buttonStart) {
			disableAllButtons();
			deleteSave();
			switchToFrame(inGameFrame);
		}
	} else if (currentFrame == pausedFrame) {
		if (buttonClicked == buttonHome) {
		//	switchToFrame(mainFrame);
			gameKeyDown(hwnd, VK_F2);
		} else if (buttonClicked == buttonContinue) {
			switchToFrame(inGameFrame);
		} else if (buttonClicked == buttonRetry) {
		//	switchToFrame(inGameFrame);
		//	lastFrame = mainFrame;
			gameKeyDown(hwnd, VK_R);
		}
	}
}

void gameMouseMove(HWND hwnd, int button, double X, double Y) {
	X /= dpiX / 96.f, Y /= dpiY / 96.f;
}

void gamePaint(HWND hwnd) {
	if (mainRenderer == nullptr) return;
	if (currentFrame == nullptr) return;

	mainRenderer->BeginDraw();
	mainRenderer->Clear();

	if (currentFrame == animationFrame) {
		animator.routine();
	}
	
	if (currentFrame->brush != nullptr) {
		mainRenderer->FillRectangle(makeRectF(0.f, 0.f, (float)windowClientWidth, (float)windowClientHeight), currentFrame->brush);
	}
	if (currentFrame->render != nullptr) currentFrame->render();

	mainRenderer->EndDraw();
}

void releaseAllKeys() {
	memset(isKeyDown, 0, sizeof isKeyDown);
}

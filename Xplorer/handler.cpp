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
#include "uiStatistics.h"
#include "music.h"
#include "uiAbout.h"
#include "uiOptions.h"

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

extern gameFrame *currentFrame, *lastFrame, *aboutFrame;
extern gameFrame *mainFrame, *inGameFrame, *optionsFrame;
extern gameFrame *animationFrame, *pausedFrame;
extern gameFrame *statisticsFrame;

extern animation animator;
extern float dpiX, dpiY;
extern bool saveDeleted;

int buttonClicked = buttonNull;

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

void returnHome() {
	switchToFrame(mainFrame);
}

void retryGame() {
	switchToFrame(inGameFrame);
	lastFrame = mainFrame;
}

void pauseGame() {
	switchToFrame(pausedFrame);
}

void continueGame() {
	switchToFrame(inGameFrame);
}

void exitGame() {
	PostQuitMessage(0);
}

void loadGame() {
	disableAllButtons();
	switchToFrame(inGameFrame);
}

void newGame() {
	disableAllButtons();
	deleteSave();
	switchToFrame(inGameFrame);
}

void toStatistics() {
	disableAllButtons();
	loadStatistics(-1);
	loadStatisticsFrame();
	switchToFrame(statisticsFrame);
}

void toAbout() {
	disableAllButtons();
	loadAboutFrame();
	switchToFrame(aboutFrame);
}

void toOptions() {
	disableAllButtons();
	loadOptionsFrame();
	switchToFrame(optionsFrame);
}

void statisticsContinue() {
	if (lastFrame == mainFrame) {
		returnHome();
	} else if (lastFrame == inGameFrame) {
		continueGame();
		lastFrame = inGameFrame;
	}
}

void viewOnGitHub() {
	ShellExecute(NULL, "open", "https://github.com/Chrogeek/Xplorer", NULL, NULL, SW_SHOWNORMAL);
}

void gameKeyDown(HWND hwnd, int keyCode) {
	if (!isInInterval(keyCode, 0, 128)) return;
	isKeyDown[keyCode] = true;
	if (currentFrame == inGameFrame) {
		switch (keyCode) {
			case VK_LEFT:
				hero.face = directionLeft, hero.lockX = false;
				return;
			case VK_RIGHT:
				hero.face = directionRight, hero.lockX = false;
				return;
			case jumpKey:
				++hero.jumpCount;
				if (hero.jumpCount <= 2) { // we allow only two jumps in a row
					hero.lockY = false;
					lastJumpTime = timeGetTime();
					hero.velocity.vY = jumpVelocityDelta[hero.jumpCount - 1].vY;
					increase(gameMaster.saveData[itemJumps], int, 1);
					increase(gameMaster.chapters[currentChapter].saveData[itemJumps], int, 1);
					if (hero.jumpCount == 2) {
						increase(gameMaster.saveData[itemDoubleJumps], int, 1);
						increase(gameMaster.chapters[currentChapter].saveData[itemDoubleJumps], int, 1);
					}
				}
				return;
			case VK_ESCAPE:
			case VK_P:
			case VK_p:
				pauseGame();
				break;
			case VK_R:
			case VK_r:
				retryGame();
				break;
			case VK_F2:
				returnHome();
				break;
			default:
				return;
		}
	} else if (currentFrame == pausedFrame) {
		switch (keyCode) {
			case VK_ESCAPE:
			case VK_P:
			case VK_p:
			case VK_C:
			case VK_c:
				continueGame();
				break;
			case VK_R:
			case VK_r:
				retryGame();
				break;
			case VK_F2:
			case VK_H:
			case VK_h:
				returnHome();
				break;
			default:
				return;
		}
	} else if (currentFrame == mainFrame) {
		switch (keyCode) {
			case VK_ESCAPE:
			case VK_E:
			case VK_e:
				exitGame();
				break;
			case VK_N:
			case VK_n:
				newGame();
				break;
			case VK_L:
			case VK_l:
				loadGame();
				break;
			case VK_S:
			case VK_s:
				toStatistics();
				break;
			case VK_A:
			case VK_a:
				toAbout();
				break;
			case VK_O:
			case VK_o:
				toOptions();
				break;
			default:
				return;
		}
	} else if (currentFrame == statisticsFrame) {
		switch (keyCode) {
			case VK_C:
			case VK_c:
				statisticsContinue();
				break;
			default:
				return;
		}
	} else if (currentFrame == aboutFrame) {
		switch (keyCode) {
			case VK_B:
			case VK_b:
			case VK_F2:
			case VK_ESCAPE:
				returnHome();
				break;
			case VK_V:
			case VK_v:
				viewOnGitHub();
				break;
			default:
				return;
		}
	} else if (currentFrame == optionsFrame) {
		switch (keyCode) {
			case VK_B:
			case VK_b:
			case VK_F2:
			case VK_ESCAPE:
				returnHome();
				break;
			default:
				return;
		}
	} else return;
	playMusic(soundClick, true);
}

void gameKeyUp(HWND hwnd, int keyCode) {
	if (!isInInterval(keyCode, 0, 128)) return;
	isKeyDown[keyCode] = false;
	if (currentFrame == inGameFrame) {
		if (keyCode == jumpKey) {
			if (timeGetTime() - lastJumpTime <= maxJumpTime * 0.5 && hero.jumpCount == 1) {
				hero.velocity.vY = std::max(0.0, hero.velocity.vY);
			} 
		}
	}
}

void updateVolume(double X, double Y) {
	double newVolume = std::max(0.0, std::min((X - 190.0) / (windowClientWidth - 300.0), 1.0));
	setVolume((float)newVolume);
	gameMaster.saveData[itemVolume] = newVolume;
}

void gameMouseDown(HWND hwnd, int button, double X, double Y) {
	X /= dpiX / 96.f, Y /= dpiY / 96.f;
	buttonClicked = getClickedButtonID(X, Y);
	if (buttonClicked != buttonNull) {
		playMusic(soundClick, true);
	}
	if (currentFrame == optionsFrame) {
		if (buttonClicked == buttonVolumeSlider) {
			updateVolume(X, Y);
		}
	}
}

void gameMouseUp(HWND hwnd, int button, double X, double Y) {
	X /= dpiX / 96.f, Y /= dpiY / 96.f;
	buttonClicked = getClickedButtonID(X, Y);
	if (currentFrame == mainFrame) {
		if (buttonClicked == buttonExit) {
			exitGame();
		} else if (buttonClicked == buttonLoad) {
			loadGame();
		} else if (buttonClicked == buttonNew) {
			newGame();
		} else if (buttonClicked == buttonStatistics) {
			toStatistics();
		} else if (buttonClicked == buttonAbout) {
			toAbout();
		} else if (buttonClicked == buttonOptions) {
			toOptions();
		}
	} else if (currentFrame == pausedFrame) {
		if (buttonClicked == buttonHome) {
			returnHome();
		} else if (buttonClicked == buttonContinue) {
			continueGame();
		} else if (buttonClicked == buttonRetry) {
			retryGame();
		}
	} else if (currentFrame == statisticsFrame) {
		if (buttonClicked == buttonStatisticsContinue) {
			statisticsContinue();
		}
	} else if (currentFrame == aboutFrame) {
		if (buttonClicked == buttonAboutContinue) {
			returnHome();
		} else if (buttonClicked == buttonViewOnGitHub) {
			viewOnGitHub();
		}
	} else if (currentFrame == optionsFrame) {
		if (buttonClicked == buttonOptionsBack) {
			returnHome();
		} else if (buttonClicked == buttonDeleteSave) {
			deleteSave();
			saveDeleted = true;
		} else if (buttonClicked == buttonVolumeSlider) {
			updateVolume(X, Y);
		}
	}
	buttonClicked = buttonNull;
}

void gameMouseMove(HWND hwnd, int button, double X, double Y) {
	X /= dpiX / 96.f, Y /= dpiY / 96.f;
	if (currentFrame == optionsFrame) {
		if (buttonClicked == buttonVolumeSlider) {
			updateVolume(X, Y);
		}
	}
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
	//	mainRenderer->FillRectangle(makeRectF(0.f, 0.f, (float)windowClientWidth, (float)windowClientHeight), currentFrame->brush);
		mainRenderer->DrawBitmap(currentFrame->bitmap, makeRectF(0.f, 0.f, (float)windowClientWidth, (float)windowClientHeight));
	}
	if (currentFrame->render != nullptr) currentFrame->render();

	mainRenderer->EndDraw();
}

void releaseAllKeys() {
	memset(isKeyDown, 0, sizeof isKeyDown);
}

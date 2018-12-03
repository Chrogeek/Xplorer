/*
	File Name: global.cpp

	This file defines global variables.
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
#include <map>
#include <vector>
#include <d2d1.h>
#include <dwrite.h>
#include <windows.h>
#include <wincodec.h>
#include "game.h"
#include "utility.h"
#include "gameLevel.h"
#include "gameFrame.h"
#include "animation.h"

int currentChapter, currentLevel;
gameFrame *currentFrame = nullptr;

buttonUI *buttons[maxButton + 1];

//animationHelper *currentAnimation = nullptr;
animation currentAnimation;

// Direct2D factory objects
ID2D1Factory *d2dFactory = nullptr;
IWICImagingFactory *imageFactory = nullptr;
IDWriteFactory *writeFactory = nullptr;

// Global renderer
IDWriteTextFormat *textFormatNormal = nullptr;
ID2D1DCRenderTarget *mainRenderer = nullptr;

// Global images
ID2D1Bitmap *bkgImage = nullptr, *wallImage = nullptr, *heroImage = nullptr;
ID2D1Bitmap *bitmapBackground = nullptr, *savedImage = nullptr;
ID2D1SolidColorBrush *brushBlack = nullptr;

//ID2D1BitmapRenderTarget *mainFrame = nullptr;
gameFrame *mainFrame = nullptr, *inGameFrame = nullptr, *animationFrame = nullptr;
//gameFrame *startAnimationFrame = nullptr, *deathAnimationFrame = nullptr;
//gameFrame *exitAnimationFrame = nullptr;

float dpiX, dpiY;

gameHero hero;
bool isKeyDown[128];
UINT lastJumpTime;
UINT lastSaveTime;

json saveData;

gameManager gameMaster;

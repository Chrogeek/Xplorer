/*
	File Name: game.h

	Header file correlated to game.cpp.
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

#ifndef XPLORER_GAME_H
#define XPLORER_GAME_H

#include <d2d1.h>
#include "defs.h"
#include "utility.h"

int getHeroState(pointVector);
void renderGame();
void updateHero();
void renderGame();
//int heroPositionAdjust(pointVector &, pointVector &);

struct gameStage {
	ID2D1Bitmap *bkgImage;
	int n, m, blocks[mapWidth][mapHeight];
	XplorerResult loadFromFile(const WCHAR *);
	pointVector initialPosition;
	~gameStage();
};

void newStage(int);

struct gameHero {
	pointVector position, velocity;
	bool lockX, lockY;
	XplorerDirection face;
	int jumpCount;
	void move(pointVector, float);
};

int heroPositionAdjust(gameHero &);
int heroFixTop(gameHero &, D2D1_RECT_F);
int heroFixBottom(gameHero &, D2D1_RECT_F);
int heroFixLeft(gameHero &, D2D1_RECT_F);
int heroFixRight(gameHero &, D2D1_RECT_F);

#endif

/*
	File Name: game.h

	This file defines objects used in game main process.
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
#include "json.h"
#include "defs.h"
#include "utility.h"
#include "geometry.h"

using json = nlohmann::json;

struct gameHero {
	pointVector position, velocity;
	bool lockX, lockY;
	directionX face;
	int jumpCount;
	void move(pointVector, double);
	double left();
	double top();
	double right();
	double bottom();
	rectReal rect();
	polygon poly();
};

bool isOutOfMap(double, double);
bool isOutOfMap(rectReal);
bool isOutOfMap(pointVector);

bool isWall(double, double);
bool isWall(pointVector);

int xWall(gameHero);
int yWall(gameHero);

void updateHero();

int heroPositionAdjust(gameHero &);
int heroFixTop(gameHero &, rectReal);
int heroFixBottom(gameHero &, rectReal);
int heroFixLeft(gameHero &, rectReal);
int heroFixRight(gameHero &, rectReal);

int getHeroState(pointVector);

void renderGame();

void levelUp();

void startLevel(int, int);
void startLevelAnimationFinish();
void levelUpAnimationFinish();

void levelUpHoldFrame(ID2D1Bitmap *, double);
void levelUpHoldFinish();

void getRenderRect(ID2D1Bitmap *, gameHero &, rectFloat &, rectFloat &);
void renderGameFrame(ID2D1Bitmap *, gameHero &, rectFloat, rectFloat, float);

void gameFrameEnter();
void gameFrameLeave();
void gameFrameLeaveFinish();

#endif

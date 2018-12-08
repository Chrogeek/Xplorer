/*
	File Name: handler.h

	Header file correlated to handler.cpp.
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

#ifndef XPLORER_HANDLER_H
#define XPLORER_HANDLER_H

#include <windows.h>

void initializeGame();

void gameTimer(HWND, UINT);
void gameKeyDown(HWND, int);
void gameKeyUp(HWND, int);
void gameMouseDown(HWND, int, double, double);
void gameMouseUp(HWND, int, double, double);
void gameMouseMove(HWND, int, double, double);
void gamePaint(HWND);

void releaseAllKeys();

#endif

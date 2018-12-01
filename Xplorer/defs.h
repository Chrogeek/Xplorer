/*
	File Name: defs.h

	This file defines global constant symbols (constants,
	enumerations and macros).
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

#ifndef XPLORER_DEFS_H
#define XPLORER_DEFS_H

#define NOMINMAX // disable visual c++'s default min/max macros

#include <d2d1.h>

typedef D2D1_RECT_F rectFloat;

#include <windows.h>
#include "utility.h"

const int bufferSize = 4096;
const int smallBufferSize = 256;

const char appName[] = "Xplorer";
const int appVersion = MAKELONG(MAKEWORD(0, 0), MAKEWORD(0, 0));
const char appVersionString[] = "0.1";
const char appEdition[] = "pre-Alpha";

const int windowClientWidth = 896;
const int windowClientHeight = 672;

const WCHAR gameFontName[] = L"Microsoft YaHei UI";

const WCHAR wallImageName[] = L"images/wall.png";
const WCHAR heroImageName[] = L"images/hero.png";

const int XplorerLeftButton = 1;
const int XplorerRightButton = 2;
const int XplorerMiddleButton = 4;

const int stageMainMenu = -2;
const int stageOptions = -1;
const int stageTutorial = 0;

const int maxStage = 1;

const int buttonStart = 1;
const int buttonExit = 2;
const int buttonOptions = 3;
const int buttonTutorial = 4;
const int maxButton = 4;

const int buttonNull = -1;

const int unitSize = 32;

const double heroSizeMultiplier = 1.4;
const double heroSize = unitSize * heroSizeMultiplier;
const double heroSideMargin = 9 * heroSizeMultiplier;
const double heroTopMargin = 10 * heroSizeMultiplier;
const double heroBottomMargin = 0 * heroSizeMultiplier;

//const int mapWidth = 28;
//const int mapHeight = 21;

const int heroLeft = 12;
const int heroRight = 0;
const int heroStanding = 0;
const int heroWalking = 4;
const int heroFalling = 8;
const int heroJumping = 10;

/*const int blockEmpty = 0;
const int blockWall = 1;
const int blockStartingPoint = 2;
const int blockCheckpoint = 3;*/

const int blockEmpty = -1;
const int blockNeedleUp = 2;
const int blockNeedleDown = 3;
const int blockNeedleRight = 4;
const int blockNeedleLeft = 5;
const int blockCheckpoint = 6;
const int blockWormhole = 7;
const int blockWall = 15;
const int blockStartingPoint = 1;

const int maxFPS = 80;
const int timerInterval = int(1000.0 / maxFPS);

const pointVector maxVelocity = {6.0 * unitSize, 30.0 * unitSize};
const pointVector minVelocity = {-6.0 * unitSize, -30.0 * unitSize};
const pointVector jumpStartVelocity = {0.0, -14.0 * unitSize};
const pointVector jumpAcceleration = {0.0, -25.0 * unitSize};
const pointVector gravityAcceleration = {0.0, 40.0 * unitSize};
const pointVector moveAcceleration = {18.0 * unitSize, 0.0};

const double epsilon = 4e-7;
const double minDelta = 0.1;
const double midDelta = 0.3;
const double maxDelta = 0.5;

const UINT gameTimerID = 100;

const int jumpKey = VK_SHIFT;
const UINT maxJumpTime = 150; // max jump key hold time in milliseconds

enum XplorerResult {
	fileBroken = -1,
	okay = 0,
	direct2DError = -3
};

enum directionX {
	directionLeft = 0,
	directionRight
};

enum direction2D {
	direction2DLeft = 0,
	direction2DUp,
	direction2DRight,
	direction2DDown
};

/* Key code constants
 *
 * The header file <winuser.h> mentioned codes for number and alpha keys,
 * but did not define them. Below are definitions of these virtual key
 * macros. Macro values are the ASCII codes (hexadecimal) for the characters.
 *
 * According to <winuser.h>:
 * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
 * 0x40 : unassigned
 * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
 */

// Number keys
#define VK_0 (0x30)
#define VK_1 (0x31)
#define VK_2 (0x32)
#define VK_3 (0x33)
#define VK_4 (0x34)
#define VK_5 (0x35)
#define VK_6 (0x36)
#define VK_7 (0x37)
#define VK_8 (0x38)
#define VK_9 (0x39)
// Uppercase alpha keys
#define VK_A (0x41)
#define VK_B (0x42)
#define VK_C (0x43)
#define VK_D (0x44)
#define VK_E (0x45)
#define VK_F (0x46)
#define VK_G (0x47)
#define VK_H (0x48)
#define VK_I (0x49)
#define VK_J (0x4A)
#define VK_K (0x4B)
#define VK_L (0x4C)
#define VK_M (0x4D)
#define VK_N (0x4E)
#define VK_O (0x4F)
#define VK_P (0x50)
#define VK_Q (0x51)
#define VK_R (0x52)
#define VK_S (0x53)
#define VK_T (0x54)
#define VK_U (0x55)
#define VK_V (0x56)
#define VK_W (0x57)
#define VK_X (0x58)
#define VK_Y (0x59)
#define VK_Z (0x5A)
// Lowercase alpha keys
#define VK_a (0x61)
#define VK_b (0x62)
#define VK_c (0x63)
#define VK_d (0x64)
#define VK_e (0x65)
#define VK_f (0x66)
#define VK_g (0x67)
#define VK_h (0x68)
#define VK_i (0x69)
#define VK_j (0x6A)
#define VK_k (0x6B)
#define VK_l (0x6C)
#define VK_m (0x6D)
#define VK_n (0x6E)
#define VK_o (0x6F)
#define VK_p (0x70)
#define VK_q (0x71)
#define VK_r (0x72)
#define VK_s (0x73)
#define VK_t (0x74)
#define VK_u (0x75)
#define VK_v (0x76)
#define VK_w (0x77)
#define VK_x (0x78)
#define VK_y (0x79)
#define VK_z (0x7A)

#endif

#ifndef XPLORER_HANDLER_H
#define XPLORER_HANDLER_H

#include <windows.h>

void initializeGame();

void gameTimer(HWND, UINT);
void gameKeyDown(HWND, int);
void gameKeyUp(HWND, int);
void gameMouseDown(HWND, int, int, int);
void gameMouseUp(HWND, int, int, int);
void gameMouseMove(HWND, int, int, int);
HRESULT gamePaint(HWND);

#endif

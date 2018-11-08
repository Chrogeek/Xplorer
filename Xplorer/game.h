#ifndef XPLORER_GAME_H
#define XPLORER_GAME_H

#include "defs.h"

XplorerResult loadMap(const WCHAR *);
int getHeroState(XplorerDirection, float, int);
HRESULT renderGame(ID2D1HwndRenderTarget *, IWICImagingFactory *);
void updateHero();

#endif

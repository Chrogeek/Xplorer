#ifndef XPLORER_GAMEFRAME_H
#define XPLORER_GAMEFRAME_H

#include "defs.h"
#include <d2d1.h>
#include "utility.h"

struct gameFrame {
	ID2D1BitmapRenderTarget *renderer;
	ID2D1Bitmap *bitmap;
	ID2D1BitmapBrush *brush;
	voidFunction render, leave, enter;
	gameFrame();
	~gameFrame();
};

#endif

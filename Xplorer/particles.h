#ifndef XPLORER_PARTICLES_H
#define XPLORER_PARTICLES_H

#include "defs.h"
#include <d2d1.h>
#include "geometry.h"

struct particle {
	pointVector p, v;
	int type;
	D2D1_COLOR_F color;
	DWORD initTime, lastTime;
	particle(int, D2D1_COLOR_F);
	bool update(rectFloat);
	void render(ID2D1RenderTarget *, rectFloat, rectFloat, float);
	bool updateAndRender(ID2D1RenderTarget *, rectFloat, rectFloat, float);
};

#endif

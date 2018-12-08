#ifndef XPLORER_PARTICLES_H
#define XPLORER_PARTICLES_H

#include "defs.h"
#include <d2d1.h>
#include "geometry.h"

struct particle {
	pointVector p, v;
	int type;
	D2D1_COLOR_F color;
	particle(int);
};

#endif

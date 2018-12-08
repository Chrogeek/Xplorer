#ifndef XPLORER_ANIMATION_H
#define XPLORER_ANIMATION_H

#include "defs.h"
#include <d2d1.h>
#include <windows.h>
#include "geometry.h"

struct animationHelper {
	DWORD timeStart, totalTime;
	virtual double progress(DWORD) = 0;
};

struct linearAnimation : public animationHelper {
	double progress(DWORD);
	linearAnimation(DWORD, DWORD);
};

struct logarithmicAnimation : public animationHelper {
	double progress(DWORD);
	logarithmicAnimation(DWORD, DWORD);
};

struct arcAnimation : public animationHelper {
	double progress(DWORD);
	arcAnimation(DWORD, DWORD);
};

struct animation {
	ID2D1Bitmap *image; // which image to animate on
	animationHelper *helper;
	voidBitmapDoubleFunction onFrame;
	voidFunction finish;
	void startAnimation(ID2D1Bitmap *, animationHelper *, voidBitmapDoubleFunction, voidFunction);
	void routine();
	bool expired();
	animation();
};

void circularExpand(ID2D1Bitmap *, double);
void crossOut(ID2D1Bitmap *, double);
void circularShrink(ID2D1Bitmap *, double);
void crossIn(ID2D1Bitmap *, double);
void fadeIn(ID2D1Bitmap *, double);
void fadeOut(ID2D1Bitmap *, double);
#endif

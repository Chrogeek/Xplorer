#ifndef XPLORER_UIABOUT_H
#define XPLORER_UIABOUT_H

#include "defs.h"
#include <d2d1.h>

void loadAboutFrame();
void renderAbout();
void showAboutFinish();
void showAbout();
void showAboutFrame(ID2D1Bitmap *, double);

void leaveAbout();
void leaveAboutFrame(ID2D1Bitmap *, double);
void leaveAboutFinish();

#endif
#pragma once

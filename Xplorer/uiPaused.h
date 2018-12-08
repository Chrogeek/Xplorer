#ifndef XPLORER_UIPAUSED_H
#define XPLORER_UIPAUSED_H

#include "defs.h"
#include <d2d1.h>

void loadPausedFrame();
void renderPaused();
void showPausedFinish();
void showPaused();
void showPausedFrame(ID2D1Bitmap *, double);

void leavePaused();
void leavePausedFrame(ID2D1Bitmap *, double);
void leavePausedFinish();

#endif

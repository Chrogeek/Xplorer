#ifndef XPLORER_UIOPTIONS_H
#define XPLORER_UIOPTIONS_H

#include "defs.h"
#include <d2d1.h>

void loadOptionsFrame();
void renderOptions();
void showOptionsFinish();
void showOptions();
void showOptionsFrame(ID2D1Bitmap *, double);

void leaveOptions();
void leaveOptionsFrame(ID2D1Bitmap *, double);
void leaveOptionsFinish();

#endif

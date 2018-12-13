#ifndef XPLORER_UISTATISTICS_H
#define XPLORER_UISTATISTICS_H

#include "defs.h"
#include <string>
#include <d2d1.h>
#include "gameLevel.h"

void loadStatisticsFrame();
void renderStatistics();

void loadStatistics(int);

void showStatisticsFinish();
void showStatistics();
void showStatisticsFrame(ID2D1Bitmap *, double);

void leaveStatistics();
void leaveStatisticsFrame(ID2D1Bitmap *, double);
void leaveStatisticsFinish();

#endif

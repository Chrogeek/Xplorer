#ifndef XPLORER_GDIPLUS_H
#define XPLORER_GDIPLUS_H

#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

Status gdipInitialize();
void gdipTerminate();

#endif

#pragma comment(lib, "gdiplus.lib")
#include <cassert>
#include <windows.h>
#include "fui/fui.h"
#include "ui.h"
#include "utility.h"
using namespace fui;

extern fuiObject *rootObject;
extern HWND fuiMainhWindow;

void mainProc(fuiEventType event, int arrayIndex, int arg1, int arg2, int arg3, int arg4) {
	switch (event) {
		case fuiEventResize:
		{
			rootObject->refresh();
			break;
		}
		case fuiEventKeyDown:
		{
			if (arg1 == VK_X && arg2 == fuiKeyCtrl) {
				terminateProgram();
			}
		}
		case fuiEventTerminate:
		{
			terminateProgram();
		}
	}
}
void fuiMain() {
	rootObject = fuiInitialize(fuiMainhWindow, mainProc);
	rootObject->refresh();
	rootObject->setFocus();
	initGraphics();
	UIMainMenu::loadMainMenu();
	UIMainMenu::drawMainMenu();
}

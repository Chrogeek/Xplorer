#ifndef XPLORER_UI_H
#define XPLORER_UI_H

#include "fui/fui.h"
#include "gdiPlus.h"
using namespace Gdiplus;
using namespace fui;

class XplorerUIButton {
private:
	Image *buttonImage;
	fuiObject *obj;
	Graphics *graphics;
public:
	void drawButton();
	XplorerUIButton(int, int, int, int, int, const WCHAR *, fuiProcType, int = 0);
	~XplorerUIButton();
	void refresh();
	int left();
	void left(int);
	int top();
	void top(int);
	int width();
	void width(int);
	int height();
	void height(int);
	bool enabled();
	void enabled(bool);
	void reloadImage(const WCHAR *);
};

namespace UIMainMenu {
	void startButtonProc(fuiEventType, int, int, int, int, int);
	void drawMainMenu();
	void loadMainMenu();
	void terminateMainMenu();
}

void initGraphics();
void terminateGraphics();
void terminateProgram();
#endif
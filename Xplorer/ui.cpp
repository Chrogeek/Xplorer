#include "ui.h"

extern HWND fuiMainhWindow;
extern ULONG_PTR gdiplusToken;

fuiObject *rootObject;
Graphics *mainGraphics;

void XplorerUIButton::drawButton() {
	graphics->DrawImage(buttonImage, 0, 0, obj->width(), obj->height());
	obj->refresh();
}
XplorerUIButton::XplorerUIButton(int parentID, int left, int top, int width, int height, const WCHAR *fileName, fuiProcType proc, int arrayIndex) {
	obj = new fuiObject();
	obj->initialize(arrayIndex, left, top, width, height, true, parentID, proc);
	obj->layered(true);
	obj->hitInTrans(true);
	buttonImage = new Image(fileName);
	graphics = new Graphics(obj->hDC());
	this->drawButton();
}
XplorerUIButton::~XplorerUIButton() {
	delete buttonImage;
	delete obj;
	delete graphics;
}
void XplorerUIButton::refresh() {
	obj->refresh();
}
int XplorerUIButton::left() { return obj->left(); }
void XplorerUIButton::left(int newValue) { obj->left(newValue); }
int XplorerUIButton::top() { return obj->top(); }
void XplorerUIButton::top(int newValue) { obj->top(newValue); }
int XplorerUIButton::width() { return obj->width(); }
void XplorerUIButton::width(int newValue) { obj->width(newValue); }
int XplorerUIButton::height() { return obj->height(); }
void XplorerUIButton::height(int newValue) { obj->height(newValue); }
bool XplorerUIButton::enabled() { return obj->enabled(); }
void XplorerUIButton::enabled(bool newValue) { obj->enabled(newValue); }
void XplorerUIButton::reloadImage(const WCHAR *fileName) {
	delete buttonImage;
	buttonImage = new Image(fileName);
	drawButton();
}

namespace UIMainMenu {
	XplorerUIButton *startButton;
	Image *backgroundImage;
	bool mainMenuLoaded = false;
	void startButtonProc(fuiEventType event, int arrayIndex, int arg1, int arg2, int arg3, int arg4) {
		switch (event) {
			case fuiEventMouseUp:
			{
				MessageBox(fuiMainhWindow, "You clicked the start button!", "Info", MB_OK | MB_ICONINFORMATION);
			}
		}
	}
	void drawMainMenu() {
		mainGraphics->DrawImage(backgroundImage, 0, 0, windowClientWidth, windowClientHeight);
		rootObject->refresh();
	}
	void loadMainMenu() {
		if (mainMenuLoaded) return;
		backgroundImage = new Image(L"images/bg_blue.png");
		startButton = new XplorerUIButton(fuiRootID, 323, 270, 148, 60, L"images/start.png", startButtonProc, 0);
		mainMenuLoaded = true;
	}
	void terminateMainMenu() {
		if (!mainMenuLoaded) return;
		delete startButton;
		delete backgroundImage;
		mainMenuLoaded = false;
		rootObject->refresh();
	}
}

void initGraphics() {
	gdipInitialize();
	mainGraphics = new Graphics(rootObject->hDC());
}

void terminateGraphics() {
	if (gdiplusToken == 0) return;
	delete mainGraphics;
	gdipTerminate();
}

void terminateProgram() {
	UIMainMenu::terminateMainMenu();
	terminateGraphics();
	PostQuitMessage(0);
}
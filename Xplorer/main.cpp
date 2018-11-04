#include "fui/fui.h"
#include <ctime>
#include <cstdio>
#include <cstdlib>
using namespace fui;
fuiObject uiobj[1000], root;
HBRUSH oBrush[1000];
fuiTimer uitimer;
FILE *fout;
int alpha[1000], dir[1000];
void timerProc(fuiEventType event, int index, int arg1, int arg2, int arg3, int arg4) {
	char ch[50];
	_itoa_s(rand(), ch, 50, 10);
	SetWindowText(fuiMainhWindow, ch);
	for (index = 0; index < 50; ++index) {
		if (dir[index] == 0)
			dir[index] = 1;
		alpha[index] += 10 * dir[index];
		if (alpha[index] >= 256)
			alpha[index] = 254, dir[index] = -1;
		if (alpha[index] <= -1)
			alpha[index] = 1, dir[index] = 1;
		uiobj[index].alpha(alpha[index]);
		uiobj[index].refresh();
	}
}
void mainProc(fuiEventType event, int arrayIndex, int arg1, int arg2, int arg3, int arg4) {
	switch (event) {
		case fuiEventResize:
		{
			root.refresh();
			break;
		}
	}
}
void test_proc(fuiEventType event, int arrayIndex, int arg1, int arg2, int arg3, int arg4) {
	static int cx, cy;
	switch (event) {
		case fuiEventMouseMove:
		{
			if (arg1 & fuiButtonLeft) {
				fuiObject &o = uiobj[arrayIndex];
				o.move(o.left() + arg3 - cx, o.top() + arg4 - cy);
				root.refresh();
			} else if (arg1 & fuiButtonRight) {
				fuiObject &o = uiobj[arrayIndex];
				o.size(o.width() + arg3 - cx, o.height() + arg4 - cy);
				cx = arg3, cy = arg4;
				gdiFillRect(uiobj[arrayIndex].hDC(), oBrush[arrayIndex], 0, 0, o.width(), o.height());
				root.refresh();
			}
			break;
		}
		case fuiEventMouseDown:
		{
			cx = arg3;
			cy = arg4;
			break;
		}
	}
}
void fuiMain() {
	root = *fuiInitialize(fuiMainhWindow, mainProc);
	root.refresh();
	srand((unsigned)time(0));
	for (int i = 0; i < 50; i++) {
		int r = i / 10, c = i % 10;
		uiobj[i].initialize(i, c * 50, r * 50, 50, 50, true, fuiRootID, test_proc);
		uiobj[i].alpha(alpha[i] = i * 5);
		oBrush[i] = gdiCreateBrush(uiobj[i].hDC(), RGB(rand() & 0xff, rand() & 0xff, rand() & 0xff));
		gdiFillRect(uiobj[i].hDC(), oBrush[i], 0, 0, 50, 50);
	}
	uitimer.initialize(0, 50, timerProc);
}

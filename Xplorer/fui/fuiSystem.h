#ifndef FUISYSTEM_H
#define FUISYSTEM_H

#include <queue>
#include <vector>
#include <cstring>
#include <algorithm>
#include "fuiGDI.h"
#pragma comment(lib, "Msimg32.lib")
#include <cassert>
using namespace std;
namespace fui {
	class fuiObject {
	public:
		int id();
		HDC hDC();
		int left();             void left(int);
		int top();              void top(int);
		int width();            void width(int);
		int height();           void height(int);
		int transparentColor(); void transparentColor(int);
		bool clicked();         void clicked(bool);
		bool enabled();         void enabled(bool);
		bool visible();         void visible(bool);
		bool layered();         void layered(bool);
		bool transparent();     void transparent(bool);
		bool hitInTrans();      void hitInTrans(bool);
		byte alpha();           void alpha(byte);
		fuiProcType proc();          void proc(fuiProcType);
		void move(int, int, int = -1, int = -1);
		void size(int, int);
		void refresh(int = 0, int = 0, int = -1, int = -1);
		void setFocus();
		void zOrder(int = -1);
		void raiseEvent(fuiEventType, int, int, int, int, int);
		void initialize(int = 0, int = 0, int = 0, int = 0, int = 0, bool = false, int = 0, fuiProcType = NULL);
		~fuiObject();
	private:
		int _id;
		fuiProcType _proc;
	};
	class fuiTimer {
	public:
		int id();
		bool enabled();	void enabled(bool);
		int interval();	void interval(int);
		fuiProcType proc();	void proc(fuiProcType);
		void initialize(int = 0, int = 0, fuiProcType = NULL);
		void raiseEvent(fuiEventType, int, int, int, int, int);
		~fuiTimer();
	private:
		int _id;
		fuiProcType _proc;
	};
	struct object {
		int arrayIndex;
		HDC hDC;
		HBITMAP bitmap;
		VOID **bitmapData;
		int left, top, width, height;
		int bitmapWidth, bitmapHeight;
		int transparentColor;
		byte style, alpha;
		int parentID;
		int nextID, lastID;
		int firstChild, lastChild;
		fuiObject *obj;
		bool isEmpty;
	};
	struct timer {
		int arrayIndex;
		int interval;
		byte style;
		fuiTimer *obj;
		bool isEmpty;
		HANDLE hTimer;
	};
	fuiObject *fuiInitialize(HWND, fuiProcType = NULL);
	void fuiTerminate();
	int fuiCreateObject(int, fuiObject &, int, int, int, int, bool, int);
	void fuiDeleteObject(int);
	int fuiCreateTimer(int, fuiTimer &);
	void fuiDeleteTimer(int);
	HDC fuiGethDC(int);
	int fuiGetLeft(int);
	void fuiSetLeft(int, int);
	int fuiGetTop(int);
	void fuiSetTop(int, int);
	int fuiGetWidth(int);
	void fuiSetWidth(int, int);
	int fuiGetHeight(int);
	void fuiSetHeight(int, int);
	int fuiGetTransparentColor(int);
	void fuiSetTransparentColor(int, int);
	bool fuiGetClicked(int);
	void fuiSetClicked(int, bool);
	bool fuiGetEnabled(int);
	void fuiSetEnabled(int, bool);
	bool fuiGetVisible(int);
	void fuiSetVisible(int, bool);
	bool fuiGetLayered(int);
	void fuiSetLayered(int, bool);
	bool fuiGetTransparent(int);
	void fuiSetTransparent(int, bool);
	bool fuiGetHitInTrans(int);
	void fuiSetHitInTrans(int, bool);
	byte fuiGetAlpha(int);
	void fuiSetAlpha(int, byte);
	void fuiMove(int, int, int, int, int);
	void fuiSize(int, int, int);
	void fuiRefresh(int, int, int, int, int);
	void fuiSetFocus(int);
	void fuiZOrder(int, int);
	VOID CALLBACK fuiTimerProc(PVOID, BOOLEAN);
	int fuiGetTimerInterval(int);
	void fuiSetTimerInterval(int &, int);
	bool fuiGetTimerEnabled(int);
	void fuiSetTimerEnabled(int &, bool);
	LRESULT CALLBACK fuiWindowProc(HWND, UINT, WPARAM, LPARAM);
	void createLinkedListObject(int, int);
	void addLinkedListFront(int, int);
	void addLinkedListBack(int, int);
	void jointLinkedList(int);
	void deleteLinkedList(int);
	void setLinkedListParent(int, int);
	void setLinkedListZOrder(int, int);
	int getLinkedListZOrder(int);
	int getLinkedListObjectID(int, int);
	int getLinkedListCount(int);
	void getLinkedListObjectAbsolutePosition(int, int &, int &);
	int getLinkedListIDWithPosition(int, int, int, int &, int &);
	void refreshLinkedListObject(int, int, int, int, int, int, int);
};

#endif

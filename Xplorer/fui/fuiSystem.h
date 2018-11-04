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
		void setfocus();
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
	namespace {
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
		HANDLE hTimerQueue;
		queue<int> objectPool, timerPool;
		vector<object> objectData;
		vector<timer> timerData;
		HWND fuihWindow;
		HDC fuihDC;
		HBITMAP fuiBitmap;
		fuiObject *fuiObjectRoot;
		int fuiBitmapWidth, fuiBitmapHeight;
		int fuiFocus, fuiLastMove;
		WNDPROC fuiOriginalProc;
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
		LRESULT CALLBACK fuiWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
			static TRACKMOUSEEVENT tMouseTrack;
			static PAINTSTRUCT vPaintStruct;
			static RECT vRect;
			static POINT vPoint;
			static bool isMouseTracked;
			static int lastKeyCode;
			int newFocus;
			int newMouseX, newMouseY;
			int newButton, newShift;
			switch (msg) {
				case WM_MOUSEMOVE:
				{
					if (!isMouseTracked) {
						isMouseTracked = true;
						tMouseTrack.dwFlags = TME_LEAVE;
						tMouseTrack.hwndTrack = hwnd;
						TrackMouseEvent(&tMouseTrack);
					}
					newButton = (bool(wparam & MK_LBUTTON) * fuiButtonLeft) | (bool(wparam & MK_RBUTTON) * fuiButtonRight) | (bool(wparam & MK_MBUTTON) * fuiButtonMiddle);
					newShift = (bool(wparam & MK_SHIFT) * fuiKeyShift) | (bool(wparam & MK_CONTROL) * fuiKeyCtrl);
					if (newButton == 0) {
						newFocus = getLinkedListIDWithPosition(LOWORD(lparam), HIWORD(lparam), 0, newMouseX, newMouseY);
						if (fuiLastMove != newFocus) {
							if (fuiLastMove != fuiObjectNull) {
								object &o = objectData[fuiLastMove];
								if (o.style & fuiStyleEnabled)
									o.obj->raiseEvent(fuiEventMouseLeave, o.arrayIndex, 0, 0, 0, 0);
							}
							fuiLastMove = newFocus;
							object &o = objectData[newFocus];
							if (o.style & fuiStyleEnabled)
								o.obj->raiseEvent(fuiEventMouseEnter, o.arrayIndex, 0, 0, 0, 0);
						}
						object &o = objectData[newFocus];
						if (o.style & fuiStyleEnabled)
							o.obj->raiseEvent(fuiEventMouseMove, o.arrayIndex, newButton, newShift, newMouseX, newMouseY);
					} else if (fuiFocus != fuiObjectNull) {
						object &o = objectData[fuiFocus];
						if (o.style & fuiStyleEnabled) {
							getLinkedListObjectAbsolutePosition(fuiFocus, newMouseX, newMouseY);
							o.obj->raiseEvent(fuiEventMouseMove, o.arrayIndex, newButton, newShift, LOWORD(lparam) - newMouseX, HIWORD(lparam) - newMouseY);
						}
					}
					break;
				}
				case WM_PAINT:
				{
					HDC screenhDC = BeginPaint(hwnd, &vPaintStruct);
					RECT &r = vPaintStruct.rcPaint;
					StretchBlt(screenhDC, r.left, r.top, r.right - r.left, r.bottom - r.top, fuihDC, r.left, r.top, r.right - r.left, r.bottom - r.top, SRCCOPY);
					EndPaint(hwnd, &vPaintStruct);
					break;
				}
				case WM_MOUSELEAVE:
				{
					isMouseTracked = false;
					if (fuiLastMove != fuiObjectNull) {
						object &o = objectData[fuiLastMove];
						if (o.style & fuiStyleEnabled)
							o.obj->raiseEvent(fuiEventMouseLeave, o.arrayIndex, wparam, 0, 0, 0);
						fuiLastMove = fuiObjectNull;
					}
					break;
				}
				case WM_MOUSEWHEEL:
				{
					ClientToScreen(hwnd, &vPoint);
					newFocus = getLinkedListIDWithPosition(LOWORD(lparam) - vPoint.x, HIWORD(lparam) - vPoint.y, 0, newMouseX, newMouseY);
					object &o = objectData[newFocus];
					if (o.style & fuiStyleEnabled)
						o.obj->raiseEvent(fuiEventMouseWheel, o.arrayIndex, (wparam >> 16) / WHEEL_DELTA, newMouseX, newMouseY, 0);
					break;
				}
				case WM_LBUTTONDOWN:
				case WM_RBUTTONDOWN:
				case WM_MBUTTONDOWN:
				{
					newFocus = getLinkedListIDWithPosition(LOWORD(lparam), HIWORD(lparam), 0, newMouseX, newMouseY);
					object &o = objectData[newFocus];
					if (!(o.style & fuiStyleEnabled))
						break;
					newButton = (bool(wparam & MK_LBUTTON) * fuiButtonLeft) | (bool(wparam & MK_RBUTTON) * fuiButtonRight) | (bool(wparam & MK_MBUTTON) * fuiButtonMiddle);
					newShift = (bool(wparam & MK_SHIFT) * fuiKeyShift) | (bool(wparam & MK_CONTROL) * fuiKeyCtrl);
					o.obj->raiseEvent(fuiEventMouseDown, o.arrayIndex, newButton, newShift, newMouseX, newMouseY);
					if (newButton == fuiButtonLeft || newButton == fuiButtonMiddle || newButton == fuiButtonRight)
						if (newFocus != fuiFocus) {
							if (fuiFocus != fuiObjectNull) {
								object &p = objectData[fuiFocus];
								if (p.style & fuiStyleEnabled)
									p.obj->raiseEvent(fuiEventLostFocus, o.arrayIndex, 0, 0, 0, 0);
							}
							fuiFocus = newFocus;
							o.obj->raiseEvent(fuiEventGotFocus, o.arrayIndex, 0, 0, 0, 0);
						}
					break;
				}
				case WM_LBUTTONUP:
				case WM_RBUTTONUP:
				case WM_MBUTTONUP:
				{
					if (fuiFocus != fuiObjectNull) {
						object &o = objectData[fuiFocus];
						if (o.style & fuiStyleEnabled) {
							newFocus = getLinkedListIDWithPosition(LOWORD(lparam), HIWORD(lparam), 0, newMouseX, newMouseY);
							newButton = (bool(wparam & MK_LBUTTON) * fuiButtonLeft) | (bool(wparam & MK_RBUTTON) * fuiButtonRight) | (bool(wparam & MK_MBUTTON) * fuiButtonMiddle);
							newShift = (bool(wparam & MK_SHIFT) * fuiKeyShift) | (bool(wparam & MK_CONTROL) * fuiKeyCtrl);
							o.obj->raiseEvent(fuiEventMouseUp, o.arrayIndex, newButton, newShift, newMouseX, newMouseY);
						}
					}
					break;
				}
				case WM_LBUTTONDBLCLK:
				case WM_RBUTTONDBLCLK:
				case WM_MBUTTONDBLCLK:
				{
					if (fuiFocus != fuiObjectNull) {
						object &o = objectData[fuiFocus];
						if (o.style & fuiStyleEnabled) {
							newFocus = getLinkedListIDWithPosition(LOWORD(lparam), HIWORD(lparam), 0, newMouseX, newMouseY);
							newButton = (bool(wparam & MK_LBUTTON) * fuiButtonLeft) | (bool(wparam & MK_RBUTTON) * fuiButtonRight) | (bool(wparam & MK_MBUTTON) * fuiButtonMiddle);
							newShift = (bool(wparam & MK_SHIFT) * fuiKeyShift) | (bool(wparam & MK_CONTROL) * fuiKeyCtrl);
							o.obj->raiseEvent(fuiEventDoubleClick, o.arrayIndex, newButton, newShift, newMouseX, newMouseY);
						}
					}
					break;
				}
				case WM_KEYDOWN:
				{
					if (fuiFocus != fuiObjectNull) {
						object &o = objectData[fuiFocus];
						if (o.style & fuiStyleEnabled)
							o.obj->raiseEvent(fuiEventKeyDown, o.arrayIndex, wparam, 0, 0, 0);
					}
					break;
				}
				case WM_KEYUP:
				{
					if (fuiFocus != fuiObjectNull) {
						object &o = objectData[fuiFocus];
						if (o.style & fuiStyleEnabled)
							o.obj->raiseEvent(fuiEventKeyUp, o.arrayIndex, wparam, 0, 0, 0);
					}
					break;
				}
				case WM_CHAR:
				{
					if (fuiFocus != fuiObjectNull) {
						object &o = objectData[fuiFocus];
						if (o.style & fuiStyleEnabled) {
							if (lastKeyCode == 0) {
								if (wparam < 0x80)
									o.obj->raiseEvent(fuiEventKeyPress, o.arrayIndex, wparam, 0, 0, 0);
								else
									lastKeyCode = wparam;
							} else {
								o.obj->raiseEvent(fuiEventKeyPress, o.arrayIndex, MAKEWORD(byte(wparam), byte(lastKeyCode)), 0, 0, 0);
								lastKeyCode = 0;
							}
						}
					}
					break;
				}
				case WM_KILLFOCUS:
				{
					if (fuiFocus != fuiObjectNull) {
						object &o = objectData[fuiFocus];
						if (o.style & fuiStyleEnabled)
							o.obj->raiseEvent(fuiEventLostFocus, o.arrayIndex, 0, 0, 0, 0);
					}
					break;
				}
				case WM_SETFOCUS:
				{
					if (fuiFocus != fuiObjectNull) {
						object &o = objectData[fuiFocus];
						if (o.style & fuiStyleEnabled)
							o.obj->raiseEvent(fuiEventGotFocus, o.arrayIndex, 0, 0, 0, 0);
					}
					break;
				}
				case WM_SIZE:
				{
					GetWindowRect(hwnd, &vRect);
					object &o = objectData[fuiRootID];
					o.width = vRect.right - vRect.left;
					o.height = vRect.bottom - vRect.top;
					if (o.width > o.bitmapWidth || o.height > o.bitmapHeight) {
						o.bitmapWidth = max(o.width, o.bitmapWidth);
						o.bitmapHeight = max(o.height, o.bitmapHeight);
						gdiRecreateBitmap(o.hDC, o.bitmap, o.bitmapWidth, o.bitmapHeight, o.bitmapData);
					}
					if (o.width > fuiBitmapWidth || o.height > fuiBitmapHeight) {
						fuiBitmapWidth = max(o.width, fuiBitmapWidth);
						fuiBitmapHeight = max(o.height, fuiBitmapHeight);
						gdiRecreateBitmap(fuihDC, fuiBitmap, fuiBitmapWidth, fuiBitmapHeight);
					}
					if (o.style & fuiStyleEnabled)
						o.obj->raiseEvent(fuiEventResize, o.arrayIndex, 0, 0, 0, 0);
					InvalidateRect(hwnd, &vRect, false);
					UpdateWindow(hwnd);
					break;
				}
				case WM_DESTROY:
				{
					fuiTerminate();
					PostQuitMessage(0);
					break;
				}
				default:
					return DefWindowProc(hwnd, msg, wparam, lparam);
			}
			return 0;
		}
		fuiObject *fuiInitialize(HWND hwnd, fuiProcType proc) {
			fuihWindow = hwnd;
			fuiOriginalProc = (WNDPROC)SetWindowLong(hwnd, GWL_WNDPROC, (LONG)(fuiWindowProc));
			fuiLastMove = fuiFocus = fuiObjectNull;
			RECT wRect;
			GetWindowRect(hwnd, &wRect);
			fuiBitmapWidth = wRect.right - wRect.left;
			fuiBitmapHeight = wRect.bottom - wRect.top;
			fuihDC = gdiCreateDC(0);
			fuiBitmap = gdiCreateBitmap(fuihDC, fuiBitmapWidth, fuiBitmapHeight);
			fuiObjectRoot = new fuiObject;
			fuiObjectRoot->initialize(0, 0, 0, wRect.right - wRect.left, wRect.bottom - wRect.top, true, fuiObjectNull, proc);
			hTimerQueue = CreateTimerQueue();
			return fuiObjectRoot;
		}
		void fuiTerminate() {
			gdiDeleteObject(fuihDC, fuiBitmap);
			for (unsigned i = 0; i < objectData.size(); i++) {
				object &o = objectData[i];
				if (!o.isEmpty)
					if (o.hDC != NULL)
						gdiDeleteObject(o.hDC, o.bitmap);
			}
			objectData.clear();
			while (!objectPool.empty())
				objectPool.pop();
		}
		void fuiMove(int id, int left, int top, int width, int height) {
			if (id <= fuiObjectNull || id >= (int)objectData.size())
				return;
			object &o = objectData[id];
			if (width != o.width || height != o.height)
				fuiSize(id, width, height);
			int oX, oY;
			if (left == o.left && top == o.top)
				return;
			oX = o.left;
			oY = o.top;
			o.left = left;
			o.top = top;
		}
		void fuiSize(int id, int width, int height) {
			if (id <= fuiObjectNull || id >= (int)objectData.size())
				return;
			object &o = objectData[id];
			int m_w, m_h;
			if (width < 0)
				width = o.width;
			if (height < 0)
				height = o.height;
			if (width == o.width && height == o.height)
				return;
			m_w = max(width, o.width);
			m_h = max(height, o.height);
			o.width = width;
			o.height = height;
			if (o.hDC != NULL)
				if (o.width > o.bitmapWidth || o.height > o.bitmapHeight) {
					o.bitmapWidth = max(o.bitmapWidth, o.width);
					o.bitmapHeight = max(o.bitmapHeight, o.height);
					gdiRecreateBitmap(o.hDC, o.bitmap, o.bitmapWidth, o.bitmapHeight, o.bitmapData);
				}
			if (o.style & fuiStyleEnabled)
				o.obj->raiseEvent(fuiEventResize, o.arrayIndex, 0, 0, 0, 0);
		}
		void fuiRefresh(int id, int left, int top, int width, int height) {
			if (id <= fuiObjectNull || id >= (int)objectData.size())
				return;
			if (width < 0)
				width = objectData[id].width;
			if (height < 0)
				height = objectData[id].height;
			if (!width || !height || !(objectData[id].style & fuiStyleVisible))
				return;
			do {
				object &o = objectData[id];
				left += o.left;
				top += o.top;
				id = o.parentID;
			} while (id != fuiObjectNull);
			object &o = objectData[fuiRootID];
			if (left < 0)
				width += left, left = 0;
			if (top < 0)
				height += top, top = 0;
			if (left + width > o.width)
				width = o.width - left;
			if (top + height > o.height)
				height = o.height - top;
			StretchBlt(fuihDC, left, top, width, height, o.hDC, left, top, width, height, SRCCOPY);
			id = o.firstChild;
			while (id != fuiObjectNull) {
				refreshLinkedListObject(id, left, top, width, height, 0, 0);
				id = objectData[id].nextID;
			}
			RECT rect;
			rect.left = left;
			rect.top = top;
			rect.right = left + width;
			rect.bottom = top + height;
			InvalidateRect(fuihWindow, &rect, false);
			UpdateWindow(fuihWindow);
		}
		void fuiSetFocus(int id) {
			if (id <= fuiObjectNull || id >= (int)objectData.size())
				return;
			if (id != fuiFocus)
				if (fuiFocus != fuiObjectNull) {
					object &e = objectData[fuiFocus];
					if (e.style & fuiStyleEnabled)
						e.obj->raiseEvent(fuiEventLostFocus, e.arrayIndex, 0, 0, 0, 0);
				}
			fuiFocus = id;
			objectData[id].obj->raiseEvent(fuiEventGotFocus, objectData[id].arrayIndex, 0, 0, 0, 0);
		}
		void fuiZOrder(int id, int zPosition) {
			if (id <= fuiObjectNull || id >= (int)objectData.size())
				return;
			setLinkedListZOrder(id, zPosition);
		}
		void createLinkedListObject(int id, int parentID) {
			object &o = objectData[id], &p = objectData[parentID];
			if (p.lastChild != fuiObjectNull)
				addLinkedListBack(p.lastChild, id);
			else {
				p.lastChild = p.firstChild = id;
				o.parentID = parentID;
				o.lastChild = o.firstChild = fuiObjectNull;
			}
		}
		void addLinkedListFront(int id, int destID) {
			if (id <= fuiObjectNull || id >= (int)objectData.size() || destID < 1 || destID >= (int)objectData.size())
				return;
			object &o = objectData[id], &p = objectData[destID];
			if (o.lastID != fuiObjectNull)
				objectData[o.lastID].nextID = destID;
			else
				objectData[o.parentID].firstChild = destID;
			p.lastID = o.lastID;
			o.lastID = destID;
			p.nextID = id;
			p.parentID = o.parentID;
		}
		void addLinkedListBack(int id, int destID) {
			if (id <= fuiObjectNull || id >= (int)objectData.size() || destID < 1 || destID >= (int)objectData.size())
				return;
			object &o = objectData[id], &p = objectData[destID];
			if (o.nextID != fuiObjectNull)
				objectData[o.nextID].lastID = destID;
			else
				objectData[o.parentID].lastChild = destID;
			p.nextID = o.nextID;
			o.nextID = destID;
			p.lastID = id;
			p.parentID = o.parentID;
		}
		void jointLinkedList(int id) {
			if (id <= fuiObjectNull || id >= (int)objectData.size())
				return;
			object &o = objectData[id];
			if (o.lastID != fuiObjectNull)
				objectData[o.lastID].nextID = o.nextID;
			else
				objectData[o.parentID].firstChild = o.nextID;
			if (o.nextID != fuiObjectNull)
				objectData[o.nextID].lastID = o.lastID;
			else
				objectData[o.parentID].lastChild = o.nextID;
		}
		void deleteLinkedList(int id) {
			if (id <= fuiObjectNull || id >= (int)objectData.size())
				return;
			object &o = objectData[id];
			for (int cur = o.firstChild; cur != fuiObjectNull; cur = objectData[cur].nextID) {
				jointLinkedList(cur);
				addLinkedListBack(id, cur);
				objectData[cur].left += o.left;
				objectData[cur].top += o.top;
			}
			jointLinkedList(id);
		}
		void setLinkedListParent(int id, int parentID) {
			if (id <= fuiObjectNull || id >= (int)objectData.size() || parentID < fuiObjectNull || parentID >= (int)objectData.size())
				return;
			for (int cur = parentID; cur != fuiObjectNull; cur = objectData[cur].parentID)
				if (cur == id)
					return;
			object &o = objectData[id], &p = objectData[parentID];
			jointLinkedList(id);
			o.lastID = o.nextID = fuiObjectNull;
			if (p.lastChild != fuiObjectNull)
				addLinkedListBack(p.lastChild, id);
			else {
				p.firstChild = p.lastChild = id;
				o.lastID = o.nextID = fuiObjectNull;
				o.parentID = parentID;
			}
		}
		void setLinkedListZOrder(int id, int zOrder) {
			if (id <= fuiObjectNull || id >= (int)objectData.size())
				return;
			object &o = objectData[id];
			int destID = getLinkedListCount(o.parentID);
			int id_zorder = getLinkedListZOrder(id);
			if (zOrder == 0)
				zOrder = 1;
			if (zOrder > destID || zOrder < 0)
				zOrder = destID;
			destID = getLinkedListObjectID(zOrder, o.parentID);
			if (id == destID)
				return;
			jointLinkedList(id);
			if (id_zorder > zOrder)
				addLinkedListFront(destID, id);
			else
				addLinkedListBack(destID, id);
		}
		int getLinkedListZOrder(int id) {
			if (id <= fuiObjectNull || id >= (int)objectData.size())
				return 0;
			int ret = 1;
			for (int cur = objectData[objectData[id].parentID].firstChild; cur != id; cur = objectData[cur].nextID)
				ret++;
			return ret;
		}
		int getLinkedListObjectID(int zOrder, int parentID) {
			int ret = objectData[parentID].firstChild;
			for (int cur = 2; cur <= zOrder; cur++)
				ret = objectData[ret].nextID;
			return ret;
		}
		int getLinkedListCount(int parentID) {
			int ret = 0;
			parentID = objectData[parentID].firstChild;
			while (parentID != fuiObjectNull) {
				ret++;
				parentID = objectData[parentID].nextID;
			}
			return ret;
		}
		void getLinkedListObjectAbsolutePosition(int id, int &left, int &top) {
			left = top = 0;
			while (id != fuiObjectNull) {
				object &o = objectData[id];
				left += o.left;
				top += o.top;
				id = o.parentID;
			}
		}
		int getLinkedListIDWithPosition(int x, int y, int parentID, int &xPosition, int &yPosition) {
			bool isInObject = false;
			byte tAlpha = 0;
			object &p = objectData[parentID];
			x -= p.left;
			y -= p.top;
			int ret = p.lastChild;
			while (ret != fuiObjectNull) {
				object &o = objectData[ret];
				if ((o.style & fuiStyleVisible) && (o.style & fuiStyleClicked)) {
					if (x >= o.left && y >= o.top && x <= o.left + o.width && y <= o.top + o.height) {
						isInObject = false;
						if (o.style & fuiStyleHitInTrans)
							isInObject = true;
						else {
							if (o.hDC != NULL && o.alpha > 0)
								if (x <= o.left + o.bitmapWidth && y <= o.top + o.bitmapHeight) {
									if (o.style & fuiStyleLayered) {
										memcpy(&tAlpha, (void *)(int(o.bitmapData) + (o.bitmapHeight - 1 - y + o.top) * o.bitmapWidth * 4 + (x - o.left) * 4 + 3), 1);
										if (tAlpha > 0)
											isInObject = true;
									} else if (o.style & fuiStyleTransparent) {
										if (GetPixel(o.hDC, x - o.left, y - o.top) != o.transparentColor)
											isInObject = true;
									} else
										isInObject = true;
								}
						}
						if (isInObject) {
							if (o.lastChild != fuiObjectNull) {
								if (o.style & fuiStyleEnabled)
									ret = getLinkedListIDWithPosition(x, y, ret, xPosition, yPosition);
							} else {
								xPosition = x - o.left;
								yPosition = y - o.top;
							}
							return ret;
						}
					}
				}
				ret = o.lastID;
			}
			ret = parentID;
			xPosition = x;
			yPosition = y;
			return ret;
		}
		void refreshLinkedListObject(int id, int left, int top, int width, int height, int parent_left, int parent_top) {
			int i, tX, tY, tWidth, tHeight;
			object &o = objectData[id];
			tX = parent_left + o.left;
			tY = parent_top + o.top;
			if (o.style & fuiStyleVisible) {
				if (left < tX + o.width && left + width > tX && top < tY + o.height && top + height > tY) {
					if (tX < left)
						tX = left;
					if (tY < top)
						tY = top;
					if (parent_left + o.left + o.width > left + width)
						tWidth = left + width - tX;
					else
						tWidth = parent_left + o.left + o.width - tX;
					if (parent_top + o.top + o.height > top + height)
						tHeight = top + height - tY;
					else
						tHeight = parent_top + o.top + o.height - tY;
					if (o.hDC != NULL && o.alpha > 0) {
						if (o.style & fuiStyleLayered) {
							BLENDFUNCTION bf;
							bf.BlendOp = AC_SRC_OVER;
							bf.BlendFlags = 0;
							bf.SourceConstantAlpha = o.alpha;
							bf.AlphaFormat = AC_SRC_ALPHA;
							AlphaBlend(fuihDC, tX, tY, tWidth, tHeight, o.hDC, tX - o.left - parent_left, tY - o.top - parent_top, tWidth, tHeight, bf);
						} else if (o.style & fuiStyleTransparent)
							TransparentBlt(fuihDC, tX, tY, tWidth, tHeight, o.hDC, tX - o.left - parent_left, tY - o.top - parent_top, tWidth, tHeight, o.transparentColor);
						else if (o.alpha < 0xff) {
							BLENDFUNCTION bf;
							bf.BlendOp = AC_SRC_OVER;
							bf.BlendFlags = 0;
							bf.SourceConstantAlpha = o.alpha;
							bf.AlphaFormat = 0;
							AlphaBlend(fuihDC, tX, tY, tWidth, tHeight, o.hDC, tX - o.left - parent_left, tY - o.top - parent_top, tWidth, tHeight, bf);
						} else
							StretchBlt(fuihDC, tX, tY, tWidth, tHeight, o.hDC, tX - o.left - parent_left, tY - o.top - parent_top, tWidth, tHeight, SRCCOPY);
					}
					for (i = o.firstChild; i != fuiObjectNull; i = objectData[i].nextID)
						refreshLinkedListObject(i, tX, tY, tWidth, tHeight, parent_left + o.left, parent_top + o.top);
				}
			}
		}
	};
};

#endif

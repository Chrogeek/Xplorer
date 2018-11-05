#include "fuiGDI.h"
#include "fuiSystem.h"
using namespace std;
namespace fui {
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
	LRESULT CALLBACK fuiWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
		static TRACKMOUSEEVENT tMouseTrack;
		static PAINTSTRUCT vPaintStruct;
		static RECT vRect;
		static POINT vPoint;
		static bool isMouseTracked;
		static int lastKeyCode;
		static int shiftMask;
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
				newShift = (bool(wparam & MK_SHIFT) * fuiKeyShift) | (bool(wparam & MK_CONTROL) * fuiKeyCtrl) | (bool(wparam & MK_ALT) * fuiKeyAlt);
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
				newShift = (bool(wparam & MK_SHIFT) * fuiKeyShift) | (bool(wparam & MK_CONTROL) * fuiKeyCtrl) | (bool(wparam & MK_ALT) * fuiKeyAlt);
				object &o = objectData[newFocus];
				if (o.style & fuiStyleEnabled)
					o.obj->raiseEvent(fuiEventMouseWheel, o.arrayIndex, (wparam >> 16) / WHEEL_DELTA, newShift, newMouseX, newMouseY);
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
				newShift = (bool(wparam & MK_SHIFT) * fuiKeyShift) | (bool(wparam & MK_CONTROL) * fuiKeyCtrl) | (bool(wparam & MK_ALT) * fuiKeyAlt);
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
						newShift = (bool(wparam & MK_SHIFT) * fuiKeyShift) | (bool(wparam & MK_CONTROL) * fuiKeyCtrl) | (bool(wparam & MK_ALT) * fuiKeyAlt);
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
						newShift = (bool(wparam & MK_SHIFT) * fuiKeyShift) | (bool(wparam & MK_CONTROL) * fuiKeyCtrl) | (bool(wparam & MK_ALT) * fuiKeyAlt);
						o.obj->raiseEvent(fuiEventDoubleClick, o.arrayIndex, newButton, newShift, newMouseX, newMouseY);
					}
				}
				break;
			}
			case WM_KEYDOWN:
			{
				if (wparam == VK_CONTROL) shiftMask |= fuiKeyCtrl;
				if (wparam == VK_SHIFT) shiftMask |= fuiKeyShift;
				if (wparam == VK_MENU) shiftMask |= fuiKeyAlt;
				if (fuiFocus != fuiObjectNull) {
					object &o = objectData[fuiFocus];
					if (o.style & fuiStyleEnabled)
						o.obj->raiseEvent(fuiEventKeyDown, o.arrayIndex, wparam, shiftMask, 0, 0);
				}
				break;
			}
			case WM_KEYUP:
			{
				if (wparam == VK_CONTROL) shiftMask &= ~fuiKeyCtrl;
				if (wparam == VK_SHIFT) shiftMask &= ~fuiKeyShift;
				if (wparam == VK_MENU) shiftMask &= ~fuiKeyAlt;
				if (fuiFocus != fuiObjectNull) {
					object &o = objectData[fuiFocus];
					if (o.style & fuiStyleEnabled)
						o.obj->raiseEvent(fuiEventKeyUp, o.arrayIndex, wparam, shiftMask, 0, 0);
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
		if (parentID == fuiObjectNull) {
			object &o = objectData[id];
			o.parentID = fuiObjectNull;
			o.lastChild = o.firstChild = fuiObjectNull;
		} else {
			object &o = objectData[id], &p = objectData[parentID];
			if (p.lastChild != fuiObjectNull)
				addLinkedListBack(p.lastChild, id);
			else {
				p.lastChild = p.firstChild = id;
				o.parentID = parentID;
				o.lastChild = o.firstChild = fuiObjectNull;
			}
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
	int fuiObject::id() {
		return _id;
	}
	HDC fuiObject::hDC() {
		return fuiGethDC(_id);
	}
	int fuiObject::left() {
		return fuiGetLeft(_id);
	}
	void fuiObject::left(int newValue) {
		fuiSetLeft(_id, newValue);
	}
	int fuiObject::top() {
		return fuiGetTop(_id);
	}
	void fuiObject::top(int newValue) {
		fuiSetTop(_id, newValue);
	}
	int fuiObject::width() {
		return fuiGetWidth(_id);
	}
	void fuiObject::width(int newValue) {
		fuiSetWidth(_id, newValue);
	}
	int fuiObject::height() {
		return fuiGetHeight(_id);
	}
	void fuiObject::height(int newValue) {
		fuiSetHeight(_id, newValue);
	}
	int fuiObject::transparentColor() {
		return fuiGetTransparentColor(_id);
	}
	void fuiObject::transparentColor(int newValue) {
		fuiSetTransparentColor(_id, newValue);
	}
	bool fuiObject::clicked() {
		return fuiGetClicked(_id);
	}
	void fuiObject::clicked(bool newValue) {
		fuiSetClicked(_id, newValue);
	}
	bool fuiObject::enabled() {
		return fuiGetEnabled(_id);
	}
	void fuiObject::enabled(bool newValue) {
		fuiSetEnabled(_id, newValue);
	}
	bool fuiObject::visible() {
		return fuiGetVisible(_id);
	}
	void fuiObject::visible(bool newValue) {
		fuiSetVisible(_id, newValue);
	}
	bool fuiObject::layered() {
		return fuiGetLayered(_id);
	}
	void fuiObject::layered(bool newValue) {
		fuiSetLayered(_id, newValue);
	}
	bool fuiObject::transparent() {
		return fuiGetTransparent(_id);
	}
	void fuiObject::transparent(bool newValue) {
		fuiSetTransparent(_id, newValue);
	}
	bool fuiObject::hitInTrans() {
		return fuiGetHitInTrans(_id);
	}
	void fuiObject::hitInTrans(bool newValue) {
		fuiSetHitInTrans(_id, newValue);
	}
	byte fuiObject::alpha() {
		return fuiGetAlpha(_id);
	}
	void fuiObject::alpha(byte newValue) {
		fuiSetAlpha(_id, newValue);
	}
	fuiProcType fuiObject::proc() {
		return _proc;
	}
	void fuiObject::proc(fuiProcType newValue) {
		_proc = newValue;
	}
	void fuiObject::move(int left, int top, int width, int height) {
		fuiMove(_id, left, top, width, height);
	}
	void fuiObject::size(int width, int height) {
		fuiSize(_id, width, height);
	}
	void fuiObject::refresh(int left, int top, int width, int height) {
		fuiRefresh(_id, left, top, width, height);
	}
	void fuiObject::setFocus() {
		fuiSetFocus(_id);
	}
	void fuiObject::zOrder(int zPosition) {
		fuiZOrder(_id, zPosition);
	}
	void fuiObject::raiseEvent(fuiEventType eventType, int arrayIndex, int arg1, int arg2, int arg3, int arg4) {
		if (_proc != NULL)
			_proc(eventType, arrayIndex, arg1, arg2, arg3, arg4);
	}
	void fuiObject::initialize(int arrayIndex, int left, int top, int width, int height, bool withBitmap, int parentID, fuiProcType proc) {
		_proc = proc;
		_id = fuiCreateObject(arrayIndex, *this, left, top, width, height, withBitmap, parentID);
	}
	fuiObject::~fuiObject() {
		fuiDeleteObject(_id);
	}
	int fuiCreateObject(int arrayIndex, fuiObject &obj, int left, int top, int width, int height, bool withBitmap, int parentID) {
		if (objectData.size() - objectPool.size() >= fuiObjectCapacity)
			return fuiObjectNull;
		int ret;
		if (objectPool.size() > 0) {
			ret = objectPool.front();
			objectPool.pop();
		} else {
			objectData.push_back(object());
			ret = objectData.size() - 1;
		}
		object &o = objectData[ret];
		o.isEmpty = false;
		o.arrayIndex = arrayIndex;
		o.obj = &obj;
		o.left = left;
		o.top = top;
		o.width = width;
		o.height = height;
		o.style = fuiStyleDefault;
		o.alpha = 0xff;
		o.parentID = parentID;
		o.nextID = o.lastID = o.firstChild = o.lastChild = fuiObjectNull;
		if (withBitmap) {
			o.bitmapWidth = width;
			o.bitmapHeight = height;
			o.hDC = gdiCreateDC(NULL);
			o.bitmap = gdiCreateBitmap(o.hDC, o.bitmapWidth, o.bitmapHeight);
		}
		createLinkedListObject(ret, parentID);
		o.obj->raiseEvent(fuiEventInitialize, o.arrayIndex, 0, 0, 0, 0);
		return ret;
	}
	void fuiDeleteObject(int id) {
		if (id <= fuiObjectNull || id >= (int)objectData.size() || id == fuiRootID)
			return;
		object &o = objectData[id];
		if (o.isEmpty == true)
			return;
		o.obj->raiseEvent(fuiEventTerminate, o.arrayIndex, 0, 0, 0, 0);
		if (fuiFocus == id)
			fuiFocus = fuiObjectNull;
		if (fuiLastMove == id)
			fuiLastMove = fuiObjectNull;
		objectPool.push(id);
		if (o.hDC != NULL) {
			gdiDeleteObject(o.hDC, o.bitmap);
			if (o.alpha > 0) {
				o.hDC = NULL;
			}
		}
		deleteLinkedList(id);
		o.isEmpty = true;
	}
	HDC fuiGethDC(int id) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return NULL;
		return objectData[id].hDC;
	}
	int fuiGetLeft(int id) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return 0;
		return objectData[id].left;
	}
	void fuiSetLeft(int id, int newValue) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return;
		fuiMove(id, newValue, objectData[id].top, -1, -1);
	}
	int fuiGetTop(int id) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return 0;
		return objectData[id].top;
	}
	void fuiSetTop(int id, int newValue) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return;
		fuiMove(id, objectData[id].left, newValue, -1, -1);
	}
	int fuiGetWidth(int id) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return 0;
		return objectData[id].width;
	}
	void fuiSetWidth(int id, int newValue) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return;
		fuiSize(id, newValue, objectData[id].height);
	}
	int fuiGetHeight(int id) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return 0;
		return objectData[id].height;
	}
	void fuiSetHeight(int id, int newValue) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return;
		fuiSize(id, objectData[id].width, newValue);
	}
	int fuiGetTransparentColor(int id) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return 0;
		return objectData[id].transparentColor;
	}
	void fuiSetTransparentColor(int id, int newValue) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return;
		object &o = objectData[id];
		if ((o.style & fuiStyleLayered) == 0 && (o.alpha == 0xff || o.alpha == 0))
			if (o.transparentColor != newValue && (o.style & fuiStyleTransparent)) {
				o.transparentColor = newValue;
			}
		o.transparentColor = newValue;
	}
	bool fuiGetClicked(int id) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return NULL;
		return bool(objectData[id].style & fuiStyleClicked);
	}
	void fuiSetClicked(int id, bool newValue) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return;
		object &o = objectData[id];
		if (newValue)
			o.style |= fuiStyleClicked;
		else
			o.style &= ~fuiStyleClicked;
	}
	bool fuiGetEnabled(int id) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return NULL;
		return bool(objectData[id].style & fuiStyleEnabled);
	}
	void fuiSetEnabled(int id, bool newValue) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return;
		object &o = objectData[id];
		if (newValue)
			o.style |= fuiStyleEnabled;
		else
			o.style &= ~fuiStyleEnabled;
	}
	bool fuiGetVisible(int id) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return NULL;
		return bool(objectData[id].style & fuiStyleVisible);
	}
	void fuiSetVisible(int id, bool newValue) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return;
		object &o = objectData[id];
		if (bool(o.style & fuiStyleVisible) != newValue) {
			if (newValue)
				o.style |= fuiStyleVisible;
			else
				o.style &= ~fuiStyleVisible;
			int t_l, t_t;
			getLinkedListObjectAbsolutePosition(id, t_l, t_t);
		}
	}
	bool fuiGetLayered(int id) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return NULL;
		return bool(objectData[id].style & fuiStyleLayered);
	}
	void fuiSetLayered(int id, bool newValue) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return;
		object &o = objectData[id];
		if ((o.style & fuiStyleTransparent) == 0)
			if (bool(o.style & fuiStyleLayered) != newValue) {
				if (newValue)
					o.style |= fuiStyleLayered;
				else
					o.style &= ~fuiStyleLayered;
			}
	}
	bool fuiGetTransparent(int id) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return NULL;
		return bool(objectData[id].style & fuiStyleTransparent);
	}
	void fuiSetTransparent(int id, bool newValue) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return;
		object &o = objectData[id];
		if ((o.style & fuiStyleLayered) == 0 && (o.alpha == 0xff || o.alpha == 0)) {
			if (bool(o.style & fuiStyleTransparent) != newValue) {
				if (newValue)
					o.style |= fuiStyleTransparent;
				else
					o.style &= ~fuiStyleTransparent;
			}
		}
	}
	bool fuiGetHitInTrans(int id) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return NULL;
		return bool(objectData[id].style & fuiStyleHitInTrans);
	}
	void fuiSetHitInTrans(int id, bool newValue) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return;
		object &o = objectData[id];
		if (newValue)
			o.style |= fuiStyleHitInTrans;
		else
			o.style &= ~fuiStyleHitInTrans;
	}
	byte fuiGetAlpha(int id) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return 0;
		return objectData[id].alpha;
	}
	void fuiSetAlpha(int id, byte newValue) {
		if (id <= fuiObjectNull || id >= (int)objectData.size())
			return;
		object &o = objectData[id];
		if ((o.style & fuiStyleTransparent) != 0 && newValue != 0xff && newValue != 0)
			return;
		if (o.alpha != newValue) {
			o.alpha = newValue;
			fuiRefresh(id, 0, 0, -1, -1);
		}
	}
};

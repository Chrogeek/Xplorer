#ifndef FUIOBJECT_H
#define FUIOBJECT_H

#include "fuiGDI.h"
#include "fuiSystem.h"
namespace fui {
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
	void fuiObject::setfocus() {
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
	namespace {
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
	}
};

#endif

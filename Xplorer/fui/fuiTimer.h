#ifndef FUITIMER_H
#define FUITIMER_H

#include <windows.h>
#include "fuiSystem.h"
namespace fui {
	int fuiTimer::id() {
		return _id;
	}
	void fuiTimer::interval(int newValue) {
		fuiSetTimerInterval(_id, newValue);
	}
	int fuiTimer::interval() {
		return fuiGetTimerInterval(_id);
	}
	void fuiTimer::proc(fuiProcType newValue) {
		_proc = newValue;
	}
	fuiProcType fuiTimer::proc() {
		return _proc;
	}
	void fuiTimer::enabled(bool newValue) {
		fuiSetTimerEnabled(_id, newValue);
	}
	bool fuiTimer::enabled() {
		return fuiGetTimerEnabled(_id);
	}
	void fuiTimer::initialize(int arrayIndex, int interval, fuiProcType proc) {
		_proc = proc;
		_id = fuiCreateTimer(arrayIndex, *this);
		this->interval(interval);
	}
	void fuiTimer::raiseEvent(fuiEventType eventType, int arrayIndex, int arg1, int arg2, int arg3, int arg4) {
		if (_proc != NULL)
			_proc(eventType, arrayIndex, arg1, arg2, arg3, arg4);
	}
	fuiTimer::~fuiTimer() {
		fuiDeleteTimer(_id);
	}
	namespace {
		VOID CALLBACK fuiTimerProc(PVOID lparam, BOOLEAN TimerOrWaitFired) {
			if (lparam != NULL) {
				timer &t = timerData[*(int*)lparam];
				t.obj->raiseEvent(fuiEventTimer, t.arrayIndex, 0, 0, 0, 0);
			}
		}
		int fuiGetTimerInterval(int id) {
			if (id <= fuiObjectNull || id >= (int)timerData.size())
				return 0;
			return timerData[id].interval;
		}
		void fuiSetTimerInterval(int &id, int newValue) {
			if (id <= fuiObjectNull || id >= (int)timerData.size())
				return;
			timer &t = timerData[id];
			t.interval = newValue;
			DeleteTimerQueueTimer(hTimerQueue, t.hTimer, NULL);
			if (newValue > 0)
				CreateTimerQueueTimer(&t.hTimer, hTimerQueue, (WAITORTIMERCALLBACK)fuiTimerProc, &id, 0, t.interval, WT_EXECUTEDEFAULT);
		}
		bool fuiGetTimerEnabled(int id) {
			if (id <= fuiObjectNull || id >= (int)timerData.size())
				return false;
			return timerData[id].style & fuiStyleEnabled;
		}
		void fuiSetTimerEnabled(int &id, bool newValue) {
			if (id <= fuiObjectNull || id >= (int)timerData.size())
				return;
			timer &t = timerData[id];
			if (newValue)
				t.style |= fuiStyleEnabled;
			else
				t.style &= ~fuiStyleEnabled;
			DeleteTimerQueueTimer(hTimerQueue, t.hTimer, NULL);
			if (newValue && t.interval > 0)
				CreateTimerQueueTimer(&t.hTimer, hTimerQueue, (WAITORTIMERCALLBACK)fuiTimerProc, &id, 0, t.interval, WT_EXECUTEDEFAULT);
		}
		int fuiCreateTimer(int arrayIndex, fuiTimer &tmr) {
			if (timerData.size() - timerPool.size() >= fuiObjectCapacity)
				return fuiObjectNull;
			int ret;
			if (timerPool.size() > 0)
				ret = timerPool.front(), timerPool.pop();
			else
				timerData.push_back(timer()), ret = timerData.size() - 1;
			timer &t = timerData[ret];
			t.isEmpty = false;
			t.arrayIndex = arrayIndex;
			t.obj = &tmr;
			t.interval = 0;
			t.style = fuiStyleDefault;
			return ret;
		}
		void fuiDeleteTimer(int id) {
			if (id <= fuiObjectNull || id >= (int)timerData.size())
				return;
			timer &t = timerData[id];
			if (t.isEmpty == true)
				return;
			timerPool.push(id);
			t.isEmpty = true;
		}
	}
};

#endif

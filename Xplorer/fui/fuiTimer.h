#ifndef FUITIMER_H
#define FUITIMER_H

#include <windows.h>
#include "fuiSystem.h"
namespace fui {
	VOID CALLBACK fuiTimerProc(PVOID, BOOLEAN);
	int fuiGetTimerInterval(int);
	void fuiSetTimerInterval(int &, int);
	bool fuiGetTimerEnabled(int);
	void fuiSetTimerEnabled(int &, bool);
	int fuiCreateTimer(int, fuiTimer &);
	void fuiDeleteTimer(int);
};

#endif

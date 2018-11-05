#ifndef FUIDEFS_H
#define FUIDEFS_H

const int fuiBuildDate = MAKELONG(2018, MAKEWORD(11, 4));
const int fuiVersion = MAKELONG(MAKEWORD(1, 1), MAKEWORD(0, 1));
enum fuiEventType {
	fuiEventInitialize = 1,	// event args: 0
	fuiEventTerminate,		// event args: 0
	fuiEventMouseDown,		// event args: 4, mouse button, shift/ctrl/alt keys, mouse x, mouse y
	fuiEventMouseMove,		// event args: 4, mouse button, shift/ctrl/alt keys, mouse x, mouse y
	fuiEventMouseUp,		// event args: 4, mouse button, shift/ctrl/alt keys, mouse x, mouse y
	fuiEventMouseWheel,		// event args: 4, mouse wheel count, shift/ctrl/alt keys, mouse x, mouse y
	fuiEventMouseEnter,		// event args: 0
	fuiEventMouseLeave,		// event args: 0
	fuiEventDoubleClick,	// event args: 4, mouse button, shift/ctrl/alt keys, mouse x, mouse y
	fuiEventKeyDown,		// event args: 2, key code, shift/ctrl/alt keys
	fuiEventKeyPress,		// event args: 1, ascii code
	fuiEventKeyUp,			// event args: 2, key code, shift/ctrl/alt keys
	fuiEventGotFocus,		// event args: 0
	fuiEventLostFocus,		// event args: 0
	fuiEventResize,			// event args: 0
	fuiEventTimer			// event args: 0
};
typedef void(*fuiProcType)(fuiEventType, int, int, int, int, int);
typedef unsigned char byte;
const int fuiObjectCapacity = 0x3fff;
const byte fuiStyleClicked = 1 << 0;
const byte fuiStyleEnabled = 1 << 1;
const byte fuiStyleVisible = 1 << 2;
const byte fuiStyleLayered = 1 << 3;
const byte fuiStyleTransparent = 1 << 4;
const byte fuiStyleHitInTrans = 1 << 5;
const byte fuiStyleDefault = fuiStyleClicked | fuiStyleEnabled | fuiStyleVisible | fuiStyleHitInTrans;
const int fuiObjectNull = -1;
const int fuiRootID = 0;
const int fuiButtonLeft = 1;
const int fuiButtonRight = 2;
const int fuiButtonMiddle = 4;
const int fuiKeyShift = 1;
const int fuiKeyCtrl = 2;
const int fuiKeyAlt = 4;

#endif

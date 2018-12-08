#include "defs.h"
#include <d2d1.h>
#include "utility.h"
#include "gameFrame.h"

gameFrame::gameFrame() {
	brush = nullptr;
	bitmap = nullptr;
	renderer = nullptr;
	render = exit = enter = nullptr;
}

gameFrame::~gameFrame() {
	safeRelease(brush);
	safeRelease(bitmap);
	safeRelease(renderer);
}

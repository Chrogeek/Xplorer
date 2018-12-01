#ifndef XPLORER_GAMELEVEL_H
#define XPLORER_GAMELEVEL_H

#include "defs.h"
#include <string>
#include <vector>
#include <d2d1.h>
#include <windows.h>
#include "json.h"

using json = nlohmann::json;

struct gameLevel {
	// Object of a single level
	int rows, columns;
	std::vector<std::vector<int> > grid;
	ID2D1BitmapRenderTarget *frame;
	HDC hDC;
	XplorerResult load(std::string);
};

struct gameChapter {
	// Chapter object (manages levels)
	std::string chapterName;
	std::vector<gameLevel> levels;
	XplorerResult load(std::string);
};

struct gameManager {
	// Root object of the game (manages chapters)
	std::vector<gameChapter> chapters;
	XplorerResult load(std::string);
};

#endif

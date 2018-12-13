#ifndef XPLORER_GAMELEVEL_H
#define XPLORER_GAMELEVEL_H

#include "defs.h"
#include <string>
#include <vector>
#include <array>
#include <d2d1.h>
#include <windows.h>
#include "json.h"
#include "utility.h"
#include "geometry.h"
#include "gameFrame.h"

using json = nlohmann::json;

std::vector<int> getNumericInDirectory(std::string);

struct gameLevel {
	// Object of a single level
	int rows, columns;
	std::vector<std::vector<int> > grid;
	std::vector<std::vector<DWORD> > touchTime;
	//ID2D1BitmapRenderTarget *frame;
	ID2D1Bitmap *objects;
	gameFrame *frame;
	pointVector initialPosition;
	json saveData;
	int id;
	//std::string objectFile;
	gameResult load(std::string);
	gameResult save(std::string);
	gameLevel();
	~gameLevel();
};

struct gameChapter {
	// Chapter object (manages levels)
	std::string chapterName;
	std::vector<gameLevel> levels;
	json saveData;
	int id;
	gameResult load(std::string);
	gameResult save(std::string);
};

struct gameManager {
	// Root object of the game (manages chapters)
	std::vector<gameChapter> chapters;
	json saveData;
	gameResult load(std::string);
	gameResult save(std::string);
};

gameLevel &theLevel();
gameChapter &theChapter();

void deleteSave();

#endif

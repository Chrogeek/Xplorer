#include "defs.h"
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <direct.h>
#include <io.h>
#include <d2d1.h>
#include <dwrite.h>
#include "json.h"
#include "gameLevel.h"
#include "utility.h"
#include "music.h"

extern float dpiX, dpiY;

extern ID2D1Factory *d2dFactory;
extern IWICImagingFactory *imageFactory;
extern IDWriteFactory *writeFactory;
extern IDWriteTextFormat *textFormatNormal;

extern ID2D1DCRenderTarget *mainRenderer;
extern ID2D1SolidColorBrush *brushBlack;

extern gameManager gameMaster;
extern int currentChapter, currentLevel;

using json = nlohmann::json;

std::vector<int> getNumericInDirectory(std::string dir) {
	std::vector<int> files;
	_finddata_t file;
	intptr_t lf;
	if ((lf = _findfirst((dir + "/*").c_str(), &file)) != -1) {
		while (_findnext(lf, &file) == 0) {
			if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0) continue;
			if (!(file.attrib | FILE_ATTRIBUTE_DIRECTORY)) continue;
			int num = getNumberFromString(std::string(file.name));
			if (num != -1) files.push_back(num);
		}
	}
	_findclose(lf);
	std::sort(files.begin(), files.end());
	return files;
}

gameResult gameLevel::load(std::string folder) {
	// Step 1: read the data

	json data, metaData;

	if (loadJSONFromFile(folder + "/data.json", data) != okay) return fileBroken;

	if (loadJSONFromFile(folder + "/tiles.json", metaData) != okay) return fileBroken;

	loadJSONFromFile(folder + "/config.json", saveData);

	if (!saveData.count(itemLastX)) saveData[itemLastX] = -1.0;
	if (!saveData.count(itemLastY)) saveData[itemLastY] = -1.0;

	// Step 2: make the grid

	rows = data["height"];
	columns = data["width"];
	grid.resize(columns);
	touchTime.resize(columns);
	for (int i = 0; i < columns; ++i) grid[i].resize(rows), touchTime[i].resize(rows);
	for (int j = 0, k = 0; j < rows; ++j) for (int i = 0; i < columns; ++i, ++k)  {
		grid[i][j] = data["layers"][1]["data"][k] - 1;
	}

	D2D1_BITMAP_PROPERTIES properties;
	properties.dpiX = dpiX;
	properties.dpiY = dpiY;
	properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;

	// Step 3: render the map

	HRESULT result = S_OK;

	safeNew(frame, gameFrame);

	if (SUCCEEDED(result)) {
		result = mainRenderer->CreateCompatibleRenderTarget(makeSizeF((float)columns * unitSize, (float)rows * unitSize), &frame->renderer);
	}
	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(mainRenderer, imageFactory, stringToWidestring(metaData["image"]).c_str(), metaData["imagewidth"], metaData["imageheight"], &objects);
	}

	frame->renderer->BeginDraw();

	ID2D1Bitmap *background = nullptr;

	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(frame->renderer, imageFactory, stringToWidestring(data["layers"][0]["image"]).c_str(), columns * unitSize, rows * unitSize, &background);
	}
	if (SUCCEEDED(result)) {
		frame->renderer->DrawBitmap(background, makeRectF(0.f, 0.f, (float)(columns * unitSize), (float)(rows * unitSize)));
	}

	safeRelease(background);

	if (SUCCEEDED(result)) {
		DWORD timeCurrent = timeGetTime();
		for (int i = 0; i < columns; ++i) for (int j = 0; j < rows; ++j) {
			if (grid[i][j] == blockEmpty) continue;
			else if (grid[i][j] == blockStartingPoint) {
				initialPosition.vX = (double)i * unitSize;
				initialPosition.vY = (double)(j + 1) * unitSize - heroSize;
				continue;
			} else if (grid[i][j] == blockFragile) {
				continue; // Fragile tiles are not prerendered
			}
			frame->renderer->DrawBitmap(objects, makeRectF((float)i * unitSize, (float)j * unitSize, (float)(i + 1) * unitSize, (float)(j + 1) * unitSize), 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, makeRectF((float)grid[i][j] * unitSize, 0.f, (float)(grid[i][j] + 1) * unitSize, (float)unitSize));
		}
	}

	// Step 4: render text

	if (SUCCEEDED(result)) {
		for (auto textObject : data["layers"][2]["objects"]) {
			std::wstring text = stringToWidestring(textObject["text"]["text"]);
			bool isBold = false, isItalic = false;
			if (textObject["text"]["bold"] == true) isBold = true;
			if (textObject["text"]["italic"] == true) isItalic = true;
			float fontSize = textObject["text"].count("pixelsize") ? (float)textObject["text"]["pixelsize"] : 16.f;

			std::string hAlign = toLower(textObject["text"].count("halign") ? textObject["text"]["halign"] : "left");
			std::string vAlign = toLower(textObject["text"].count("valign") ? textObject["text"]["valign"] : "top");
			DWRITE_TEXT_ALIGNMENT hA;
			DWRITE_PARAGRAPH_ALIGNMENT vA;

			if (hAlign == "left") hA = DWRITE_TEXT_ALIGNMENT_LEADING;
			else if (hAlign == "right") hA = DWRITE_TEXT_ALIGNMENT_TRAILING;
			else if (hAlign == "center") hA = DWRITE_TEXT_ALIGNMENT_CENTER;
			else hA = DWRITE_TEXT_ALIGNMENT_JUSTIFIED;

			if (vAlign == "top") vA = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
			else if (vAlign == "center") vA = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
			else vA = DWRITE_PARAGRAPH_ALIGNMENT_FAR;
			
			DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_REGULAR;
			std::string fontFamily;
			
			getFontFamilyWeight(textObject["text"]["fontfamily"], fontFamily, weight);

			result = drawText(frame->renderer, writeFactory, stringToWidestring(fontFamily),
				weight, isItalic, fontSize, text, makeRectF((float)textObject["x"], (float)textObject["y"],
				(float)textObject["x"] + (float)textObject["width"], (float)textObject["y"] + (float)textObject["height"]), hA, vA, brushBlack);
		}
	}

	frame->renderer->EndDraw();

	// Step 5: finish
	if (SUCCEEDED(result)) {
		result = frame->renderer->GetBitmap(&frame->bitmap);
	}
	if (SUCCEEDED(result)) {
		result = frame->renderer->CreateBitmapBrush(frame->bitmap, &frame->brush);
	}
	if (result != S_OK) return direct2DError;
	return okay;
}

gameResult gameLevel::save(std::string folder) {
	std::ofstream fout(folder + "/config.json");
	if (!fout.is_open()) return fileBroken;
	fout << std::setw(4) << saveData << std::endl;
	fout.close();
	return okay;
}

gameLevel::gameLevel() {
	frame = nullptr;
	objects = nullptr;
	grid.clear();
}

gameLevel::~gameLevel() {
	safeRelease(objects);
	delete frame;
}

gameResult gameChapter::load(std::string folder) {
	loadJSONFromFile(folder + "/config.json", saveData);
	//if (!saveData.count(itemTitle)) saveData[itemTitle] = "";
	if (!saveData.count(itemTime)) saveData[itemTime] = 0.0;
	if (!saveData.count(itemDeaths)) saveData[itemDeaths] = 0;
	if (!saveData.count(itemLastSaved)) saveData[itemLastSaved] = -1;
	if (!saveData.count(itemDistance)) saveData[itemDistance] = 0.0;
	if (!saveData.count(itemTitle)) saveData[itemTitle] = "";
	if (!saveData.count(itemJumps)) saveData[itemJumps] = 0;
	if (!saveData.count(itemDoubleJumps)) saveData[itemDoubleJumps] = 0;
	std::vector<int> levelID = getNumericInDirectory(folder);
	levels.resize(levelID.size());
	for (unsigned i = 0; i < levels.size(); ++i) {
	//	printf("Loading level %d\n", levelID[i]);
		levels[i].load(folder + "/" + intToString(levelID[i]));
		levels[i].id = levelID[i];
	}
	if (saveData[itemLastSaved] >= 0) currentLevel = saveData[itemLastSaved];
	return okay;
}

gameResult gameChapter::save(std::string folder) {
	std::ofstream fout(folder + "/config.json");
	if (!fout.is_open()) return fileBroken;
	fout << std::setw(4) << saveData << std::endl;
	fout.close();
	return okay;
}

gameResult gameManager::load(std::string folder) {
	loadJSONFromFile(folder + "/config.json", saveData);
	if (!saveData.count(itemTime)) saveData[itemTime] = 0.0;
	if (!saveData.count(itemDeaths)) saveData[itemDeaths] = 0;
	if (!saveData.count(itemLastSaved)) saveData[itemLastSaved] = -1;
	if (!saveData.count(itemDistance)) saveData[itemDistance] = 0.0;
	if (!saveData.count(itemXplorations)) saveData[itemXplorations] = 0;
	if (!saveData.count(itemJumps)) saveData[itemJumps] = 0;
	if (!saveData.count(itemDoubleJumps)) saveData[itemDoubleJumps] = 0;
	if (!saveData.count(itemVictories)) saveData[itemVictories] = 0;
	if (!saveData.count(itemVolume)) saveData[itemVolume] = 0.5;

	setVolume((float)saveData[itemVolume]);

	std::vector<int> chapterID = getNumericInDirectory(folder);
	chapters.resize(chapterID.size());
	for (unsigned i = 0; i < chapters.size(); ++i) {
	//	printf("Loading chapter %d\n", chapterID[i]);
		chapters[i].load(folder + "/" + intToString(chapterID[i]));
		chapters[i].id = chapterID[i];
	}
	if (saveData[itemLastSaved] >= 0) currentChapter = saveData[itemLastSaved];
	return okay;
}

gameResult gameManager::save(std::string folder) {
	std::ofstream fout(folder + "/config.json");
	if (!fout.is_open()) return fileBroken;
	fout << std::setw(4) << saveData << std::endl;
	fout.close();
	return okay;
}

gameLevel &theLevel() {
	return gameMaster.chapters[currentChapter].levels[currentLevel];
}

gameChapter &theChapter() {
	return gameMaster.chapters[currentChapter];
}

void deleteSave() {
	gameMaster.saveData[itemLastSaved] = -1;
	for (int i = 0; i < (int)gameMaster.chapters.size(); ++i) {
		gameMaster.chapters[i].saveData[itemLastSaved] = -1;
		gameMaster.chapters[i].saveData[itemDistance] = 0.0;
		gameMaster.chapters[i].saveData[itemJumps] = 0;
		gameMaster.chapters[i].saveData[itemDoubleJumps] = 0;
		gameMaster.chapters[i].saveData[itemDeaths] = 0;
		gameMaster.chapters[i].saveData[itemTime] = 0.0;
		for (int j = 0; j < (int)gameMaster.chapters[i].levels.size(); ++j) {
			gameMaster.chapters[i].levels[j].saveData[itemLastX] = -1.0;
			gameMaster.chapters[i].levels[j].saveData[itemLastY] = -1.0;
		}
	}
}

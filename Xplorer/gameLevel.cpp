#include "defs.h"
#include <string>
#include <fstream>
#include <d2d1.h>
#include <dwrite.h>
#include "json.h"
#include "gameLevel.h"
#include "utility.h"

extern float dpiX, dpiY;

extern ID2D1Factory *d2dFactory;
extern IWICImagingFactory *imageFactory;
extern IDWriteFactory *writeFactory;
extern IDWriteTextFormat *textFormatNormal;

extern ID2D1DCRenderTarget *mainRenderer;
extern ID2D1SolidColorBrush *brushBlack;

using json = nlohmann::json;

XplorerResult gameManager::load(std::string folder) {
	std::ifstream fin(folder + "/config.json");
	if (fin.bad()) return XplorerResult::fileBroken;
	json data;
	fin >> data;
	chapters.resize(data["chapters"]);
	for (unsigned i = 0; i < chapters.size(); ++i) {
		chapters[i].load(folder + "/" + intToString(i));
	}
	fin.close();
	return XplorerResult::okay;
}

XplorerResult gameChapter::load(std::string folder) {
	std::ifstream fin(folder + "/config.json");
	if (fin.bad()) return XplorerResult::fileBroken;
	json data;
	fin >> data;
	chapterName = data["title"];
	levels.resize(data["levels"]);
	for (unsigned i = 0; i < levels.size(); ++i) {
		levels[i].load(folder + "/" + intToString(i));
	}
	fin.close();
	return XplorerResult::okay;
}

XplorerResult gameLevel::load(std::string folder) {
	// Step 1: read the data

	json data, metaData;

	std::ifstream fin(folder + "/data.json");
	if (fin.bad()) return XplorerResult::fileBroken;
	fin >> data;
	fin.close();

	fin.open(folder + "/tiles.json");
	if (fin.bad()) return XplorerResult::fileBroken;
	fin >> metaData;
	fin.close();

	// Step 2: make the grid

	rows = data["height"];
	columns = data["width"];
	grid.resize(columns);
	for (int i = 0; i < columns; ++i) grid[i].resize(rows);
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

	safeRelease(bitmap);
	safeRelease(objects);
	safeRelease(frame);

	if (SUCCEEDED(result)) {
		result = mainRenderer->CreateCompatibleRenderTarget(makeSizeF((float)columns * unitSize, (float)rows * unitSize), &frame);
	}
	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(mainRenderer, imageFactory, stringToWidestring(metaData["image"]).c_str(), metaData["imagewidth"], metaData["imageheight"], &objects);
	}

	frame->BeginDraw();

	ID2D1Bitmap *background = nullptr;

	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(frame, imageFactory, stringToWidestring(data["layers"][0]["image"]).c_str(), columns * unitSize, rows * unitSize, &background);
	}
	if (SUCCEEDED(result)) {
		frame->DrawBitmap(background, makeRectF(0.f, 0.f, columns * unitSize, rows * unitSize));
	}

	safeRelease(background);

	if (SUCCEEDED(result)) {
		for (int i = 0; i < columns; ++i) for (int j = 0; j < rows; ++j) {
			if (grid[i][j] == blockEmpty) continue;
			if (grid[i][j] == blockStartingPoint) {
				initialPosition.vX = (double)i * unitSize;
				initialPosition.vY = (double)j * unitSize;
				continue;
			}
			frame->DrawBitmap(objects, makeRectF((float)i * unitSize, (float)j * unitSize, (float)(i + 1) * unitSize, (float)(j + 1) * unitSize), 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, makeRectF((float)grid[i][j] * unitSize, 0.f, (float)(grid[i][j] + 1) * unitSize, (float)unitSize));
		}
	}
	// Step 4: render text

	IDWriteTextFormat *textFormat = nullptr;

	if (SUCCEEDED(result)) {
		for (auto textObject : data["layers"][2]["objects"]) {
			std::wstring text = stringToWidestring(textObject["text"]["text"]);
			bool isBold = false, isItalic = false;
			if (textObject["text"]["bold"] == true) isBold = true;
			if (textObject["text"]["italic"] == true) isItalic = true;

			result = writeFactory->CreateTextFormat(stringToWidestring(textObject["text"]["fontfamily"]).c_str(), nullptr, isBold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_REGULAR, isItalic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, (float)textObject["text"]["pixelsize"], L"en-us", &textFormat);

			if (SUCCEEDED(result)) {
				frame->DrawTextA(text.c_str(), text.length(), textFormat, makeRectF((float)textObject["x"], (float)textObject["y"], (float)textObject["x"] + (float)textObject["width"], (float)textObject["y"] + (float)textObject["height"]), brushBlack);
			}
			safeRelease(textFormat);
		}
	}

	frame->EndDraw();

	// Step 5: finish
	if (SUCCEEDED(result)) {
		result = frame->GetBitmap(&bitmap);
	}
	if (result != S_OK) return XplorerResult::direct2DError;
	return XplorerResult::okay;
}

gameLevel::~gameLevel() {
	safeRelease(bitmap);
	safeRelease(objects);
	safeRelease(frame);
}

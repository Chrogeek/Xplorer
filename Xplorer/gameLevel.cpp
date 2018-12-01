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
extern ID2D1DCRenderTarget *mainRenderer;

using json = nlohmann::json;

XplorerResult gameManager::load(std::string folder) {
	std::ifstream fin(folder + "/config.json");
	if (fin.bad()) return XplorerResult::fileNotFound;
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
	if (fin.bad()) return XplorerResult::fileNotFound;
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

	std::ifstream fin(folder + "/config.json");
	if (fin.bad()) return XplorerResult::fileNotFound;
	json data;
	fin >> data;
	rows = data["rows"];
	columns = data["columns"];
	grid.resize(columns);
	for (int i = 0; i < columns; ++i) grid[i].resize(rows);
	fin.close();
	FILE *fdata = nullptr;
	if (fopen_s(&fdata, (folder + "/data.csv").c_str(), "r") != 0) return XplorerResult::fileNotFound;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < columns - 1; ++j) {
			if (fscanf_s(fdata, "%d,", &grid[j][i]) != 1)
				return XplorerResult::fileBroken;
		}
		if (fscanf_s(fdata, "%d", &grid[columns - 1][i]) != 1)
			return XplorerResult::fileBroken;
	}
	fclose(fdata);

	D2D1_BITMAP_PROPERTIES properties;
	properties.dpiX = dpiX;
	properties.dpiY = dpiY;
	properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;

	// Step 2: render the image

	bitmap = nullptr;

	ID2D1Bitmap *objects = nullptr;

	HRESULT result = S_OK;

	if (SUCCEEDED(result)) {
		result = mainRenderer->CreateCompatibleRenderTarget(makeSizeF((float)columns * unitSize, (float)rows * unitSize), &frame);
	}

	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(mainRenderer, imageFactory, L"images/objects.png", 64 * unitSize, unitSize, &objects);
	}

	if (SUCCEEDED(result)) {
		frame->BeginDraw();
		for (int i = 0; i < columns; ++i) for (int j = 0; j < rows; ++j) {
			if (grid[i][j] == blockEmpty) continue;
			if (grid[i][j] == blockStartingPoint) {
				initialPosition.vX = (double)i * unitSize;
				initialPosition.vY = (double)j * unitSize;
				continue;
			}
			frame->DrawBitmap(objects, makeRectF((float)i * unitSize, (float)j * unitSize, (float)(i + 1) * unitSize, (float)(j + 1) * unitSize), 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, makeRectF((float)grid[i][j] * unitSize, 0.f, (float)(grid[i][j] + 1) * unitSize, (float)unitSize));
		}
		frame->EndDraw();
	}

	if (SUCCEEDED(result)) {
		result = frame->GetBitmap(&bitmap);
	}

	safeRelease(objects);

	if (result != S_OK) return XplorerResult::direct2DError;
	return XplorerResult::okay;
}

gameLevel::~gameLevel() {
	safeRelease(bitmap);
	safeRelease(frame);
}

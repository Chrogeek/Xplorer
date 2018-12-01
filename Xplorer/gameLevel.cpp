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
	std::ifstream fin(folder + "/config.json");
	if (fin.bad()) return XplorerResult::fileNotFound;
	json data;
	fin >> data;
	rows = data["rows"];
	columns = data["columns"];
	grid.resize(rows);
	for (int i = 0; i < rows; ++i) grid[i].resize(columns);
	fin.close();
	FILE *fdata = nullptr;
	if (fopen_s(&fdata, (folder + "/data.csv").c_str(), "r") != 0) return XplorerResult::fileNotFound;
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < columns - 1; ++j) {
			if (fscanf_s(fdata, "%d,", &grid[i][j]) != 1)
				return XplorerResult::fileBroken;
		}
		if (fscanf_s(fdata, "%d", &grid[i][columns - 1]) != 1)
			return XplorerResult::fileBroken;
	}
	fclose(fdata);

	D2D1_BITMAP_PROPERTIES properties;
	properties.dpiX = dpiX;
	properties.dpiY = dpiY;
	properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;

	HRESULT result = S_OK;

	if (SUCCEEDED(result)) {
		result = mainRenderer->CreateCompatibleRenderTarget(&frame);
	}

	if (SUCCEEDED(result)) {
//		loadBitmapFromFile()
	}

	if (result != S_OK) return XplorerResult::direct2DError;
	return XplorerResult::okay;
}

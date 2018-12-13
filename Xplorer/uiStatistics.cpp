#include "defs.h"
#include <string>
#include <algorithm>
#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include "game.h"
#include "geometry.h"
#include "handler.h"
#include "utility.h"
#include "animation.h"
#include "gameFrame.h"
#include "gameLevel.h"
#include "uiStatistics.h"

extern gameFrame *nextFrame, *statisticsFrame, *lastFrame, *inGameFrame, *mainFrame;
extern buttonUI *buttons[maxButton + 1];
extern ID2D1Factory *d2dFactory;
extern IWICImagingFactory *imageFactory;
extern IDWriteFactory *writeFactory;
extern ID2D1SolidColorBrush *brushWhite, *brushBlack;
extern ID2D1DCRenderTarget *mainRenderer;

extern gameFrame *currentFrame;
extern gameManager gameMaster;

extern int currentChapter, currentLevel;
extern ID2D1Bitmap *heroImage;
extern gameHero hero;
extern animation animator;

double totalDistance, totalSeconds;
int totalDeaths, totalXplorations;
int totalJumps, totalDoubleJumps;
int chapterID, totalVictories;
double gradePoint;

D2D1::ColorF hsvToRGB(double h, double s, double v, double a = 1.0) {
	double c = v * s,
		x = c * (1.0 - fabs((h / 60.0) - floor((h / 60.0) / 2.0) * 2.0 - 1.0)),
		m = v - c;
	double rr, gg, bb;
	if (h < 60.0) rr = c, gg = x, bb = 0.0;
	else if (h < 120.0) rr = x, gg = c, bb = 0.0;
	else if (h < 180.0) rr = 0.0, gg = c, bb = x;
	else if (h < 240.0) rr = 0.0, gg = x, bb = c;
	else if (h < 300.0) rr = x, gg = 0.0, bb = c;
	else rr = c, gg = 0.0, bb = x;
	return D2D1::ColorF((float)(rr + m), (float)(gg + m), (float)(bb + m), (float)a);
}

D2D1::ColorF getGradePointColor(double gradePoint) {
	return hsvToRGB(gradePoint * 30.0, 0.8, 0.8, 1.0);
}

void loadStatistics(int chapter) {
	if (chapter < 0 || chapter > (int)gameMaster.chapters.size()) {
		// load overall data
		totalDistance = gameMaster.saveData[itemDistance];
		totalSeconds = gameMaster.saveData[itemTime];
		totalDeaths = gameMaster.saveData[itemDeaths];
		totalXplorations = gameMaster.saveData[itemXplorations];
		totalJumps = gameMaster.saveData[itemJumps];
		totalDoubleJumps = gameMaster.saveData[itemDoubleJumps];
		totalVictories = gameMaster.saveData[itemVictories];
		chapterID = -1;
	} else {
		// load chapter data
		totalDistance = gameMaster.chapters[chapter].saveData[itemDistance];
		totalSeconds = gameMaster.chapters[chapter].saveData[itemTime];
		totalDeaths = gameMaster.chapters[chapter].saveData[itemDeaths];
		totalXplorations = -1;
		totalJumps = gameMaster.chapters[chapter].saveData[itemJumps];
		totalDoubleJumps = gameMaster.chapters[chapter].saveData[itemDoubleJumps];
		chapterID = chapter;
		totalVictories = -1;
		// calculate grade point
		gradePoint = totalDeaths <= 0 ? 4.0 : std::min(log(std::max(totalSeconds / totalDeaths, 45.0) - 44.0) / log(156.0) * 3.9, 3.9);
	}
}

void loadStatisticsFrame() {
	HRESULT result = S_OK;
	safeNew(statisticsFrame, gameFrame);
	statisticsFrame->render = renderStatistics;

	/*ID2D1Bitmap *background = nullptr;
	if (SUCCEEDED(result)) {
		result = loadBitmapFromFile(mainFrame->renderer, imageFactory, L"images/bg_black.png", windowClientWidth, windowClientHeight, &background);
	}*/
	if (SUCCEEDED(result)) {
		result = mainRenderer->CreateCompatibleRenderTarget(makeSizeF((float)windowClientWidth, (float)windowClientHeight), &statisticsFrame->renderer);
	}
	if (SUCCEEDED(result)) {
		statisticsFrame->renderer->BeginDraw();
	//	statisticsFrame->renderer->DrawBitmap(background, makeRectF(0.f, 0.f, (float)windowClientWidth, (float)windowClientHeight));
	//	safeRelease(background);
		drawButton(statisticsFrame->renderer, buttons[buttonStatisticsContinue]);

		drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_EXTRA_LIGHT, false, 70.f,
			std::wstring(L"Statistics"),
			makeRectF(0.f, 20.f, windowClientWidth, 95.f), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
		);

		drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_BOLD, true, 24.f,
			chapterID < 0 ? std::wstring(L"Xplorations") : std::wstring(L"Chapter ") + intToWideString(gameMaster.chapters[chapterID].id),
			makeRectF(100.f, 150.f, windowClientWidth, 175.f), DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
		);
		drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_LIGHT, true, 24.f, std::wstring(L"Deaths"),
			makeRectF(100.f, 205.f, windowClientWidth, 230.f), DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
		);
		drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_LIGHT, true, 24.f, std::wstring(L"Jumps"),
			makeRectF(100.f, 260.f, windowClientWidth, 285.f), DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
		);
		drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_LIGHT, true, 24.f, std::wstring(L"Double Jumps"),
			makeRectF(100.f, 315.f, windowClientWidth, 340.f), DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
		);
		drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_LIGHT, true, 24.f, std::wstring(L"Distance"),
			makeRectF(100.f, 370.f, windowClientWidth, 395.f), DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
		);
		drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_LIGHT, true, 24.f, std::wstring(chapterID < 0 ? L"Total Time Played" : L"Time"),
			makeRectF(100.f, 425.f, windowClientWidth, 450.f), DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
		);
		if (chapterID < 0) {
			drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_LIGHT, true, 24.f, std::wstring(L"Victories"),
				makeRectF(100.f, 480.f, windowClientWidth, 505.f), DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
			);
		} else {
			drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_BOLD, true, 24.f, std::wstring(L"Grade Point"),
				makeRectF(100.f, 480.f, windowClientWidth, 505.f), DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
			);
		}

		drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_BOLD, false, 24.f,
			chapterID < 0 ? intToWideString(totalXplorations) : stringToWidestring(gameMaster.chapters[chapterID].saveData[itemTitle]),
			makeRectF(0.f, 150.f, windowClientWidth - 100.f, 175.f), DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
		);
		drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_LIGHT, false, 24.f, intToWideString(totalDeaths),
			makeRectF(0.f, 205.f, windowClientWidth - 100.f, 230.f), DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
		);
		drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_LIGHT, false, 24.f, intToWideString(totalJumps),
			makeRectF(0.f, 260.f, windowClientWidth - 100.f, 285.f), DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
		);
		drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_LIGHT, false, 24.f, intToWideString(totalDoubleJumps),
			makeRectF(0.f, 315.f, windowClientWidth - 100.f, 340.f), DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
		);
		drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_LIGHT, false, 24.f, intToWideString(longint(totalDistance / unitSize * 2)) + L" m",
			makeRectF(0.f, 370.f, windowClientWidth - 100.f, 395.f), DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
		);
		drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_LIGHT, false, 24.f, secondsToWideString(totalSeconds),
			makeRectF(0.f, 425.f, windowClientWidth - 100.f, 450.f), DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
		);
		if (chapterID < 0) {
			drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_LIGHT, false, 24.f, intToWideString(totalVictories),
				makeRectF(0.f, 480.f, windowClientWidth - 100.f, 505.f), DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brushWhite
			);
		} else {
			ID2D1SolidColorBrush *brush = nullptr;
			statisticsFrame->renderer->CreateSolidColorBrush(getGradePointColor(gradePoint), &brush);
			if (brush != nullptr) {
				drawText(statisticsFrame->renderer, writeFactory, std::wstring(gameFontName), DWRITE_FONT_WEIGHT_SEMI_BOLD, false, 24.f, doubleToWideString(gradePoint),
					makeRectF(0.f, 480.f, windowClientWidth - 100.f, 505.f), DWRITE_TEXT_ALIGNMENT_TRAILING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, brush
				);
			}
			safeRelease(brush);
		}

		statisticsFrame->renderer->EndDraw();
	}
	if (SUCCEEDED(result)) {
		result = statisticsFrame->renderer->GetBitmap(&statisticsFrame->bitmap);
	}
	statisticsFrame->enter = showStatistics;
	statisticsFrame->leave = leaveStatistics;

	if (chapterID == gameMaster.chapters.size() - 1) {
		deleteSave(); // delete save file
		increase(gameMaster.saveData[itemVictories], int, 1); // increase victory count by 1
	}
}

void renderStatistics() {
	mainRenderer->DrawBitmap(statisticsFrame->bitmap, makeRectF(0.f, 0.f, (float)windowClientWidth, (float)windowClientHeight));
	renderRain(0.08f);
}

void showStatisticsFinish() {
	currentFrame = statisticsFrame;
}

void showStatistics() {
	makeRain();
	disableAllButtons();
	buttons[buttonStatisticsContinue]->enabled = true;
	animator.startAnimation(statisticsFrame->bitmap, new linearAnimation(timeGetTime(), shortAnimation), showStatisticsFrame, showStatisticsFinish);
}

void showStatisticsFrame(ID2D1Bitmap *bitmap, double progress) {
	crossIn(bitmap, progress);
	renderRain(0.08f * (float)progress);
}

void leaveStatistics() {
	animator.startAnimation(statisticsFrame->bitmap, new linearAnimation(timeGetTime(), shortAnimation), leaveStatisticsFrame, leaveStatisticsFinish);
}

void leaveStatisticsFrame(ID2D1Bitmap *bitmap, double progress) {
	crossOut(bitmap, progress);
	renderRain(0.08f * (float)(1.0 - progress));
}

void leaveStatisticsFinish() {
	if (chapterID >= 0) {
		currentFrame = inGameFrame;
		nextChapter();
	} else {
		loadNextFrame();
	}
}


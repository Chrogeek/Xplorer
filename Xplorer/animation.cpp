#include "defs.h"
#include <algorithm>
#include <d2d1.h>
#include "game.h"
#include "gameFrame.h"
#include "animation.h"

extern animation animator;
extern gameFrame *currentFrame;
extern gameFrame *animationFrame;
extern ID2D1DCRenderTarget *mainRenderer;
extern ID2D1Factory *d2dFactory;
extern gameHero hero;

double linearAnimation::progress(DWORD time) {
	DWORD timeNow = std::min(time, timeStart + totalTime);
	return double(timeNow - timeStart) / totalTime;
}

linearAnimation::linearAnimation(DWORD time, DWORD interval) {
	timeStart = time;
	totalTime = interval;
}

double logarithmicAnimation::progress(DWORD time) {
	DWORD timeNow = std::min(time, timeStart + totalTime);
	return log2(double(timeNow - timeStart) / totalTime + 1.0);
}

logarithmicAnimation::logarithmicAnimation(DWORD time, DWORD interval) {
	timeStart = time;
	totalTime = interval;
}

double arcAnimation::progress(DWORD time) {
	DWORD timeNow = std::min(time, timeStart + totalTime);
	double x = double(timeNow - timeStart) / totalTime;
	return sqrt(-x * x + 2.0 * x);
}

arcAnimation::arcAnimation(DWORD time, DWORD interval) {
	timeStart = time;
	totalTime = interval;
}

void animation::startAnimation(ID2D1Bitmap *target, animationHelper *helper, voidBitmapDoubleFunction onFrame, voidFunction finish) {
	image = target;
	this->helper = helper;
	this->onFrame = onFrame;
	this->finish = finish;
	currentFrame = animationFrame;
}

void animation::routine() {
	DWORD timeNow = timeGetTime();
	if (helper == nullptr) return;
	double progress = helper->progress(timeNow);
	if (onFrame != nullptr) onFrame(image, progress);
	if (timeNow >= helper->timeStart + helper->totalTime) {
		delete helper;
		helper = nullptr;
		if (finish != nullptr) finish();
	}
}

bool animation::expired() {
	if (helper == nullptr) return true;
	return timeGetTime() >= helper->timeStart + helper->totalTime;
}

animation::animation() {
	onFrame = nullptr;
	finish = nullptr;
	image = nullptr;
}

void circularExpand(ID2D1Bitmap *bitmap, double progress) {
	rectFloat srcRect, destRect;
	pointVector point = rectCenter(hero.rect());
	getRenderRect(bitmap, hero, srcRect, destRect);

	pointVector center = point - pointVector(srcRect.left, srcRect.top) + pointVector(destRect.left, destRect.top);

	double dist = sqrt(pow(std::max(center.vX - destRect.left, destRect.right - center.vX), 2.0) + pow(std::max(center.vY - destRect.top, destRect.bottom - center.vY), 2.0));
	float len = std::max((float)(dist * progress), (float)maxDelta);
	HRESULT result = S_OK;

	ID2D1EllipseGeometry *ellipse = nullptr;
	ID2D1Layer *pLayer = nullptr;
	if (SUCCEEDED(result)) {
		result = d2dFactory->CreateEllipseGeometry(makeEllipse(center, len, len), &ellipse);
	}
	if (SUCCEEDED(result)) {
		result = mainRenderer->CreateLayer(nullptr, &pLayer);
	}
	if (SUCCEEDED(result)) {
		mainRenderer->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), ellipse), pLayer);
		renderGameFrame(bitmap, hero, srcRect, destRect, (float)progress);
		mainRenderer->PopLayer();
	}
	safeRelease(pLayer);
	safeRelease(ellipse);
}

void crossOut(ID2D1Bitmap *bitmap, double progress) {
	const int k = 25;
	mainRenderer->Clear();
	for (int i = 0; i < k; ++i) {
		double x;
		if (k == 1) {
			x = pow(progress, 2.0);
		} else {
			double t = std::min(2.0 / 3.0, std::max(0.0, progress - i / (3.0 * (k - 1))));
			x = 2.25 * pow(t, 2.0);
			if (i % 2 == 1) x = -x;
		}
		x *= windowClientWidth;
		int top = (int)((double(i) / double(k)) * windowClientHeight), bottom = (int)((double(i + 1) / double(k)) * windowClientHeight) + 1;
		mainRenderer->DrawBitmap(bitmap,
			makeRectF((float)x, (float)top, (float)(x + windowClientWidth), (float)bottom),
			1.f,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			makeRectF(0.f, (float)top, (float)windowClientWidth, (float)bottom)
		);
	}
}

void circularShrink(ID2D1Bitmap *bitmap, double progress) {
	rectFloat srcRect, destRect;
	pointVector point = rectCenter(hero.rect());
	getRenderRect(bitmap, hero, srcRect, destRect);

	pointVector center = point - pointVector(srcRect.left, srcRect.top) + pointVector(destRect.left, destRect.top);

	double dist = sqrt(pow(std::max(center.vX, destRect.right - center.vX), 2.0) + pow(std::max(center.vY, destRect.bottom - center.vY), 2.0));
	float len = std::max((float)(dist * (1.0 - progress)), (float)maxDelta);

	HRESULT result = S_OK;

	ID2D1EllipseGeometry *ellipse = nullptr;
	ID2D1Layer *pLayer = nullptr;
	if (SUCCEEDED(result)) {
		result = d2dFactory->CreateEllipseGeometry(makeEllipse(center, len, len), &ellipse);
	}
	if (SUCCEEDED(result)) {
		result = mainRenderer->CreateLayer(nullptr, &pLayer);
	}
	if (SUCCEEDED(result)) {
		mainRenderer->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), ellipse), pLayer);
		renderGameFrame(bitmap, hero, srcRect, destRect, 1.f - (float)progress);
		mainRenderer->PopLayer();
	}
	safeRelease(pLayer);
	safeRelease(ellipse);
}

void crossIn(ID2D1Bitmap *bitmap, double progress) {
	const int k = 25;
	progress = 1.0 - progress;
	mainRenderer->Clear();
	for (int i = 0; i < k; ++i) {
		double x;
		if (k == 1) {
			x = pow(progress, 2.0);
		} else {
			double t = std::min(2.0 / 3.0, std::max(0.0, progress - (k - 1 - i) / (3.0 * (k - 1))));
			x = 2.25 * pow(t, 2.0);
			if (i % 2 == 0) x = -x;
		}
		x *= windowClientWidth;
		int top = (int)((double(i) / double(k)) * windowClientHeight), bottom = (int)((double(i + 1) / double(k)) * windowClientHeight) + 1;
		mainRenderer->DrawBitmap(bitmap,
			makeRectF((float)x, (float)top, (float)(x + windowClientWidth), (float)bottom),
			1.f,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			makeRectF(0.f, (float)top, (float)windowClientWidth, (float)bottom)
		);
	}
}

void fadeIn(ID2D1Bitmap *bitmap, double progress) {
	rectFloat srcRect, destRect;
	getRenderRect(bitmap, hero, srcRect, destRect);
	mainRenderer->DrawBitmap(bitmap, destRect, (float)progress, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, srcRect);
}

void fadeOut(ID2D1Bitmap *bitmap, double progress) {
	rectFloat srcRect, destRect;
	getRenderRect(bitmap, hero, srcRect, destRect);
	mainRenderer->DrawBitmap(bitmap, destRect, (float)(1.0 - progress), D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, srcRect);
}

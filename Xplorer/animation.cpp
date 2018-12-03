#include "defs.h"
#include <algorithm>
#include <d2d1.h>
#include "gameFrame.h"
#include "animation.h"

extern animation currentAnimation;
extern gameFrame *currentFrame;
extern gameFrame *animationFrame;
extern ID2D1DCRenderTarget *mainRenderer;
extern ID2D1Factory *d2dFactory;

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

void animation::startAnimation(ID2D1Bitmap *target, animationHelper *helper, voidBitmapPointDoubleFunction onFrame, voidFunction finish, pointVector central) {
	image = target;
	this->helper = helper;
	this->onFrame = onFrame;
	this->finish = finish;
	this->central = central;
	currentFrame = animationFrame;
}

void animation::routine() {
	DWORD timeNow = timeGetTime();
	if (helper == nullptr) return;
	double progress = helper->progress(timeNow);
	if (timeNow >= helper->timeStart + helper->totalTime) {
		delete helper;
		helper = nullptr;
		finish();
	}
	onFrame(image, central, progress);
}

animation::animation() {
	onFrame = nullptr;
	finish = nullptr;
	image = nullptr;
}

void circularExpand(ID2D1Bitmap *bitmap, pointVector point, double progress) {
	D2D1_SIZE_F sizeF = bitmap->GetSize();

	float rLeft = std::max(0.f, std::min((float)point.vX - windowClientWidth / 2.f, (float)sizeF.width - windowClientWidth));
	float rTop = std::max(0.f, std::min((float)point.vY - windowClientHeight / 2.f, (float)sizeF.height - windowClientHeight));

	pointVector center = point - pointVector(rLeft, rTop);

	double dist = sqrt(pow(std::max(center.vX, windowClientWidth - center.vX), 2.0) + pow(std::max(center.vY, windowClientHeight - center.vY), 2.0));
	float len = (float)(dist * progress);

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
		mainRenderer->DrawBitmap(bitmap, makeRectF(0.f, 0.f, (float)windowClientWidth, (float)windowClientHeight), 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, makeRectF(rLeft, rTop, rLeft + windowClientWidth, rTop + windowClientHeight));
		mainRenderer->PopLayer();
	}
	safeRelease(pLayer);
	safeRelease(ellipse);
}

void crossExpand(ID2D1Bitmap *bitmap, pointVector point, double progress) {
	const int k = 7;
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
		int top = (int)((double(i) / double(k)) * windowClientHeight), bottom = (int)((double(i + 1) / double(k)) * windowClientHeight);
		mainRenderer->DrawBitmap(bitmap,
			makeRectF((float)x, (float)top, (float)(x + windowClientWidth), (float)bottom),
			1.f,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			makeRectF(0.f, (float)top, (float)windowClientWidth, (float)bottom)
		);
	}
}

void circularShrink(ID2D1Bitmap *bitmap, pointVector point, double progress) {
	D2D1_SIZE_F sizeF = bitmap->GetSize();

	float rLeft = std::max(0.f, std::min((float)point.vX - windowClientWidth / 2.f, (float)sizeF.width - windowClientWidth));
	float rTop = std::max(0.f, std::min((float)point.vY - windowClientHeight / 2.f, (float)sizeF.height - windowClientHeight));

	pointVector center = point - pointVector(rLeft, rTop);

	double dist = sqrt(pow(std::max(center.vX, windowClientWidth - center.vX), 2.0) + pow(std::max(center.vY, windowClientHeight - center.vY), 2.0));
	float len = (float)(dist * (1.0 - progress));

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
		mainRenderer->DrawBitmap(bitmap, makeRectF(0.f, 0.f, (float)windowClientWidth, (float)windowClientHeight), 1.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, makeRectF(rLeft, rTop, rLeft + windowClientWidth, rTop + windowClientHeight));
		mainRenderer->PopLayer();
	}
	safeRelease(pLayer);
	safeRelease(ellipse);
}

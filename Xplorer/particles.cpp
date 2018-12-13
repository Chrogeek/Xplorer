#include "defs.h"
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <windows.h>
#include "utility.h"
#include "particles.h"

particle::particle(int type, D2D1_COLOR_F color) {
	v.vX = randomDouble(particleMinVelocity.vX, particleMaxVelocity.vX);
	v.vY = randomDouble(particleMinVelocity.vY, particleMaxVelocity.vY);
	this->type = type;
	this->color = color;
	initTime = lastTime = timeGetTime();
}

bool particle::update(rectFloat boundingBox) {
	DWORD thisTime = timeGetTime();
	double interval = (thisTime - lastTime) / 1000.0;
	if (type == particleGravitational) {
		v += gravityAcceleration * interval * 2.0;
	} else if (type == particleUniform) {
		//v.vY = randomDouble(particleMinVelocity.vY, particleMaxVelocity.vY);
	}
	p += v * interval;
	double newX = std::min((double)boundingBox.right, std::max((double)boundingBox.left, p.vX)),
		newY = std::min((double)boundingBox.bottom, std::max((double)boundingBox.top, p.vY));
	if (dcmp(p.vX, newX) != 0) {
		p.vX = newX;
		v.vX = 0.0;
		if (randomDouble(0.0, 1.0) >= 0.95) v.vY = 0.0;
	}
	if (dcmp(p.vY, newY) != 0) {
		p.vY = newY;
		v.vY = 0.0;
		if (randomDouble(0.0, 1.0) >= 0.95) v.vX = 0.0;
	}
	lastTime = thisTime;
	return (thisTime - initTime >= maxParticleLife);
}

void particle::render(ID2D1RenderTarget *renderer, rectFloat srcRect, rectFloat destRect, float opacity) {
	ID2D1SolidColorBrush *brush = nullptr;
	HRESULT result = S_OK;

	if (SUCCEEDED(result)) {
		result = renderer->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, opacity), &brush);
	}
	if (SUCCEEDED(result)) {
		renderer->FillEllipse(makeEllipse(p - pointVector(srcRect.left, srcRect.top) + pointVector(destRect.left, destRect.top), 1.5, 1.5), brush);
	}
	safeRelease(brush);
}

bool particle::updateAndRender(ID2D1RenderTarget *renderer, rectFloat srcRect, rectFloat destRect, float opacity) {
	DWORD thisTime = timeGetTime();
	double interval = (thisTime - lastTime) / 1000.0;
	if (type == particleGravitational) {
		v += gravityAcceleration * interval * 2.0;
	} else if (type == particleUniform) {
		//v.vY = randomDouble(particleMinVelocity.vY, particleMaxVelocity.vY);
	}
	pointVector oldP = p;
	bool sticked = false;
	p += v * interval;
	double newX = std::min((double)srcRect.right, std::max((double)srcRect.left, p.vX)),
		newY = std::min((double)srcRect.bottom, std::max((double)srcRect.top, p.vY));
	if (dcmp(p.vX, newX) != 0) {
		p.vX = newX;
		v.vX = 0.0;
		sticked = true;
		if (randomDouble(0.0, 1.0) >= 0.95) v.vY = 0.0;
	}
	if (dcmp(p.vY, newY) != 0) {
		p.vY = newY;
		v.vY = 0.0;
		sticked = true;
		if (randomDouble(0.0, 1.0) >= 0.95) v.vX = 0.0;
	}
	lastTime = thisTime;
	ID2D1SolidColorBrush *brush = nullptr;
	HRESULT result = S_OK;

	if (SUCCEEDED(result)) {
		result = renderer->CreateSolidColorBrush(D2D1::ColorF(color.r, color.g, color.b, (float)opacity), &brush);
	}
	if (SUCCEEDED(result)) {
		//renderer->FillEllipse(makeEllipse(p - pointVector(srcRect.left, srcRect.top) + pointVector(destRect.left, destRect.top), 1.5, 1.5), brush);
		renderer->DrawLine(pointToD2Point(oldP - pointVector(srcRect.left, srcRect.top) + pointVector(destRect.left, destRect.top)), pointToD2Point(p - pointVector(srcRect.left, srcRect.top) + pointVector(destRect.left, destRect.top)), brush, 1.5f);
	}
	safeRelease(brush);
	return (thisTime - initTime >= maxParticleLife) || sticked;
}

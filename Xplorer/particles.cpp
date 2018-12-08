#include <cstdlib>
#include <vector>
#include "utility.h"
#include "particles.h"

particle::particle(int type) {
	v.vX = randomDouble(particleMinVelocity.vX, particleMaxVelocity.vX);
	v.vY = randomDouble(particleMinVelocity.vY, particleMaxVelocity.vY);
	this->type = type;
}

void updateParticles(std::vector<particle *> &particles) {
	for (int i = 0; i < (int)particles.size(); ++i) {
		particle *o = particles[i];
		o->p += o->v;
		if (o->type == particleGravitational) {
			o->v += gravityAcceleration / 10.0;
		} else if (o->type == particleUniform) {
			o->v.vY = randomDouble(particleMinVelocity.vY, particleMaxVelocity.vY);
		}
	}
}

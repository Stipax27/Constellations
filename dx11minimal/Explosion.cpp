#ifndef _EXPLOSION_C_
#define _EXPLOSION_C_

#include "component.h"

struct Explosion : Component {

	float max_radius = 50.f;
	float radius = 0.f;
	float speed = 5.f;

};

#endif
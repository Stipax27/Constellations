#ifndef _HEALTH_C_
#define _HEALTH_C_

#include "component.h"
#include "fraction.h"

struct Health : Component
{
	float hp = 100.0f;
	float maxHp = 100.0f;

	Fraction fraction = Fraction::Enemy;
};

#endif
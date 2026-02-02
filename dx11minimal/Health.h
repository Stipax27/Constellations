#ifndef _HEALTH_H_
#define _HEALTH_H_

#include "component.h"
#include "fraction.h"

struct Health : Component
{
	float hp = 100.0f;
	float maxHp = 100.0f;

	Fraction fraction = Fraction::Enemy;
};

#endif
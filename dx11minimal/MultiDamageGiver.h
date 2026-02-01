#ifndef _MULTI_DAMAGE_GIVER_H_
#define _MULTI_DAMAGE_GIVER_H_

#include "component.h"
#include "timer.h"

struct MultiDamageGiver : Component
{
	float damage = 1.0f;
	int repeats = 10;

	double inverval = 100;
	double lastDamageTime = 0;
};

#endif
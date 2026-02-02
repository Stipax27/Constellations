#ifndef _MULTI_DAMAGER_H_
#define _MULTI_DAMAGER_H_

#include "component.h"
#include "timer.h"

struct MultiDamager : Component
{
	float damage = 1.0f;
	int repeats = 10;

	double inverval = 100;
	double lastDamageTime = 0;
};

#endif
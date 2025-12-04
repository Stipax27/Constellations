#ifndef _MULTI_DAMAGE_GIVER_C_
#define _MULTI_DAMAGE_GIVER_C_

#include "component.h"
#include "timer.h"

struct MultiDamageGiver : Component
{
	float damage = 1.0f;
	int repeats = 10;

	DWORD inverval = 100;
	DWORD lastDamageTime = 0;
};

#endif
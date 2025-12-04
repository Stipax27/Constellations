#ifndef _SINGLE_DAMAGE_GIVER_C_
#define _SINGLE_DAMAGE_GIVER_C_

#include "component.h"

struct SingleDamageGiver : Component
{
	float damage = 10.0f;
};

#endif
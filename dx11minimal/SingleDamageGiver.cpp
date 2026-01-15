#ifndef _SINGLE_DAMAGE_GIVER_C_
#define _SINGLE_DAMAGE_GIVER_C_

#include "component.h"
#include "fraction.h"

struct SingleDamageGiver : Component
{
	Fraction target = Fraction::Player;
	bool destroyable = true;
	float damage = 10.0f;
};

#endif
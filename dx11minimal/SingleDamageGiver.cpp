#ifndef _SINGLE_DAMAGE_GIVER_C_
#define _SINGLE_DAMAGE_GIVER_C_

#include "component.h"
#include "damagetarget.h"

struct SingleDamageGiver : Component
{
	DamageTarget target = DamageTarget::Player;
	float damage = 10.0f;
};

#endif
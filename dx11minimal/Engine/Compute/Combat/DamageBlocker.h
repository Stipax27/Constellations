#ifndef _DAMAGE_BLOCKER_H_
#define _DAMAGE_BLOCKER_H_

#include "../../ECS_Base/component.h"
#include "damageType.h"

struct DamageBlocker : Component
{
	float physicResistFactor = 1.0f;
	float magicResistFactor = 1.0f;
};

#endif
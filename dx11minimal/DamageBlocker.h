#ifndef _DAMAGE_BLOCKER_H_
#define _DAMAGE_BLOCKER_H_

#include "Engine/ECS_Base/component.h"
#include "damageType.h"

struct DamageBlocker : Component
{
	float resistFactor = 1.0f;
	DamageType damageType;
};

#endif
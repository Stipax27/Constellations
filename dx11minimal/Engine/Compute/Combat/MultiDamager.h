#ifndef _MULTI_DAMAGER_H_
#define _MULTI_DAMAGER_H_

#include "../../ECS_Base/component.h"
#include "fraction.h"
#include "damageType.h"

struct MultiDamager : Component
{
	Fraction target = Fraction::Player;
	DamageType damageType = DamageType::Kinetic;
	bool destroyable = false;

	float damage = 1.0f;
	int repeats = 10;

	double inverval = 100;

	double lastDamageTime = 0;
	int repeatCount = 0;
};

#endif
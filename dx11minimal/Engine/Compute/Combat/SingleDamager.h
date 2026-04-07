#ifndef _SINGLE_DAMAGER_H_
#define _SINGLE_DAMAGER_H_

#include "../../ECS_Base/component.h"
#include "fraction.h"
#include "damageType.h"
#include <vector>

struct SingleDamager : Component
{
	Fraction target = Fraction::Player;
	DamageType damageType = DamageType::Physic;
	bool destroyable = false;

	float damage = 10.0f;
	int maxHitCount = 1;

	vector<int> entityFilter;
};

#endif
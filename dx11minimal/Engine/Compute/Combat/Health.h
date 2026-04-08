#ifndef _HEALTH_H_
#define _HEALTH_H_

#include "../../ECS_Base/component.h"
#include "fraction.h"
#include "damageUnit.h"
#include <vector>


struct Health : Component
{
	float hp = 100.0f;
	float maxHp = 100.0f;

	Fraction fraction = Fraction::Enemy;
	bool destroyOnDeath = true;

	bool immortal = false;

	vector<DamageUnit> damageQueue;
};

#endif
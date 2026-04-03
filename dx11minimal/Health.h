#ifndef _HEALTH_H_
#define _HEALTH_H_

#include "Engine/BasicComponents/component.h"
#include "fraction.h"
#include "damageType.h"
#include <vector>

struct DamageUnit {
	DamageType damageType;
	float damage;

	DamageUnit(DamageType onDamageType, float onDamage)
		: damageType(onDamageType), damage(onDamage)
	{}
};

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
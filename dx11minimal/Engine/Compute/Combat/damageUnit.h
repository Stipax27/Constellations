#ifndef _DAMAGE_UNIT_H_
#define _DAMAGE_UNIT_H_

#include "damageType.h"

struct DamageUnit {
	DamageType damageType;
	float damage;

	DamageUnit() : damageType(DamageType::Physic), damage(0.0f) {}

	DamageUnit(DamageType onDamageType, float onDamage)
		: damageType(onDamageType), damage(onDamage)
	{
	}
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(DamageUnit,
	damageType,
	damage)

#endif
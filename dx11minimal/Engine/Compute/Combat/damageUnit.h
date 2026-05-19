#ifndef _DAMAGE_UNIT_H_
#define _DAMAGE_UNIT_H_

#include "damageType.h"

struct DamageUnit {
	DamageType damageType;
	float damage;

	DamageUnit(DamageType onDamageType, float onDamage)
		: damageType(onDamageType), damage(onDamage)
	{
	}
};

#endif
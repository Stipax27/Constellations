#ifndef _SINGLE_DAMAGE_GIVER_H_
#define _SINGLE_DAMAGE_GIVER_H_

#include "component.h"
#include "fraction.h"
#include <vector>

struct SingleDamageGiver : Component
{
	Fraction target = Fraction::Player;
	bool destroyable = true;
	float damage = 10.0f;
	int maxHitCount = 1;

	vector<int> entityFilter;
};

#endif
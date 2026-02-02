#ifndef _SINGLE_DAMAGER_H_
#define _SINGLE_DAMAGER_H_

#include "component.h"
#include "fraction.h"
#include <vector>

struct SingleDamager : Component
{
	Fraction target = Fraction::Player;
	bool destroyable = true;
	float damage = 10.0f;
	int maxHitCount = 1;

	vector<int> entityFilter;
};

#endif
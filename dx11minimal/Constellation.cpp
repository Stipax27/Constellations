#ifndef _CONSTELLATION_C_
#define _CONSTELLATION_C_

#include "component.h"
#include "point3d.h"
#include <vector>
#include <utility>


struct Constellation : Component
{
	float health = 100;
	float maxHealth = 100;

	vector<point3d> stars = vector<point3d>();
	vector<pair<int, int>> links = vector<pair<int, int>>();
};

#endif
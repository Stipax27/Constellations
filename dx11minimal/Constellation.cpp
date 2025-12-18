#ifndef _CONSTELLATION_C_
#define _CONSTELLATION_C_

#include "component.h"
#include "point3d.h"
#include <vector>
#include <utility>


struct Constellation : Component
{
	float starSize = 0.1f;
	float linkSize = 0.2f;

	vector<point3d> stars = vector<point3d>();
	vector<pair<int, int>> links = vector<pair<int, int>>();
};

#endif
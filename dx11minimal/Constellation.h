#ifndef _CONSTELLATION_H_
#define _CONSTELLATION_H_

#include "component.h"
#include "point3d.h"
#include <vector>
#include <utility>


struct Constellation : Component
{
	float starSize = 0.05f;
	float linkSize = 0.15f;
	float crownRadius = 0.1f;

	point3d starColor1 = point3d(0.95, 0.98, 1);
	point3d starColor2 = point3d(0.55, 0.4, 0.85);
	point3d crownColor = point3d(0.95, 0.9, 1);

	vector<point3d> stars = vector<point3d>();
	vector<pair<int, int>> links = vector<pair<int, int>>();
};

#endif
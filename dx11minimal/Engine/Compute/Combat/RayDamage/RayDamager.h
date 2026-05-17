#ifndef _RAY_DAMAGER_H_
#define _RAY_DAMAGER_H_

#include "../../../ECS_Base/component.h"
#include "../fraction.h"
#include "../damageType.h"
#include "../../../Physic/Collision/CollisionGroup.h"

struct RayDamager : Component
{
	point3d direction = point3d(0, 0, 1);

	Fraction target = Fraction::Player;
	DamageType damageType = DamageType::Pure;
	CollisionFilter::Group collisionGroup = CollisionFilter::Group::Enemy;
	bool touchableOnly = true;

	float damage = 1.0f;
	int repeats = 100;

	double interval = 10;

	double lastDamageTime = 0;
	int repeatCount = 0;
};

#endif
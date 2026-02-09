#ifndef _COLLISION_GROUP_H_
#define _COLLISION_GROUP_H_

namespace CollisionFilter
{
	enum Group
	{
		Projectile,
		Player,
		Enemy,
		PlayerRay
	};

	const bool collisionTable[4][4] = {
		{false, true, true, false},
		{true, false, true, false},
		{true, true, false, true},
		{false, false, true, false},
	};
}

#endif

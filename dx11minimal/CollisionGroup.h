#ifndef _COLLISION_GROUP_H_
#define _COLLISION_GROUP_H_

namespace CollisionFilter
{
	enum Group
	{
		Projectile,
		Player,
		Enemy
	};

	const bool collisionTable[3][3] = {
		{false, true, true},
		{true, false, true},
		{true, true, false},
	};
}

#endif

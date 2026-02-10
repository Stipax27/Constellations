#ifndef _COLLISION_GROUP_H_
#define _COLLISION_GROUP_H_

#define T true
#define F false

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
		{F, T, T, F},
		{T, F, T, F},
		{T, T, F, T},
		{F, F, T, F},
	};
}

#undef T
#undef F

#endif

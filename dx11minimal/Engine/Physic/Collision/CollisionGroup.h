#ifndef _COLLISION_GROUP_H_
#define _COLLISION_GROUP_H_

#define T true
#define F false

namespace CollisionFilter
{
	enum class Group
	{
		Projectile,
		Player,
		Enemy,
		PlayerRay,
	};

	const bool collisionTable[5][5] = {
		{T, T, T, F}, // Projectile
		{T, F, T, F}, // Player
		{T, T, F, T}, // Enemy
		{T, F, T, F}, // PlayerRay
	};
}

#undef T
#undef F

#endif

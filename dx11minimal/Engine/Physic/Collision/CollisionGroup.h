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
		HardBody,
	};

	const bool collisionTable[5][5] = {
		{T, T, T, F, T}, // Projectile
		{T, F, T, F, T}, // Player
		{T, T, F, T, T}, // Enemy
		{F, F, T, F, T}, // PlayerRay
		{T, T, T, T, T}, // HardBody
	};
}

#undef T
#undef F

#endif

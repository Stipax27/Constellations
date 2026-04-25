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
		StaticObject
	};

	const bool collisionTable[5][5] = {
		{F, T, T, F, T},
		{T, F, T, F, T},
		{T, T, F, T, T},
		{F, F, T, F, T},
		{T, T, T, T, T},
	};
}

#undef T
#undef F

#endif

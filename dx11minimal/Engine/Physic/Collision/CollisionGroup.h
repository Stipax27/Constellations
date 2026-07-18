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

		_editorSelect,
	};

	const bool collisionTable[6][6] = {
		{T, T, T, F, T, T}, // Projectile
		{T, F, T, F, T, T}, // Player
		{T, T, F, T, T, T}, // Enemy
		{F, F, T, F, T, T}, // PlayerRay
		{T, T, T, T, T, T}, // HardBody
		{T, T, T, T, T, T}, // _editorSelect
	};
}

#undef T
#undef F

#endif

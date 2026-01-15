////////////////////////////////////////////////////////////////////////////////
// Filename: PlayerAbilities.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PLAYER_ABILITIES_H_
#define _PLAYER_ABILITIES_H_

//////////////
// INCLUDES //
//////////////
#include "world.h"
#include "entity.h"

#include "Point3d.h"

#include "Transform.cpp"
#include "PhysicBody.cpp"
#include "SphereCollider.cpp"
#include "Star.cpp"
#include "DelayedDestroy.cpp"
#include "SingleDamageGiver.cpp"


/////////////
// GLOBALS //
/////////////


////////////////////////////////////////////////////////////////////////////////
// Class name: PlayerAbilities
////////////////////////////////////////////////////////////////////////////////


class PlayerAbilities
{
public:
	PlayerAbilities();
	PlayerAbilities(const PlayerAbilities&);
	~PlayerAbilities();

	void Initialize(World*);
	void Shutdown();

	void Attack(Transform, point3d);

private:
	World* world;
};

#endif
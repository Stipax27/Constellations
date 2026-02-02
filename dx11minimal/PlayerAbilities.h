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

#include "Transform.h"
#include "PhysicBody.h"
#include "SphereCollider.h"
#include "Star.h"
#include "DelayedDestroy.h"
#include "SingleDamager.h"


/////////////
// STRUCTS //
/////////////

enum PlayerWeapons
{
	Fists,
	Sword,
	Bow
};


/////////////
// GLOBALS //
/////////////


////////////////////////////////////////////////////////////////////////////////
// Class name: PlayerAbilities
////////////////////////////////////////////////////////////////////////////////


class PlayerAbilities
{
public:
	PlayerWeapons weapon;
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
////////////////////////////////////////////////////////////////////////////////
// Filename: PlayerAbilities.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PLAYER_ABILITIES_H_
#define _PLAYER_ABILITIES_H_

//////////////
// INCLUDES //
//////////////
#include "cameraclass.h"

#include "world.h"
#include "entity.h"

#include "Point3d.h"

#include "Transform.h"
#include "PhysicBody.h"
#include "SphereCollider.h"
#include "Star.h"
#include "DelayedDestroy.h"
#include "SingleDamager.h"
#include "ParticleEmitter.h"
#include "Beam.h"
#include "PointCloud.h"

#include "CollisionManagerClass.h"


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

const DWORD CHARGE_START_DELTA = 500;
const float RAY_DISTANCE = 100;


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

	void Initialize(World*, CameraClass*, Entity*, CollisionManagerClass*);
	void Shutdown();
	void Update();

	void Attack(Transform, point3d);
	void Charging();

	void BlockStart();
	void BlockEnd();

private:
	World* world;
	CameraClass* camera;
	CollisionManagerClass* collisionManager;
	Entity* playerEntity;

	bool charging;
	bool chargeAnim;
	double chargeTimeAchor;

	float charge;
	float maxCharge;

	bool block;

	vector<Entity*> projectiles;

private:
	void CommonAttack(Transform, point3d);
	void ChargedAttack(Transform, point3d);
};

#endif
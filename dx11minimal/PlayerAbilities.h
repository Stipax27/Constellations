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
#include "Mesh.h"

#include "Transform2D.h"

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

#define CHARGE_START_DELTA 500
#define RAY_DISTANCE 100.0f
#define STAMINA_RESTORE_STEP 0.5f

#define ATTACK_COST 25.0f

#define TIMESTOP_STEP 0.15f


////////////////////////////////////////////////////////////////////////////////
// Class name: PlayerAbilities
////////////////////////////////////////////////////////////////////////////////


class PlayerAbilities
{
public:
	PlayerWeapons weapon;

	float stamina;
	float maxStamina;

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

	void TimestopStart();
	void TimestopEnd();

private:
	World* world;
	CameraClass* camera;
	CollisionManagerClass* collisionManager;

	Entity* playerEntity;
	Entity* worldFolder;

	bool charging;
	bool chargeAnim;
	double chargeTimeAchor;
	bool chargeDone;

	float charge;
	float maxCharge;

	bool block;

	bool timeStopped;
	float timestopProgress;

	vector<Entity*> projectiles;

private:
	void UpdateProjectiles();

	void CommonAttack(Transform, point3d);
	void ChargedAttack(Transform, point3d);

	Entity* FistsCommon(Transform, point3d);
	Entity* SwordCommon(Transform, point3d);
	Entity* BowCommon(Transform, point3d);

	Entity* FistsCharged(Transform, point3d);
	Entity* SwordCharged(Transform, point3d);
	Entity* BowCharged(Transform, point3d);
};

#endif
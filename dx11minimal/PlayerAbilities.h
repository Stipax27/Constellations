////////////////////////////////////////////////////////////////////////////////
// Filename: PlayerAbilities.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PLAYER_ABILITIES_H_
#define _PLAYER_ABILITIES_H_

//////////////
// INCLUDES //
//////////////
#include "singleton.h"
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
#include "Health.h"
#include "Mesh.h"
#include "Components/Nebula.h"

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

// Константы для щита
#define SHIELD_ACTIVATION_COST 10.0f
#define SHIELD_DAMAGE_MULTIPLIER 2.0f  // Множитель стоимости блокировки урона
#define SHIELD_COST_PER_SECOND 40.0f

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

	// Новые публичные методы для работы со щитом
	void ShieldStart();
	void ShieldEnd();
	bool TryBlockDamage(float damage);
	bool IsShieldActive() const { return shieldActive; }


	void ParticleVacuumStart();
	void ParticleVacuumEnd();
	void CreateBlueStar(float size = 1.0f);
	void BlowGasStart();
	void BlowGasEnd();


public:
	PlayerAbilities();
	PlayerAbilities(const PlayerAbilities&);
	~PlayerAbilities();

	void Initialize(Entity*);
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
	EntityStorage* entityStorage;
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

	// Новые переменные для щита
	bool shieldActive;
	Entity* shieldEntity;
	float shieldVisualIntensity;
	double shieldLastDamageTime;
	double lastShieldUpdateTime;
	double shieldStartTime;

	Entity* starEntity;
	Entity* currentParticles;
	bool burstActive;

	Entity* vacuumCenterEntity;
	double vacuumStartTime;   
	float maxStarSize;

	bool canBlowGas;          
	Entity* gasBurstEntity;
	float starMinSize;
	bool isBlowingGas;    
	double blowGasStartTime;
	float blowGasRate;

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

	CollisionInfo GetProjectileCollisionInfo(Entity*);
};

#endif
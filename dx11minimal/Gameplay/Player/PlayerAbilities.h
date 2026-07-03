////////////////////////////////////////////////////////////////////////////////
// Filename: PlayerAbilities.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PLAYER_ABILITIES_H_
#define _PLAYER_ABILITIES_H_

//////////////
// INCLUDES //
//////////////
#include "../../Engine/Lib/singleton.h"
#include "../../Engine/Camera/cameraclass.h"
#include "../../Engine/Mouse/mouseclass.h"

#include "../../Engine/ECS_Base/world.h"
#include "../../Engine/ECS_Base/entity.h"

#include "../../Engine/Types/Point3d.h"
#include "../../Engine/Utils/componentutils.h"

#include "../../Engine/BasicComponents/Transform.h"
#include "../../Engine/Physic/Movement/PhysicBody.h"
#include "../../Engine/Physic/Collision/SphereCollider.h"
#include "../../Engine/Render/Star.h"
#include "../../Engine/Compute/DelayedDestroy/DelayedDestroy.h"
#include "../../Engine/Compute/Combat/SingleDamager.h"
#include "../../Engine/Render/ParticleEmitter.h"
#include "../../Engine/Render/Beam.h"
#include "../../Engine/Render/PointCloud.h"
#include "../../Engine/Compute/Combat/Health.h"
#include "../../Engine/Compute/Combat/DamageBlocker.h"
#include "../../Engine/Render/Mesh/Mesh.h"

#include "../../Engine/Render/Nebula/Nebula.h"

#include "../../Engine/BasicComponents/Transform2D.h"
#include "../../Engine/Physic/Collision/CollisionManagerClass.h"

#include "Grabbable.h"
#include "attackTable.h"


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

#define RADAR_START_RADIUS 1.f
#define RADAR_FINAL_RADIUS 150.f


////////////////////////////////////////////////////////////////////////////////
// Class name: PlayerAbilities
////////////////////////////////////////////////////////////////////////////////


class PlayerAbilities : public ISingleton
{
public:
	PlayerWeapons weapon;
	Elements element;
	Entity* ExecutionObject;

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

	void StartRadar();
	
	PlayerAbilities();
	PlayerAbilities(const PlayerAbilities&);
	~PlayerAbilities();

	void Initialize() override;
	void Shutdown();
	void Update();

	void Attack(Transform, point3d);
	void Charging();

	void BlockStart();
	void BlockEnd();

	void Timestop();

	void Grab();
	void Execution();

	void Grap();

	

private:
	World* world;
	EntityStorage* entityStorage;
	CameraClass* camera;
	MouseClass* mouse;
	CollisionManagerClass* collisionManager;

	Entity* playerEntity;
	Entity* worldFolder;

	Entity* grabbedObject;
	

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
	bool isVacuum;
	Nebula* interactiveNebula;

	bool canBlowGas;          
	Entity* gasBurstEntity;
	float starMinSize;
	bool isBlowingGas;    
	double blowGasStartTime;
	float blowGasRate;

	Entity* radarEntity;
	float radarStartTime;
	float speedRadius;
	float radiusRad;
	bool isRadaring;

	std::vector<AttackDesc> attacks;

private:
	void UpdateProjectiles();

	void CommonAttack(Transform, point3d);
	void ChargedAttack(Transform, point3d);

	Entity* FistsCharged(Transform, point3d);
	Entity* SwordCharged(Transform, point3d);
	Entity* BowCharged(Transform, point3d);

	Nebula* FindNearestNebula();
};

#endif
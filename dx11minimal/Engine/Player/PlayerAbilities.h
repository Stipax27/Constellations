////////////////////////////////////////////////////////////////////////////////
// Filename: PlayerAbilities.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _PLAYER_ABILITIES_H_
#define _PLAYER_ABILITIES_H_

//////////////
// INCLUDES //
//////////////
#include "../Lib/isingleton.h"
#include "../Camera/cameraclass.h"

#include "../ECS_Base/world.h"
#include "../ECS_Base/entity.h"

#include "../Types/Point3d.h"
#include "../Utils/componentutils.h"

#include "../BasicComponents/Transform.h"
#include "../Physic/Movement/PhysicBody.h"
#include "../Physic/Collision/SphereCollider.h"
#include "../Render/Star.h"
#include "../Compute/DelayedDestroy/DelayedDestroy.h"
#include "../Compute/Combat/SingleDamager.h"
#include "../Render/ParticleEmitter.h"
#include "../Render/Beam.h"
#include "../Render/PointCloud.h"
#include "../Compute/Combat/Health.h"
#include "../Compute/Combat/DamageBlocker.h"
#include "../Render/Mesh/Mesh.h"

#include "../Render/Nebula/Nebula.h"

#include "../BasicComponents/Transform2D.h"
#include "../Physic/Collision/CollisionManagerClass.h"

#include "Grabbable.h"
#include "attackTable.h"


/////////////
// GLOBALS //
/////////////

#define CHARGE_START_DELTA 500
#define RAY_DISTANCE 1000.0f
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
	
public:
	PlayerAbilities();
	PlayerAbilities(const PlayerAbilities&);
	~PlayerAbilities();

	void Initialize(Entity*, EntityStorage*);
	void Shutdown();
	void Update();

	void Attack(Transform, point3d);

	void Timestop();

	void Grab();

	

private:
	World* world;
	EntityStorage* entityStorage;
	CameraClass* camera;
	CollisionManagerClass* collisionManager;

	Entity* playerEntity;
	Entity* worldFolder;

	Entity* grabbedObject;

	bool timeStopped;
	float timestopProgress;

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
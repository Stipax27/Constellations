#include "attackTable.h"

#include "../ECS_Base/entityStorage.h"

#include "../Render/Star.h"
#include "../Physic/Movement/PhysicBody.h"
#include "../Physic/Collision/SphereCollider.h"
#include "../Compute/Combat/SingleDamager.h"
#include "../Compute/DelayedDestroy/DelayedDestroy.h"


// FUNCTIONS //
/////////////////////////////////////////////////////////////////////

Entity* fists_air_start(EntityStorage* entityStorage, const Transform& startTransform, const point3d& direction)
{
	Entity* projectile = entityStorage->CreateEntity("PlayerProjectile");
	Transform* transform = projectile->AddComponent<Transform>();
	transform->position = startTransform.position;
	transform->mRotation = startTransform.mRotation;

	PhysicBody* physicBody = projectile->AddComponent<PhysicBody>();
	physicBody->airFriction = 0.0f;
	physicBody->velocity = direction.normalized() * 100.0f;

	Star* star = projectile->AddComponent<Star>();
	star->radius = 0.4f;
	star->color1 = point3d(0.9f, 1.0f, 0.99f);
	star->color2 = point3d(0.34f, 0.8f, 0.45f);
	star->crownColor = point3d(0.27f, 0.63f, 1.0f);

	SingleDamager* singleDamager = projectile->AddComponent<SingleDamager>();
	singleDamager->target = Fraction::Enemy;
	singleDamager->damage = 5.0f;

	SphereCollider* sphereCollider = projectile->AddComponent<SphereCollider>();
	sphereCollider->isTouchable = false;
	sphereCollider->radius = 0.4f;

	DelayedDestroy* delayedDestroy = projectile->AddComponent<DelayedDestroy>();
	delayedDestroy->lifeTime = 5000;

	return projectile;
}

void Update(Entity*) {

}

void End(Entity*) {

}

/////////////////////////////////////////////////////////////////////

AttackLogic PlayerAttackTable[3][4] =
{
	// Fists
	{
		// Air
		{fists_air_start, Update, End},

		// Fire
		{},

		// Water
		{},

		// Earth
		{}
	},

	// Sword
	{
		// Air
		{},

		// Fire
		{},

		// Water
		{},

		// Earth
		{}
	},

	// Bow
	{
		// Air
		{},

		// Fire
		{},

		// Water
		{},

		// Earth
		{}
	}
};
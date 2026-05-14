#include "attackTable.h"

#include "../ECS_Base/entityStorage.h"

#include "../Render/Star.h"
#include "../Render/Beam.h"
#include "../Render/ParticleEmitter.h"
#include "../Render/Mesh/Mesh.h"

#include "../Physic/Movement/PhysicBody.h"
#include "../Physic/Collision/SphereCollider.h"

#include "../Compute/Combat/SingleDamager.h"
#include "../Compute/Combat/Health.h"
#include "../Compute/DelayedDestroy/DelayedDestroy.h"

#include "../Utils/componentutils.h"
#include "../Lib/interp.h"


// FUNCTIONS //
/////////////////////////////////////////////////////////////////////

// FISTS

// NONE
AttackDesc fists_none_start(EntityStorage* entityStorage, const Transform& startTransform, const point3d& direction)
{
	// Hitbox

	Entity* hitbox = entityStorage->CreateEntity("FistHitbox");

	Transform* transform = hitbox->AddComponent<Transform>();
	transform->position = startTransform.position + startTransform.GetLookVector() * 5;
	transform->mRotation = startTransform.mRotation;

	SingleDamager* singleDamager = hitbox->AddComponent<SingleDamager>();
	singleDamager->target = Fraction::Enemy;
	singleDamager->damage = 5.0f;
	singleDamager->maxHitCount = -1;

	SphereCollider* sphereCollider = hitbox->AddComponent<SphereCollider>();
	sphereCollider->isTouchable = false;
	sphereCollider->radius = 3.0f;

	DelayedDestroy* delayedDestroy = hitbox->AddComponent<DelayedDestroy>();
	delayedDestroy->lifeTime = 100;

	// Visualisation

	Entity* effect = entityStorage->CreateEntity("FistHitEffect");

	transform = effect->AddComponent<Transform>();
	transform->position = startTransform.position + startTransform.GetLookVector() * 5;
	transform->mRotation = startTransform.mRotation;

	ParticleEmitter* particleEmitter = effect->AddComponent<ParticleEmitter>();
	particleEmitter->rate = 150;
	particleEmitter->lifetime = 500;
	particleEmitter->color = point3d(1.0f, 0.15f, 0.85f);
	particleEmitter->size = { 2.0f, 0.0f };
	particleEmitter->opacity = { 0.75f, 0.0f };
	particleEmitter->speed = { 20.0f, 0.0f };
	particleEmitter->spread = { PI, 0 };
	particleEmitter->isHeapEmit = true;
	particleEmitter->heapEmitRepeats = 1;
	particleEmitter->lastEmitTime = timer::currentTime - particleEmitter->heapEmitInterval;
	particleEmitter->emitDirection = EmitDirection::Right;

	delayedDestroy = effect->AddComponent<DelayedDestroy>();
	delayedDestroy->lifeTime = 500;

	return AttackDesc(effect, PlayerWeapons::Fists, Elements::None);
}

void fists_none_update(EntityStorage* entityStorage, Entity* entity) {
	return;
}

void fists_none_end(EntityStorage* entityStorage, Entity* entity, const CollisionInfo& info) {
	Entity* impact = entityStorage->CreateEntity("Impact", entityStorage->GetEntityByName("World"));

	Transform* transform = impact->AddComponent<Transform>();
	transform->position = info.position;

	ParticleEmitter* particleEmitter = impact->AddComponent<ParticleEmitter>();
	particleEmitter->rate = 150;
	particleEmitter->lifetime = 500;
	particleEmitter->color = point3d(1.0f, 0.15f, 0.85f);
	particleEmitter->size = { 2.0f, 0.0f };
	particleEmitter->opacity = { 1.0f, 0.0f };
	particleEmitter->speed = { 20.0f, 0.0f };
	particleEmitter->spread = { PI, PI };
	particleEmitter->isHeapEmit = true;
	particleEmitter->heapEmitRepeats = 1;
	particleEmitter->lastEmitTime = timer::currentTime - particleEmitter->heapEmitInterval;

	DelayedDestroy* delayedDestroy = impact->AddComponent<DelayedDestroy>();
	delayedDestroy->lifeTime = 1000;

	entity->Destroy();
}

// AIR
AttackDesc fists_air_start(EntityStorage* entityStorage, const Transform& startTransform, const point3d& direction)
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

	return AttackDesc(projectile, PlayerWeapons::Fists, Elements::Air);
}

void fists_air_update(EntityStorage* entityStorage, Entity* entity) {
	if (!entity->HasComponent<SingleDamager>()) {
		const CollisionInfo& info = GetProjectileCollisionInfo(entityStorage, entity);
		PlayerAttackTable[0][1].end(entityStorage, entity, info);
	}
}

void fists_air_end(EntityStorage* entityStorage, Entity* entity, const CollisionInfo& info) {
	Entity* airField = entityStorage->CreateEntity(AIR_FIELD, entityStorage->GetEntityByName("World"));

	Transform* transform = airField->AddComponent<Transform>();
	transform->position = info.position;

	SphereCollider* sphereCollider = airField->AddComponent<SphereCollider>();
	sphereCollider->isTouchable = false;
	sphereCollider->collisionGroup = CollisionFilter::Group::Projectile;
	sphereCollider->radius = 1.0f;

	interp::Animate(sphereCollider->radius, 5.0f, 10, interp::Curve::EaseOutQuad, airField);

	ParticleEmitter* particleEmitter = airField->AddComponent<ParticleEmitter>();
	particleEmitter->rate = 200;
	particleEmitter->lifetime = 10000;
	particleEmitter->color = point3d(0.55f, 1.0f, 1.0f);
	particleEmitter->size = { 1.0f, 5.0f };
	particleEmitter->opacity = { 0.1f, 0.0f };
	particleEmitter->speed = { 1.0f, 0.0f };
	particleEmitter->spread = { PI, PI };
	particleEmitter->isHeapEmit = true;
	particleEmitter->heapEmitRepeats = 1;
	particleEmitter->lastEmitTime = timer::currentTime - particleEmitter->heapEmitInterval;

	DelayedDestroy* delayedDestroy = airField->AddComponent<DelayedDestroy>();
	delayedDestroy->lifeTime = 10000;

	entity->Destroy();
}

// FIRE
AttackDesc fists_fire_start(EntityStorage* entityStorage, const Transform& startTransform, const point3d& direction)
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

	return AttackDesc(projectile, PlayerWeapons::Fists, Elements::Fire);
}

void fists_fire_update(EntityStorage* entityStorage, Entity* entity) {
	SphereCollider* sphereCollider = entity->GetComponent<SphereCollider>();

	for (int i = 0; i < sphereCollider->collisions.size(); i++) {
		const CollisionInfo& collision = sphereCollider->collisions[i];
		Entity* target = entityStorage->GetEntityById(collision.entityId);

		if (target->name == AIR_FIELD) {
			Entity* explosion = entityStorage->CreateEntity("Explosion", entityStorage->GetEntityByName("World"));

			Transform* transform = explosion->AddComponent<Transform>();
			transform->position = target->GetComponent<Transform>()->position;

			ParticleEmitter* particleEmitter = explosion->AddComponent<ParticleEmitter>();
			particleEmitter->rate = 150;
			particleEmitter->lifetime = 750;
			particleEmitter->color = point3d(0.87, 0.26, 0);
			particleEmitter->size = { 5.0f, 0.0f };
			particleEmitter->opacity = { 1.0f, 0.0f };
			particleEmitter->speed = { 20.0f, 0.0f };
			particleEmitter->spread = { PI, PI };
			particleEmitter->isHeapEmit = true;
			particleEmitter->heapEmitRepeats = 1;
			particleEmitter->lastEmitTime = timer::currentTime - particleEmitter->heapEmitInterval;

			SphereCollider* sphereCollider = explosion->AddComponent<SphereCollider>();
			sphereCollider->isTouchable = false;
			sphereCollider->collisionGroup = CollisionFilter::Group::Projectile;
			sphereCollider->radius = 2;

			interp::Animate(sphereCollider->radius, 7.0f, 0.5, interp::Curve::Linear, explosion);

			SingleDamager* singleDamager = explosion->AddComponent<SingleDamager>();
			singleDamager->target = Fraction::Enemy;
			singleDamager->damage = 25.0f;
			singleDamager->maxHitCount = -1;

			DelayedDestroy* delayedDestroy = explosion->AddComponent<DelayedDestroy>();
			delayedDestroy->lifeTime = 750;

			target->Destroy();

			break;
		}
	}

	if (!entity->HasComponent<SingleDamager>()) {
		const CollisionInfo& info = GetProjectileCollisionInfo(entityStorage, entity);
		PlayerAttackTable[0][2].end(entityStorage, entity, info);
	}
}

void fists_fire_end(EntityStorage* entityStorage, Entity* entity, const CollisionInfo& info) {
	Entity* impact = entityStorage->CreateEntity("Impact", entityStorage->GetEntityByName("World"));

	Transform* transform = impact->AddComponent<Transform>();
	transform->position = info.position;

	ParticleEmitter* particleEmitter = impact->AddComponent<ParticleEmitter>();
	particleEmitter->rate = 150;
	particleEmitter->lifetime = 250;
	particleEmitter->color = point3d(0.87, 0.42, 0);
	particleEmitter->size = { 2.0f, 0.0f };
	particleEmitter->opacity = { 1.0f, 0.0f };
	particleEmitter->speed = { 15.0f, 0.0f };
	particleEmitter->spread = { PI, PI };
	particleEmitter->isHeapEmit = true;
	particleEmitter->heapEmitRepeats = 1;
	particleEmitter->lastEmitTime = timer::currentTime - particleEmitter->heapEmitInterval;

	DelayedDestroy* delayedDestroy = impact->AddComponent<DelayedDestroy>();
	delayedDestroy->lifeTime = 1000;

	entity->Destroy();
}

// SWORD

AttackDesc sword_none_start(EntityStorage* entityStorage, const Transform& startTransform, const point3d& direction)
{
	Entity* projectile = entityStorage->CreateEntity("PlayerProjectile");
	Transform* transform = projectile->AddComponent<Transform>();
	transform->position = startTransform.position;
	transform->mRotation = startTransform.mRotation;

	PhysicBody* physicBody = projectile->AddComponent<PhysicBody>();
	physicBody->airFriction = 0.0f;
	physicBody->velocity = direction.normalized() * 100.0f;

	Beam* beam = projectile->AddComponent<Beam>();
	beam->point1 = point3d(6, 6, 0) * -0.4f;
	beam->point2 = point3d(6, 6, 0) * 0.4f;
	beam->size1 = 1.0f;
	beam->size2 = 1.0f;
	beam->color1 = point3d(1, 0.15f, 0.15f);
	beam->color2 = point3d(1, 0.15f, 0.15f);
	beam->opacity1 = 1.25f;
	beam->opacity2 = 1.25f;
	beam->pShader = 22;

	for (int i = -5; i < 6; i++) {
		float offset = (float)i * 0.4f;

		Entity* entity = entityStorage->CreateEntity("ProjectileCollider", projectile);
		Transform* transform = entity->AddComponent<Transform>();
		transform->position = point3d(offset, offset, 0);

		SphereCollider* sphereCollider = entity->AddComponent<SphereCollider>();
		sphereCollider->isTouchable = false;
		sphereCollider->radius = 0.4f;
	}

	SingleDamager* singleDamager = projectile->AddComponent<SingleDamager>();
	singleDamager->target = Fraction::Enemy;
	singleDamager->damage = 5.0f;
	singleDamager->maxHitCount = 3;

	DelayedDestroy* delayedDestroy = projectile->AddComponent<DelayedDestroy>();
	delayedDestroy->lifeTime = 5000;

	return AttackDesc(projectile, PlayerWeapons::Sword, Elements::None);
}

// BOW

AttackDesc bow_none_start(EntityStorage* entityStorage, const Transform& startTransform, const point3d& direction)
{
	Entity* projectile = entityStorage->CreateEntity("PlayerProjectile");
	Transform* transform = projectile->AddComponent<Transform>();
	transform->position = startTransform.position;
	transform->mRotation = GetMatrixBetweenLookVector(*transform, direction) * transform->mRotation;

	PhysicBody* physicBody = projectile->AddComponent<PhysicBody>();
	physicBody->airFriction = 0.0f;
	physicBody->velocity = direction.normalized() * 150.0f;

	Mesh* mesh = projectile->AddComponent<Mesh>();
	mesh->index = 1;

	ParticleEmitter* particleEmitter = projectile->AddComponent<ParticleEmitter>();
	particleEmitter->rate = 100;
	particleEmitter->lifetime = 1000;
	particleEmitter->color = point3d(0.15f, 0.95f, 0.35f);
	particleEmitter->size = { 1.5f, 5.0f };
	particleEmitter->opacity = { 0.75f, 0.0f };
	particleEmitter->emitDirection = EmitDirection::Front;
	particleEmitter->speed = { 25.0f, -5.0f };

	SingleDamager* singleDamager = projectile->AddComponent<SingleDamager>();
	singleDamager->target = Fraction::Enemy;
	singleDamager->damage = 15.0f;
	singleDamager->maxHitCount = 5;

	SphereCollider* sphereCollider = projectile->AddComponent<SphereCollider>();
	sphereCollider->isTouchable = false;
	sphereCollider->radius = 0.4f;

	DelayedDestroy* delayedDestroy = projectile->AddComponent<DelayedDestroy>();
	delayedDestroy->lifeTime = 5000;

	return AttackDesc(projectile, PlayerWeapons::Bow, Elements::None);
}

/////////////////////////////////////////////////////////////////////

AttackLogic PlayerAttackTable[3][5] =
{
	// Fists
	{
		// None
		{fists_none_start, fists_none_update, fists_none_end},

		// Air
		//{fists_air_start, fists_air_update, fists_air_end},
		{},

		// Fire
		//{fists_fire_start, fists_fire_update, fists_fire_end},
		{},

		// Water
		{},

		// Earth
		{}
	},

	// Sword
	{
		// None
		//{sword_none_start, fists_none_update, fists_none_end},
		{},

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
		// None
		//{bow_none_start, fists_none_update, fists_none_end},
		{},

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
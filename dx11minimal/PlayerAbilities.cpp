#include "PlayerAbilities.h"


PlayerAbilities::PlayerAbilities()
{
}

PlayerAbilities::PlayerAbilities(const PlayerAbilities& other)
{
}


PlayerAbilities::~PlayerAbilities()
{
}


void PlayerAbilities::Initialize(World* m_World)
{
	weapon = PlayerWeapons::Fists;

	world = m_World;
}


void PlayerAbilities::Shutdown()
{
	if (world) {
		world = 0;
	}
}


void PlayerAbilities::Attack(Transform startTransform, point3d direction)
{
	switch (weapon) {
	case PlayerWeapons::Fists:
	{
		Entity* projectile = world->entityStorage->CreateEntity("PlayerProjectile");
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

		break;
	}
	case PlayerWeapons::Sword:
	{
		Entity* projectile = world->entityStorage->CreateEntity("PlayerProjectile");
		Transform* transform = projectile->AddComponent<Transform>();
		transform->position = startTransform.position;
		transform->mRotation = startTransform.mRotation;

		PhysicBody* physicBody = projectile->AddComponent<PhysicBody>();
		physicBody->airFriction = 0.0f;
		physicBody->velocity = direction.normalized() * 100.0f;

		for (int i = -5; i < 6; i++) {
			float offset = (float)i * 0.4f;

			Entity* entity = world->entityStorage->CreateEntity("ProjectileCollider", projectile);
			Transform* transform = entity->AddComponent<Transform>();
			transform->position = point3d(offset, offset, 0);

			/*Star* star = entity->AddComponent<Star>();
			star->radius = 0.4f;
			star->color1 = point3d(0.9f, 1.0f, 0.99f);
			star->color2 = point3d(0.34f, 0.8f, 0.45f);
			star->crownColor = point3d(0.27f, 0.63f, 1.0f);*/

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

		break;
	}
	case PlayerWeapons::Bow:
	{
		Entity* projectile = world->entityStorage->CreateEntity("PlayerProjectile");
		Transform* transform = projectile->AddComponent<Transform>();
		transform->position = startTransform.position;
		transform->mRotation = startTransform.mRotation;

		PhysicBody* physicBody = projectile->AddComponent<PhysicBody>();
		physicBody->airFriction = 0.0f;
		physicBody->velocity = direction.normalized() * 150.0f;

		Star* star = projectile->AddComponent<Star>();
		star->radius = 0.4f;
		star->color1 = point3d(0.9f, 1.0f, 0.99f);
		star->color2 = point3d(0.34f, 0.8f, 0.45f);
		star->crownColor = point3d(0.27f, 0.63f, 1.0f);

		ParticleEmitter* particleEmitter = projectile->AddComponent<ParticleEmitter>();
		particleEmitter->rate = 100;
		particleEmitter->lifetime = 500;
		particleEmitter->color = point3d(1.0f, 0.25f, 0.75f);
		particleEmitter->size = { 1.5f, 5.0f };
		particleEmitter->opacity = { 0.75f, 0.0f };

		SingleDamager* singleDamager = projectile->AddComponent<SingleDamager>();
		singleDamager->target = Fraction::Enemy;
		singleDamager->damage = 15.0f;
		singleDamager->maxHitCount = 5;

		SphereCollider* sphereCollider = projectile->AddComponent<SphereCollider>();
		sphereCollider->isTouchable = false;
		sphereCollider->radius = 0.4f;

		DelayedDestroy* delayedDestroy = projectile->AddComponent<DelayedDestroy>();
		delayedDestroy->lifeTime = 5000;

		break;
	}
	}
}
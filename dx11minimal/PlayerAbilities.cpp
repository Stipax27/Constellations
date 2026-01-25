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
		Entity* projectile = world->CreateEntity("PlayerProjectile");
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

		SingleDamageGiver* singleDamageGiver = projectile->AddComponent<SingleDamageGiver>();
		singleDamageGiver->target = Fraction::Enemy;
		singleDamageGiver->damage = 5.0f;

		SphereCollider* sphereCollider = projectile->AddComponent<SphereCollider>();
		sphereCollider->isTouchable = false;
		sphereCollider->radius = 0.4f;

		DelayedDestroy* delayedDestroy = projectile->AddComponent<DelayedDestroy>();
		delayedDestroy->lifeTime = 5000;

		break;
	}
	case PlayerWeapons::Sword:
	{
		Entity* projectile = world->CreateEntity("PlayerProjectile");
		Transform* transform = projectile->AddComponent<Transform>();
		transform->position = startTransform.position;
		transform->mRotation = startTransform.mRotation;

		PhysicBody* physicBody = projectile->AddComponent<PhysicBody>();
		physicBody->airFriction = 0.0f;
		physicBody->velocity = direction.normalized() * 100.0f;

		for (int i = -5; i < 6; i++) {
			float offset = (float)i * 0.4f;

			Entity* entity = world->CreateEntity("ProjectileCollider", projectile);
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

		SingleDamageGiver* singleDamageGiver = projectile->AddComponent<SingleDamageGiver>();
		singleDamageGiver->target = Fraction::Enemy;
		singleDamageGiver->damage = 5.0f;

		DelayedDestroy* delayedDestroy = projectile->AddComponent<DelayedDestroy>();
		delayedDestroy->lifeTime = 5000;

		break;
	}
	case PlayerWeapons::Bow:
	{
		break;
	}
	}
}
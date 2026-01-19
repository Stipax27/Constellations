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

		projectile = 0;
		transform = 0;
		physicBody = 0;
		star = 0;
		singleDamageGiver = 0;
		sphereCollider = 0;
		delayedDestroy = 0;

		break;
	}
	case PlayerWeapons::Sword:
	{
		break;
	}
	case PlayerWeapons::Bow:
	{
		break;
	}
	}
}
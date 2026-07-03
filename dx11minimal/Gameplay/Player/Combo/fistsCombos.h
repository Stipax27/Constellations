#ifndef _FISTS_COMBOS_H_
#define _FISTS_COMBOS_H_

#include "../../../Engine/ECS_Base/entity.h"
#include "../../../Engine/ECS_Base/entityStorage.h"
#include "../../../Engine/BasicComponents/Transform.h"
#include "../../../Engine/Compute/Combat/SingleDamager.h"
#include "../../../Engine/Physic/Collision/SphereCollider.h"
#include "../../../Engine/Render/ParticleEmitter.h"
#include "../../../Engine/Compute/DelayedDestroy/DelayedDestroy.h"
#include "../../../Engine/Sound/SoundPlayer.h"


namespace fistsCombos
{

	void first_punch_l(EntityStorage* entityStorage, Entity* player, const point3d& direction)
	{
		Transform startTransform = GetWorldTransform(player);

		// Hitbox

		Entity* hitbox = entityStorage->CreateEntity("FistHitbox");

		Transform* transform = hitbox->AddComponent<Transform>();
		transform->position = startTransform.position + startTransform.GetLookVector() * 5;
		transform->mRotation = startTransform.mRotation;

		SingleDamager* singleDamager = hitbox->AddComponent<SingleDamager>();
		singleDamager->target = Fraction::Enemy;
		singleDamager->damage = 50.0f;
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

		SoundPlayer* soundPlayer = effect->AddComponent<SoundPlayer>();
		soundPlayer->soundName = "punch-swing";
		soundPlayer->playing = true;

		delayedDestroy = effect->AddComponent<DelayedDestroy>();
		delayedDestroy->lifeTime = 1000;
	}


	void final_punch_l(EntityStorage* entityStorage, Entity* player, const point3d& direction)
	{
		Transform startTransform = GetWorldTransform(player);

		// Hitbox

		Entity* hitbox = entityStorage->CreateEntity("FistHitbox");

		Transform* transform = hitbox->AddComponent<Transform>();
		transform->position = startTransform.position + startTransform.GetLookVector() * 5;
		transform->mRotation = startTransform.mRotation;

		SingleDamager* singleDamager = hitbox->AddComponent<SingleDamager>();
		singleDamager->target = Fraction::Enemy;
		singleDamager->damage = 8.0f;
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
		particleEmitter->color = point3d(1, 0, 0);
		particleEmitter->size = { 2.0f, 0.0f };
		particleEmitter->opacity = { 0.75f, 0.0f };
		particleEmitter->speed = { 25.0f, 0.0f };
		particleEmitter->spread = { PI, 0 };
		particleEmitter->isHeapEmit = true;
		particleEmitter->heapEmitRepeats = 1;
		particleEmitter->lastEmitTime = timer::currentTime - particleEmitter->heapEmitInterval;
		particleEmitter->emitDirection = EmitDirection::Right;

		SoundPlayer* soundPlayer = effect->AddComponent<SoundPlayer>();
		soundPlayer->soundName = "punch-swing";
		soundPlayer->playing = true;

		delayedDestroy = effect->AddComponent<DelayedDestroy>();
		delayedDestroy->lifeTime = 1000;
	}
}

#endif _FISTS_COMBOS_H_
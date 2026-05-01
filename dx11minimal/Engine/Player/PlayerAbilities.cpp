#include "PlayerAbilities.h"

#include "../Lib/timer.h"

using namespace std;


PlayerAbilities::PlayerAbilities()
{
}

PlayerAbilities::PlayerAbilities(const PlayerAbilities& other)
{
}

PlayerAbilities::~PlayerAbilities()
{
}


void PlayerAbilities::Initialize(Entity* PlayerEntity, EntityStorage* storage)
{
	weapon = PlayerWeapons::Fists;
	element = Elements::None;

	world = Singleton::GetInstance<World>();
	entityStorage = storage;
	camera = Singleton::GetInstance<CameraClass>();
	collisionManager = Singleton::GetInstance<CollisionManagerClass>();

	playerEntity = PlayerEntity;
	worldFolder = entityStorage->GetEntityByName("World");

	// Добавляем для отслеживания времени
	timeStopped = false;
	timestopProgress = 1;
}

	


void PlayerAbilities::Shutdown()
{
	if (world) {
		world = nullptr;
	}

	if (camera) {
		camera = nullptr;
	}

	if (collisionManager) {
		collisionManager = nullptr;
	}

	if (playerEntity) {
		playerEntity = nullptr;
	}

	attacks.clear();
}

void PlayerAbilities::Update()
{
	if (timeStopped) {
		timestopProgress = max(timestopProgress - TIMESTOP_STEP * (timer::deltaTime * 0.01), 0);
		worldFolder->SetTimeScale(timestopProgress);
	}
	else if (timestopProgress < 1.0f) {
		timestopProgress = min(timestopProgress + TIMESTOP_STEP * (timer::deltaTime * 0.01), 1);
		worldFolder->SetTimeScale(timestopProgress);
	}
	UpdateProjectiles();
}


void PlayerAbilities::Attack(Transform startTransform, point3d direction)
{
	RayInfo rayInfo = RayInfo(camera->position, direction * RAY_DISTANCE, CollisionFilter::Group::PlayerRay, false);
	RaycastResult result = collisionManager->Raycast(rayInfo);

	point3d pos = result.hit ? result.position : camera->position + direction * RAY_DISTANCE;
	point3d dir = (pos - startTransform.position).normalized();

	CommonAttack(startTransform, dir);
}


void PlayerAbilities::CommonAttack(Transform startTransform, point3d direction)
{
	AttackDesc attackDesc = PlayerAttackTable[(int)weapon][(int)element].start(entityStorage, startTransform, direction);
	attacks.push_back(attackDesc);
}


void PlayerAbilities::Timestop()
{
	timeStopped = not timeStopped;
}


void PlayerAbilities::Grab()
{
	if (grabbedObject == nullptr) {

		Entity* grabHitbox = playerEntity->GetChildByName("GrabHitbox");
		SphereCollider* grabCollider = grabHitbox->GetComponent<SphereCollider>();

		CollisionInfo result = GetCollisionWithComponent<Grabbable>(grabCollider);
		if (result.entityId >= 0) {
			grabbedObject = entityStorage->GetEntityById(result.entityId);
			if (grabbedObject != nullptr) {
				Transform* grabbedTransform = grabbedObject->GetComponent<Transform>();

				Entity* infoReserve = entityStorage->CreateEntity("infoReserve", grabbedObject);
				infoReserve->SetActive(false);
				Transform* transformInfo = infoReserve->AddComponent<Transform>();
				transformInfo = grabbedTransform;

				Transform relative = GetRelativeTransform(GetWorldTransform(playerEntity), GetWorldTransform(grabbedObject));
				grabbedObject->SetParent(playerEntity);

				grabbedTransform->position = relative.position;
				grabbedTransform->mRotation = relative.mRotation;
				grabbedTransform->scale = relative.scale;
			}
		}

	}
	else {

		Transform wt = GetWorldTransform(grabbedObject);
		grabbedObject->SetParent(worldFolder);

		Transform* grabbedTransform = grabbedObject->GetComponent<Transform>();
		grabbedTransform->position = wt.position;
		grabbedTransform->mRotation = wt.mRotation;
		grabbedTransform->scale = wt.scale;

		grabbedObject->GetChildByName("infoReserve")->Destroy();
		grabbedObject = 0;

	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////


void PlayerAbilities::UpdateProjectiles()
{
	int i = 0;
	while (i < attacks.size())
	{
		AttackDesc attack = attacks[i];
		Entity* projectile = attack.entity;

		if (IsEntityValid(projectile) && !projectile->IsDeleting() && projectile->GetTimeScale() > 0) {
			PlayerAttackTable[attack.weapon][attack.element].update(entityStorage, projectile);
		}

		if (!IsEntityValid(projectile))
		{
			attacks.erase(attacks.begin() + i);
		}
		else
		{
			i++;

			if (timeStopped) {
				projectile->SetTimeScale(max(projectile->GetLocalTimeScale() - TIMESTOP_STEP * (timer::deltaTime * 0.01), 0));
			}
			else {
				projectile->SetTimeScale(min(projectile->GetLocalTimeScale() + TIMESTOP_STEP * (timer::deltaTime * 0.01), 1));
			}
		}
	}
}


Entity* PlayerAbilities::FistsCharged(Transform startTransform, point3d direction)
{
	Entity* projectile = entityStorage->CreateEntity("PlayerProjectile");
	Transform* transform = projectile->AddComponent<Transform>();
	transform->position = startTransform.position;
	transform->mRotation = startTransform.mRotation;

	PhysicBody* physicBody = projectile->AddComponent<PhysicBody>();
	physicBody->airFriction = 0.0f;
	physicBody->velocity = direction.normalized() * 100.0f;

	Star* star = projectile->AddComponent<Star>();
	star->radius = 0.6f;
	star->crownRadius = 1.5f;
	star->color1 = point3d(1.0f, 0.8f, 0.7f);
	star->color2 = point3d(0.7f, 0.3f, 0.2f);
	star->crownColor = point3d(0.74f, 0.35f, 0.15f);

	SingleDamager* singleDamager = projectile->AddComponent<SingleDamager>();
	singleDamager->target = Fraction::Enemy;
	singleDamager->damage = 15.0f;

	SphereCollider* sphereCollider = projectile->AddComponent<SphereCollider>();
	sphereCollider->isTouchable = false;
	sphereCollider->radius = 0.6f;

	DelayedDestroy* delayedDestroy = projectile->AddComponent<DelayedDestroy>();
	delayedDestroy->lifeTime = 5000;

	return projectile;
}


Entity* PlayerAbilities::SwordCharged(Transform startTransform, point3d direction)
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
	beam->color1 = point3d(1, 0.35f, 1);
	beam->color2 = point3d(1, 0.35f, 1);
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
	singleDamager->damage = 20.0f;
	singleDamager->maxHitCount = 3;

	DelayedDestroy* delayedDestroy = projectile->AddComponent<DelayedDestroy>();
	delayedDestroy->lifeTime = 5000;

	return projectile;
}


Entity* PlayerAbilities::BowCharged(Transform startTransform, point3d direction)
{
	Entity* projectile = entityStorage->CreateEntity("PlayerProjectile");
	Transform* transform = projectile->AddComponent<Transform>();
	transform->position = startTransform.position;
	transform->mRotation = GetMatrixFromLookVector(*transform, direction) * transform->mRotation;

	PhysicBody* physicBody = projectile->AddComponent<PhysicBody>();
	physicBody->airFriction = 0.0f;
	physicBody->velocity = direction.normalized() * 150.0f;

	Mesh* mesh = projectile->AddComponent<Mesh>();
	mesh->index = 1;

	ParticleEmitter* particleEmitter = projectile->AddComponent<ParticleEmitter>();
	particleEmitter->rate = 100;
	particleEmitter->lifetime = 1000;
	particleEmitter->color = point3d(0.95f, 0.25f, 0.1f);
	particleEmitter->size = { 1.5f, 5.0f };
	particleEmitter->opacity = { 0.75f, 0.0f };
	particleEmitter->emitDirection = EmitDirection::Front;
	particleEmitter->speed = { 25.0f, -5.0f };

	SingleDamager* singleDamager = projectile->AddComponent<SingleDamager>();
	singleDamager->target = Fraction::Enemy;
	singleDamager->damage = 45.0f;
	singleDamager->maxHitCount = 5;

	SphereCollider* sphereCollider = projectile->AddComponent<SphereCollider>();
	sphereCollider->isTouchable = false;
	sphereCollider->radius = 0.4f;

	DelayedDestroy* delayedDestroy = projectile->AddComponent<DelayedDestroy>();
	delayedDestroy->lifeTime = 5000;

	return projectile;
}


Nebula* PlayerAbilities::FindNearestNebula()
{
	if (!playerEntity || !world) return nullptr;

	
	SphereCollider* sphereCollider = playerEntity->GetComponent<SphereCollider>();
	if (!sphereCollider) return nullptr;

	for (CollisionInfo info : sphereCollider->collisions) {
		Entity* entity = entityStorage->GetEntityById(info.entityId);
		if (!IsEntityValid(entity)) {
			continue;
		}

		Nebula* nebula = entity->GetComponentInAncestor<Nebula>();
		if (nebula != nullptr && nebula->active && nebula->isInteractive) {
			return nebula;
		}
	}

	return nullptr;
}
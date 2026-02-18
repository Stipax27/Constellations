#include "PlayerAbilities.h" //


PlayerAbilities::PlayerAbilities()
{
}

PlayerAbilities::PlayerAbilities(const PlayerAbilities& other)
{
}


PlayerAbilities::~PlayerAbilities()
{
}


void PlayerAbilities::Initialize(World* m_World, CameraClass* Camera, Entity* PlayerEntity, CollisionManagerClass* CollisionManager)
{
	weapon = PlayerWeapons::Fists;

	world = m_World;
	camera = Camera;
	collisionManager = CollisionManager;
	playerEntity = PlayerEntity;

	maxStamina = 1000;
	stamina = maxStamina;

	charging = false;
	chargeDone = false;
	charge = 0;
	maxCharge = 100;
}


void PlayerAbilities::Shutdown()
{
	if (world) {
		world = 0;
	}

	if (camera) {
		camera = 0;
	}

	if (collisionManager) {
		collisionManager = 0;
	}

	if (playerEntity) {
		playerEntity = 0;
	}
}


void PlayerAbilities::Update()
{
	stamina = clamp(stamina + STAMINA_RESTORE_STEP, 0, maxStamina);

	if (charging) {
		double delta = timer::currentTime - chargeTimeAchor;
		if (delta >= CHARGE_START_DELTA) {
			camera->SetFov(lerp(camera->GetFov(), 90, 0.025f));
			camera->distance = (DEFAULT_CAMERA_DISTANCE - 2.5f) * (tan(DEFAULT_FOV / camera->GetFov()));

			if (delta - CHARGE_START_DELTA > 500) {
				chargeDone = true;
			}
		}
	}
	else if (chargeAnim) {
		if (camera->GetFov() - DEFAULT_FOV > 0.1f) {
			camera->SetFov(lerp(camera->GetFov(), DEFAULT_FOV, 0.1f));
			camera->distance = (DEFAULT_CAMERA_DISTANCE - 2.5f) * (tan(DEFAULT_FOV / camera->GetFov()));
		}
		else {
			chargeAnim = false;
			camera->SetFov(DEFAULT_FOV);
			camera->distance = DEFAULT_CAMERA_DISTANCE;
		}
	}

	for (Entity* entity : projectiles) {
		
	}
}


void PlayerAbilities::Attack(Transform startTransform, point3d direction)
{
	if (block) {
		return;
	}

	charging = false;
	chargeTimeAchor = timer::currentTime;

	if (!chargeDone) {
		if (stamina < ATTACK_COST) {
			return;
		}
		stamina -= ATTACK_COST;
	}

	RayInfo rayInfo = RayInfo(camera->GetPosition(), direction * RAY_DISTANCE, CollisionFilter::Group::PlayerRay);
	RaycastResult result = collisionManager->Raycast(rayInfo);

	point3d pos = result.hit ? result.position : camera->GetPosition() + direction * RAY_DISTANCE;
	point3d dir = (pos - startTransform.position).normalized();

	if (chargeDone) {
		ChargedAttack(startTransform, dir);
	}
	else {
		CommonAttack(startTransform, dir);
	}
	chargeDone = false;
}


void PlayerAbilities::Charging()
{
	if (block) {
		return;
	}

	if (!charging) {
		charging = true;
		chargeAnim = true;
		chargeTimeAchor = timer::currentTime;
	}
}


void PlayerAbilities::CommonAttack(Transform startTransform, point3d direction)
{
	switch (weapon) {
	case PlayerWeapons::Fists:
	{
		FistsCommon(startTransform, direction);
		break;
	}
	case PlayerWeapons::Sword:
	{
		SwordCommon(startTransform, direction);
		break;
	}
	case PlayerWeapons::Bow:
	{
		BowCommon(startTransform, direction);
		break;
	}
	}
}


void PlayerAbilities::ChargedAttack(Transform startTransform, point3d direction)
{
	switch (weapon) {
	case PlayerWeapons::Fists:
	{
		FistsCharged(startTransform, direction);
		break;
	}
	case PlayerWeapons::Sword:
	{
		SwordCharged(startTransform, direction);
		break;
	}
	case PlayerWeapons::Bow:
	{
		BowCharged(startTransform, direction);
		break;
	}
	}
}


void PlayerAbilities::BlockStart()
{
	if (charging) {
		return;
	}

	block = true;

	PointCloud* pointCloud = playerEntity->GetComponent<PointCloud>();
	pointCloud->color = point3d(1, 1, 0);
}


void PlayerAbilities::BlockEnd()
{
	block = false;

	PointCloud* pointCloud = playerEntity->GetComponent<PointCloud>();
	pointCloud->color = point3d(1, 0.6f, 0.9f);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////


void PlayerAbilities::FistsCommon(Transform startTransform, point3d direction)
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
}


void PlayerAbilities::SwordCommon(Transform startTransform, point3d direction)
{
	Entity* projectile = world->entityStorage->CreateEntity("PlayerProjectile");
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

		Entity* entity = world->entityStorage->CreateEntity("ProjectileCollider", projectile);
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
}


void PlayerAbilities::BowCommon(Transform startTransform, point3d direction)
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
}


void PlayerAbilities::FistsCharged(Transform startTransform, point3d direction)
{
	Entity* projectile = world->entityStorage->CreateEntity("PlayerProjectile");
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
}


void PlayerAbilities::SwordCharged(Transform startTransform, point3d direction)
{
	Entity* projectile = world->entityStorage->CreateEntity("PlayerProjectile");
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

		Entity* entity = world->entityStorage->CreateEntity("ProjectileCollider", projectile);
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
}


void PlayerAbilities::BowCharged(Transform startTransform, point3d direction)
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
	star->color1 = point3d(1.0f, 0.9f, 0.85f);
	star->color2 = point3d(0.7f, 0.5f, 0.45f);
	star->crownColor = point3d(0.74f, 0.23f, 0.35f);

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
}
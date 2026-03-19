#include "PlayerAbilities.h"
#include "timer.h"

PlayerAbilities::PlayerAbilities()
{
}

PlayerAbilities::PlayerAbilities(const PlayerAbilities& other)
{
}

PlayerAbilities::~PlayerAbilities()
{
}


void PlayerAbilities::Initialize(Entity* PlayerEntity)
{
	weapon = PlayerWeapons::Fists;

	world = Singleton::GetInstance<World>();
	entityStorage = Singleton::GetInstance<EntityStorage>();
	camera = Singleton::GetInstance<CameraClass>();
	collisionManager = Singleton::GetInstance<CollisionManagerClass>();

	playerEntity = PlayerEntity;
	worldFolder = entityStorage->GetEntityByName("World");

	maxStamina = 1000;
	stamina = maxStamina;

	charging = false;
	chargeAnim = false;
	chargeDone = false;
	charge = 0;
	maxCharge = 100;

	block = false;

	// Инициализация переменных щита
	shieldActive = false;
	shieldEntity = nullptr;
	shieldVisualIntensity = 0.0f;
	shieldLastDamageTime = 0.0f;
	shieldStartTime = 0.0;
	// Вакуум и звезда
	starEntity = nullptr;
	vacuumCenterEntity = nullptr;  
	currentParticles = nullptr;
	burstActive = false;
	vacuumStartTime = 0;        
	maxStarSize = 3.0f;
	// Выдувание и уменьшение звезды 
	canBlowGas = false;  
	gasBurstEntity = nullptr;
	starMinSize = 0.3f;
	isBlowingGas = false;  
	blowGasStartTime = 0;
	blowGasRate = 0.01f;

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

	// Очистка щита
	if (shieldEntity) {
		shieldEntity = nullptr;
	}

	// Очистка проектайлов
	for (Entity* entity : projectiles) {
	}
	projectiles.clear();
}

void PlayerAbilities::Update()
{

	if (vacuumCenterEntity && !vacuumCenterEntity->IsDeleting()) {
		Star* centerStar = vacuumCenterEntity->GetComponent<Star>();
		if (centerStar) {
			// Расчет времени удержания
			float holdTime = (float)(timer::currentTime - vacuumStartTime) / 1000.0f;
			holdTime = min(holdTime, 3.0f);

		
			float baseSize = 0.1f + holdTime * 0.63f;

		
			float pulse = 1.0f + 0.2f * sin(timer::currentTime * 0.01f);

		
			centerStar->radius = baseSize * pulse;
			centerStar->crownRadius = baseSize * 1.5f * pulse;
		}
	}


	if (starEntity && !starEntity->IsDeleting()) {
		Star* star = starEntity->GetComponent<Star>();
		if (star) {
			
			if (isBlowingGas) {
				float blowTime = (float)(timer::currentTime - blowGasStartTime) / 1000.0f;
				float sizeReduction = blowGasRate * blowTime;

				float newSize = star->baseRadius - sizeReduction;

				// Проверяем минимальный размер
				if (newSize <= starMinSize) {
					starEntity->Destroy();
					starEntity = nullptr;
					canBlowGas = false;
					isBlowingGas = false;
					return;
				}

				
				star->baseRadius = newSize;
				star->baseCrownRadius = newSize * 1.5f;
			}

			
			float pulse = 1.0f + 0.2f * sin(timer::currentTime * 0.005f);
			star->radius = star->baseRadius * pulse;
			star->crownRadius = star->baseCrownRadius * pulse;
		}

		
		Transform* starTransform = starEntity->GetComponent<Transform>();
		if (starTransform) {
			starTransform->mRotation = starTransform->mRotation *
				XMMatrixRotationY(0.01f * timer::deltaTime);
		}

		
		canBlowGas = true;
	}
	else {
		canBlowGas = false;
		isBlowingGas = false;
	}


	if (gasBurstEntity && !gasBurstEntity->IsDeleting()) {
		Transform* gasTransform = gasBurstEntity->GetComponent<Transform>();
		Transform* playerTransform = playerEntity->GetComponent<Transform>();
		if (gasTransform && playerTransform) {
			
			gasTransform->position = playerTransform->position +
				playerTransform->GetLookVector() * 2.0f +
				playerTransform->GetUpVector() * 1.5f;

			
			gasTransform->mRotation = playerTransform->mRotation;
		}

	
		if (!isBlowingGas) {
			gasBurstEntity->AddComponent<DelayedDestroy>()->lifeTime = 500;
			gasBurstEntity = nullptr;
		}
	}

	// Восстановление выносливости ТОЛЬКО если щит не активен
	if (!shieldActive) {
		stamina = clamp(stamina + STAMINA_RESTORE_STEP, 0.0f, maxStamina);
	}

	// Логика щита - непрерывное отнятие стамины
	if (shieldActive) {
		// Рассчитываем время с последнего обновления
		double currentTime = timer::currentTime;
		double deltaTime = currentTime - lastShieldUpdateTime;

		// Обновляем каждые 16.67 мс (60 FPS) для плавности
		if (deltaTime > 16.67) {
			// Конвертируем в секунды для расчета (16.67 мс = 0.01667 секунды)
			float secondsPassed = deltaTime / 1000.0f;

			// Рассчитываем стоимость за прошедшее время
			float costThisFrame = SHIELD_COST_PER_SECOND * secondsPassed;

			// Отнимаем стамину
			stamina = clamp(stamina - costThisFrame, 0.0f, maxStamina);

			lastShieldUpdateTime = currentTime;
		}

		// Проверяем не кончилась ли стамина
		if (stamina <= 0.0f) {
			ShieldEnd(); // Автоматически выключаем щит если кончилась энергия
			return; // Выходим, чтобы не выполнять остальную логику щита
		}

		// Визуальный эффект - делаем щит мерцающим когда мало энергии
		if (shieldEntity) {
			Star* shieldStar = shieldEntity->GetComponent<Star>();
			if (shieldStar) {
				// Обновляем позицию щита (следует за игроком)
				Transform* playerTransform = playerEntity->GetComponent<Transform>();
				Transform* shieldTransform = shieldEntity->GetComponent<Transform>();
				if (playerTransform && shieldTransform) {
					shieldTransform->position = playerTransform->position;
				}

				// Эффект при низкой энергии
				if (stamina < maxStamina * 0.2f) {
					// Мигаем красным
					float intensity = 0.5f + 0.5f * sin(timer::currentTime * 0.01f);
					shieldStar->color1 = point3d(1.0f, intensity, intensity);
					shieldStar->color2 = point3d(1.0f, intensity * 0.5f, intensity * 0.5f);
				}
				else {
					// Нормальный цвет
					shieldStar->color1 = point3d(0.0f, 0.8f, 1.0f);
					shieldStar->color2 = point3d(0.0f, 0.4f, 0.8f);
				}
			}
		}
	}

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

// Метод для активации щита
void PlayerAbilities::ShieldStart()
{
	if (charging || block || shieldActive) {
		return; // Нельзя активировать щит во время других действий
	}

	// Проверяем хватает ли энергии для активации
	if (stamina < SHIELD_ACTIVATION_COST) {
		return;
	}

	shieldActive = true;
	stamina -= SHIELD_ACTIVATION_COST;

	// Запоминаем время активации для отслеживания расхода стамины
	lastShieldUpdateTime = timer::currentTime;
	shieldStartTime = timer::currentTime;

	// Визуальные изменения игрока
	PointCloud* pointCloud = playerEntity->GetComponent<PointCloud>();
	if (pointCloud) {
		pointCloud->color = point3d(0.0f, 0.8f, 1.0f); // Голубой цвет для щита
	}
	else if (timestopProgress < 1.0f) {
		timestopProgress = min(timestopProgress + TIMESTOP_STEP * (timer::deltaTime * 0.01), 1);
		worldFolder->SetTimeScale(timestopProgress);
	}

	// Создаем визуальный эффект щита
	shieldEntity = entityStorage->CreateEntity("PlayerShield");

	// Делаем щит дочерним объектом игрока
	Transform* playerTransform = playerEntity->GetComponent<Transform>();
	Transform* shieldTransform = shieldEntity->AddComponent<Transform>();
	shieldTransform->position = playerTransform->position;

	// Добавляем визуальный компонент
	Star* shieldStar = shieldEntity->AddComponent<Star>();
	shieldStar->radius = 2.0f;
	shieldStar->crownRadius = 2.5f;
	shieldStar->color1 = point3d(0.0f, 0.8f, 1.0f);
	shieldStar->color2 = point3d(0.0f, 0.4f, 0.8f);
	shieldStar->crownColor = point3d(0.2f, 0.6f, 1.0f);
	//shieldStar->opacity = 0.5f;

	// Добавляем коллайдер для обнаружения атак
	SphereCollider* sphereCollider = shieldEntity->AddComponent<SphereCollider>();
	sphereCollider->collisionGroup = CollisionFilter::Group::Player;
	sphereCollider->isTouchable = true;
	sphereCollider->radius = 2.0f;

	Health* ShieldHp = shieldEntity->AddComponent<Health>();
	ShieldHp->fraction = Fraction::Player;
	ShieldHp->immortal = true;
	UpdateProjectiles();
	shieldVisualIntensity = 0.5f;
	shieldLastDamageTime = 0.0f;
}


// Метод для деактивации щита
void PlayerAbilities::ShieldEnd()
{
	if (!shieldActive) {
		return;
	}

	shieldActive = false;

	// Возвращаем обычный цвет
	PointCloud* pointCloud = playerEntity->GetComponent<PointCloud>();
	if (pointCloud) {
		pointCloud->color = point3d(1.0f, 0.6f, 0.9f);
	}

	// Уничтожаем визуальный  эффект щита
	if (shieldEntity) {
		shieldEntity->Destroy();
		shieldEntity = nullptr;
	}
}

// Метод для обработки входящего урона
bool PlayerAbilities::TryBlockDamage(float damage)
{
	if (!shieldActive) {
		return false; // Щит не активен, пропускаем урон
	}

	// Рассчитываем стоимость блокировки в зависимости от урона
	float blockCost = damage * SHIELD_DAMAGE_MULTIPLIER;

	if (stamina >= blockCost) {
		// Блокируем урон и тратим энергию
		stamina -= blockCost;
		shieldLastDamageTime = timer::currentTime;

		// Визуальный эффект при блокировании
		if (shieldEntity) {
			Star* shieldStar = shieldEntity->GetComponent<Star>();
			if (shieldStar) {
				//shieldStar->opacity = 1.0f; // Вспышка при блокировании
				shieldVisualIntensity = 1.0f;
			}
		}

		return true; // Урон заблокирован
	}
	else {
		// Не хватает энергии - щит ломается
		ShieldEnd();
		return false; // Пропускаем урон
	}
}

void PlayerAbilities::ParticleVacuumStart() {

	if (currentParticles && !currentParticles->IsDeleting()) {
		currentParticles->Destroy();
	}

	// Запоминаем время начала
	vacuumStartTime = timer::currentTime;

	if (vacuumCenterEntity && !vacuumCenterEntity->IsDeleting()) {
		vacuumCenterEntity->Destroy();
		vacuumCenterEntity = nullptr;
	}

	if (starEntity && !starEntity->IsDeleting()) {
		starEntity->Destroy();
		starEntity = nullptr;
	}

	// Получаем цвет текущей туманности
	point3d nebulaColor = GetCurrentNebulaColor();

	vacuumCenterEntity = world->entityStorage->CreateEntity("VacuumCenter", playerEntity);
	if (vacuumCenterEntity) {
		Transform* centerTransform = vacuumCenterEntity->AddComponent<Transform>();
		centerTransform->position = point3d(0.0f, 3.0f, 2.0f);

		Star* centerStar = vacuumCenterEntity->AddComponent<Star>();
		centerStar->radius = 0.1f;
		centerStar->crownRadius = 0.25f;

		// Используем цвет туманности для звезды в центре
		centerStar->color1 = nebulaColor;
		centerStar->color2 = nebulaColor * 0.5f; // Более темный оттенок
		centerStar->crownColor = nebulaColor * 1.5f; // Более яркий оттенок
	}

	currentParticles = world->entityStorage->CreateEntity("Particles", playerEntity);
	Transform* transform = currentParticles->AddComponent<Transform>();
	transform->position = point3d(0.0f, 3.0f, 2.0f);

	ParticleEmitter* particleEmitter = currentParticles->AddComponent<ParticleEmitter>();
	particleEmitter->rate = 150;
	particleEmitter->lifetime = 1000;

	// Используем цвет туманности для частиц
	particleEmitter->color = nebulaColor;

	particleEmitter->size = { 0.0f, 4.0f };
	particleEmitter->opacity = { 1.0f, 0.0f };
	particleEmitter->emitDirection = EmitDirection::Up;
	particleEmitter->speed = { 10.0f, 0.0f };
	particleEmitter->spread = { PI, PI };
	particleEmitter->isReverse = true;
}

void PlayerAbilities::ParticleVacuumEnd() {
	
	if (currentParticles && !currentParticles->IsDeleting()) {
		ParticleEmitter* particleEmitter = currentParticles->GetComponent<ParticleEmitter>();
		if (particleEmitter) {
			particleEmitter->active = false;
		}
		currentParticles->AddComponent<DelayedDestroy>()->lifeTime = 1000;
		currentParticles = nullptr;
	}

	
	float finalSize = 0.5f;
	if (vacuumCenterEntity && !vacuumCenterEntity->IsDeleting()) {
		Star* centerStar = vacuumCenterEntity->GetComponent<Star>();
		if (centerStar) {
			finalSize = centerStar->radius; 
		}
		vacuumCenterEntity->Destroy();
		vacuumCenterEntity = nullptr;
	}

	CreateBlueStar(finalSize);
}

void PlayerAbilities::CreateBlueStar(float size) {

	if (!world) {
		return;
	}

	if (!playerEntity) {
		return;
	}

	Transform* playerTransform = playerEntity->GetComponent<Transform>();
	if (!playerTransform) {
		return;
	}

	if (starEntity && !starEntity->IsDeleting()) {
		starEntity->Destroy();
		starEntity = nullptr;
	}

	// Получаем цвет текущей туманности
	point3d nebulaColor = GetCurrentNebulaColor();

	starEntity = world->entityStorage->CreateEntity("BlueStar", playerEntity);
	if (!starEntity) {
		return;
	}

	Transform* starTransform = starEntity->AddComponent<Transform>();
	if (!starTransform) {
		return;
	}

	starTransform->position = point3d(0.0f, 3.0f, 2.0f);

	Star* star = starEntity->AddComponent<Star>();
	if (!star) {
		return;
	}

	star->baseRadius = size;
	star->baseCrownRadius = size * 1.5f;

	star->radius = size;
	star->crownRadius = size * 1.5f;

	// Используем цвет туманности для звезды
	star->color1 = nebulaColor;
	star->color2 = nebulaColor * 0.4f; // Темнее для внутренней части
	star->crownColor = nebulaColor * 1.2f; // Ярче для короны

	ParticleEmitter* starGlow = starEntity->AddComponent<ParticleEmitter>();
	starGlow->rate = 20;
	starGlow->lifetime = 800;

	// Используем тот же цвет для частиц свечения
	starGlow->color = nebulaColor * 0.8f;

	starGlow->size = { 0.5f, 1.5f };
	starGlow->opacity = { 0.6f, 0.0f };
	starGlow->emitDirection = EmitDirection::Front;
	starGlow->speed = { 2.0f, 1.0f };
	starGlow->spread = { PI, PI };

	DelayedDestroy* delayed = starEntity->AddComponent<DelayedDestroy>();
	if (delayed) {
		delayed->lifeTime = 10000;
	}
}

void PlayerAbilities::BlowGasStart() {

	if (!canBlowGas || !starEntity || starEntity->IsDeleting()) {
		return;
	}

	blowGasStartTime = timer::currentTime;
	isBlowingGas = true;

	if (gasBurstEntity && !gasBurstEntity->IsDeleting()) {
		gasBurstEntity->Destroy();
	}

	// Получаем цвет текущей туманности
	point3d nebulaColor = GetCurrentNebulaColor();

	gasBurstEntity = world->entityStorage->CreateEntity("GasBurst", nullptr);

	Transform* playerTransform = playerEntity->GetComponent<Transform>();
	Transform* gasTransform = gasBurstEntity->AddComponent<Transform>();

	gasTransform->position = playerTransform->position +
		playerTransform->GetLookVector() * 2.0f +
		playerTransform->GetUpVector() * 1.5f;

	// Основа выдувания с цветом туманности
	ParticleEmitter* jetEmitter = gasBurstEntity->AddComponent<ParticleEmitter>();
	jetEmitter->rate = 200;
	jetEmitter->lifetime = 1000;

	// Используем цвет туманности для газа
	jetEmitter->color = nebulaColor;

	jetEmitter->size = { 0.2f, 1.5f };
	jetEmitter->opacity = { 0.8f, 0.0f };
	jetEmitter->emitDirection = EmitDirection::Front;
	jetEmitter->speed = { 30.0f, 5.0f };
	jetEmitter->spread = { PI * 0.05f, PI * 0.05f };
}

void PlayerAbilities::BlowGasEnd() {
	if (isBlowingGas) {
		isBlowingGas = false;
	}
}

void PlayerAbilities::Attack(Transform startTransform, point3d direction)
{
	if (block || shieldActive) {
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
	if (block || shieldActive) {
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
	Entity* projectile;

	switch (weapon) {
	case PlayerWeapons::Fists:
	{
		projectile = FistsCommon(startTransform, direction);
		break;
	}
	case PlayerWeapons::Sword:
	{
		projectile = SwordCommon(startTransform, direction);
		break;
	}
	case PlayerWeapons::Bow:
	{
		projectile = BowCommon(startTransform, direction);
		break;
	}
	}

	projectiles.push_back(projectile);
}

void PlayerAbilities::ChargedAttack(Transform startTransform, point3d direction)
{
	Entity* projectile;

	switch (weapon) {
	case PlayerWeapons::Fists:
	{
		projectile = FistsCharged(startTransform, direction);
		break;
	}
	case PlayerWeapons::Sword:
	{
		projectile = SwordCharged(startTransform, direction);
		break;
	}
	case PlayerWeapons::Bow:
	{
		projectile = BowCharged(startTransform, direction);
		break;
	}
	}

	projectiles.push_back(projectile);
}

void PlayerAbilities::BlockStart()
{
	if (charging || shieldActive) {
		return;
	}

	block = true;

	PointCloud* pointCloud = playerEntity->GetComponent<PointCloud>();
	if (pointCloud) {
		pointCloud->color = point3d(1.0f, 1.0f, 0.0f);
	}
}

void PlayerAbilities::BlockEnd()
{
	block = false;

	PointCloud* pointCloud = playerEntity->GetComponent<PointCloud>();
	if (pointCloud) {
		pointCloud->color = point3d(1.0f, 0.6f, 0.9f);
	}
}


void PlayerAbilities::TimestopStart()
{
	timeStopped = true;
}


void PlayerAbilities::TimestopEnd()
{
	timeStopped = false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////


void PlayerAbilities::UpdateProjectiles()
{
	int i = 0;
	while (i < projectiles.size())
	{
		Entity* projectile = projectiles[i];

		if (!projectile->IsDeleting() && !projectile->HasComponent<SingleDamager>()) {
			CollisionInfo info = GetProjectileCollisionInfo(projectile);

			Entity* impact = entityStorage->CreateEntity("Impact", worldFolder);

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

			DelayedDestroy* delayedDestroy = impact->AddComponent<DelayedDestroy>();
			delayedDestroy->lifeTime = 1000;

			projectile->Destroy();
		}

		if (projectile->IsDeleting())
		{
			projectiles.erase(projectiles.begin() + i);
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


Entity* PlayerAbilities::FistsCommon(Transform startTransform, point3d direction)
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


Entity* PlayerAbilities::SwordCommon(Transform startTransform, point3d direction)
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

	return projectile;
}


Entity* PlayerAbilities::BowCommon(Transform startTransform, point3d direction)
{
	Entity* projectile = entityStorage->CreateEntity("PlayerProjectile");
	Transform* transform = projectile->AddComponent<Transform>();
	transform->position = startTransform.position;
	transform->mRotation = startTransform.mRotation;

	PhysicBody* physicBody = projectile->AddComponent<PhysicBody>();
	physicBody->airFriction = 0.0f;
	physicBody->velocity = direction.normalized() * 150.0f;

	/*Star* star = projectile->AddComponent<Star>();
	star->radius = 0.4f;
	star->color1 = point3d(0.9f, 1.0f, 0.99f);
	star->color2 = point3d(0.34f, 0.8f, 0.45f);
	star->crownColor = point3d(0.27f, 0.63f, 1.0f);*/

	Mesh* mesh = projectile->AddComponent<Mesh>();
	mesh->index = 1;
	//mesh->mRotation = XMMatrixRotationAxis(XMVectorSet(1, 0, 0, 0), 90 * RAD) * transform->mRotation;

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

	return projectile;
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
	transform->mRotation = startTransform.mRotation;

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


CollisionInfo PlayerAbilities::GetProjectileCollisionInfo(Entity* projectile)
{
	SphereCollider* sphereCollider = projectile->GetComponent<SphereCollider>();

	if (sphereCollider != nullptr) {
		for (CollisionInfo info : sphereCollider->collisions) {
			Entity* entity = entityStorage->GetEntityById(info.entityId);
			if (!IsEntityValid(entity)) {
				continue;
			}

			Health* health = entity->GetComponentInAncestor<Health>();
			if (health != nullptr && health->active) {
				return info;
			}
		}
	}

	return CollisionInfo();
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

		InteractiveNebula* interactiveNebula = entity->GetComponentInAncestor<InteractiveNebula>();
		if (interactiveNebula != nullptr && interactiveNebula->active) {
			return interactiveNebula;
		}
	}

	point3d playerPos = playerTransform->position;

	
	Entity* worldFolder = world->entityStorage->GetEntityByName("World");
	if (!worldFolder) return nullptr;


	vector<Entity*> locations = worldFolder->GetChildren(true);

	Nebula* nearestNebula = nullptr;
	float minDistance = FLT_MAX;

	for (Entity* location : locations) {
		if (!location || !location->IsActive()) continue;

		
		vector<Nebula*> nebulae = location->GetAllComponentsOfBase<Nebula>();

		for (Nebula* nebula : nebulae) {
			if (!nebula) continue;

			
			Entity* nebulaEntity = nullptr; 
		

			Transform* nebulaTransform = nullptr; // Получить трансформ

		
			float distance = 50.0f; // Заглушка

			if (distance < minDistance) {
				minDistance = distance;
				nearestNebula = nebula;
			}
		}
	}

	return nearestNebula;
}

point3d PlayerAbilities::GetCurrentNebulaColor()
{
	// Цвет по умолчанию (Белый)
	point3d defaultColor = point3d(1.0f, 1.0f, 1.0f);

	if (!playerEntity || !world) {
		return defaultColor;
	}

	// Получаем позицию игрока
	Transform* playerTransform = playerEntity->GetComponent<Transform>();
	if (!playerTransform) {
		return defaultColor;
	}

	point3d playerPos = playerTransform->position;

	// Получаем корневой элемент мира
	Entity* worldFolder = world->entityStorage->GetEntityByName("World");
	if (!worldFolder) return defaultColor;

	// Получаем все дочерние элементы мира (локации)
	vector<Entity*> locations = worldFolder->GetChildren(true);

	point3d resultColor = defaultColor;
	float totalWeight = 0.0f;
	const float INFLUENCE_RADIUS = 100.0f;

	for (Entity* location : locations) {
		if (!location || !location->IsActive()) continue;

		// Получаем трансформ локации
		Transform* locationTransform = location->GetComponent<Transform>();
		if (!locationTransform) continue;

		// Получаем все компоненты Nebula в этой локации
		vector<Nebula*> nebulae = location->GetAllComponentsOfBase<Nebula>();

		for (Nebula* nebula : nebulae) {
			if (!nebula) continue;

			// Расстояние до локации (приблизительно)
			float distance = (playerPos - locationTransform->position).magnitude();

			if (distance < INFLUENCE_RADIUS) {
				// Вес влияния обратно пропорционален расстоянию
				float weight = 1.0f - (distance / INFLUENCE_RADIUS);
				weight = max(0.0f, weight);

				// Используем цвет туманности, если он задан
				point3d nebulaColor = nebula->color;
				if (nebulaColor.magnitude() > 0.01f) {
					resultColor = resultColor + nebulaColor * weight;
					totalWeight += weight;
				}
			}
		}
	}

	// Нормализуем результат
	if (totalWeight > 0) {
		resultColor = resultColor / totalWeight;
	}

	return resultColor;
}
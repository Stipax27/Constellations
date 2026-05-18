#include "LevelManagerClass.h"

using namespace std;



LevelManagerClass::LevelManagerClass()
{
	window = 0;
	mouse = 0;

	m_World = 0;

	m_IsExecutionActive = false;
	m_ExecutionUI = nullptr;
	m_ExecutionTimer = 0.0f;
	m_ShowGameOverMessage = false;
	m_ShowVictoryMessage = false;
	m_MessageTimer = 0.0f;
	m_IsInBossArena = false;
}

LevelManagerClass::LevelManagerClass(const LevelManagerClass& other)
{
}


LevelManagerClass::~LevelManagerClass()
{
}


void LevelManagerClass::InitWindow()
{
	if (window == 0) {
		window = Singleton::GetInstance<WindowClass>();
	}
}

void LevelManagerClass::ProcessSound(const char* name)
{
	//PlaySound(TEXT(name), NULL, SND_FILENAME | SND_ASYNC);
}


bool LevelManagerClass::Initialize()
{
	InitWindow();

	m_World = Singleton::GetInstance<World>();
	m_World->Initialize();

	collisionManager = Singleton::GetInstance<CollisionManagerClass>();
	collisionManager->Initialize(*m_World->entityStorage);

	mouse = Singleton::GetInstance<MouseClass>();
	mouse->Initialize();

	questManager = Singleton::GetInstance<QuestManager>();
	questManager->Initialize();

	Dx11Init(window->hWnd, window->width, window->height);
	std::thread modelsLoadingThread(&LevelManagerClass::LoadModels, this);

	D3D11_BUFFER_DESC boneDesc = {};
	boneDesc.Usage = D3D11_USAGE_DEFAULT;
	boneDesc.ByteWidth = sizeof(XMMATRIX) * 128;
	boneDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	device->CreateBuffer(&boneDesc, nullptr, &m_BoneBuffer);

	// window params into const buffer
	ConstBuf::frame.aspect = XMFLOAT4(window->aspect, window->iaspect, float(window->width), float(window->height));

	ConstBuf::factors.AriesNebulaLerpFactor = 0;
	ConstBuf::UpdateFactors();

	//Textures::LoadTexture("..\\dx11minimal\\Resourses\\Textures\\testTexture.tga");
	Textures::LoadDDSTexture("gta", L"..\\dx11minimal\\Resourses\\Textures\\gta.dds");
	Textures::LoadDDSTexture("aperture", L"..\\dx11minimal\\Resourses\\Textures\\aperture.dds");
	Textures::LoadPNGTexture("comicsSpot", L"..\\dx11minimal\\Resourses\\Textures\\comicsSpot.png");

	if (modelsLoadingThread.joinable()) {
		modelsLoadingThread.join();
	}
	else {
		modelsLoadingThread.detach();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// WORLD CREATING START //
	//////////////////////////////////////////////////////////////////////////////////////////////

	Entity* entity;
	Constellation* constellation;
	Transform* transform;
	PhysicBody* physicBody;
	AIComponent* aiComponent;

	SphereCollider* sphereCollider;

	SpriteCluster* spriteCluster;
	PlaneCollider* planeCollider;
	ParticleEmitter* particleEmitter;

	Mesh* mesh;
	PointCloud* pointCloud;
	DelayedDestroy* delayedDestroy;
	Star* star;
	Health* health;
	SingleDamager* singleDamager;


	worldFolder = m_World->entityStorage->CreateEntity("World");

	Entity* player = CreatePlayer();


	
	/*entity = m_World->entityStorage->CreateEntity("AriesNebulaLocation", folder);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0.0f, 0.0f, 50.0f);
	transform->scale = point3d(1, 0, 0);
	spriteCluster = entity->AddComponent<SpriteCluster>();
	spriteCluster->vShader = 7;
	spriteCluster->gShader = 7;
	spriteCluster->pShader = 7;
	spriteCluster->pointsNum = 900000;
	spriteCluster->vertexNum = 1;
	spriteCluster->frustumRadius = 60;
	spriteCluster->topology = InputAssembler::topology::pointList;
	spriteCluster->compress = RenderCompress::x2;*/
	/*sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->radius = 25.0f;
	sphereCollider->softness = 0.5f;*/
	//entity->AddComponent<SurfaceCollider>();

	/*entity = m_World->entityStorage->CreateEntity("StarsBackground", folder);
	spriteCluster = entity->AddComponent<SpriteCluster>();
	spriteCluster->vShader = 2;
	spriteCluster->pShader = 2;
	spriteCluster->pointsNum = 10000;*/

	/////////////////////////

	//entity = m_World->entityStorage->CreateEntity("Star", folder);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(0.0f, 0.0f, -20.0f);
	//star = entity->AddComponent<Star>();
	//sphereCollider = entity->AddComponent<SphereCollider>();
	//sphereCollider->radius = 0.5f;
	//sphereCollider->active = false;
	//sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	////sphereCollider->softness = 0.5f;
	//singleDamager = entity->AddComponent<SingleDamager>();
	//singleDamager->damage = 1000;

	/*entity = m_World->entityStorage->CreateEntity("TestStar", worldFolder);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(-200, 0, -200);
	star = entity->AddComponent<Star>();
	star->radius = 75;
	star->crownRadius = 3.0f;
	star->color1 = point3d(0.87f, 0.24f, 0.13f);
	star->color2 = point3d(0.35f, 0.0f, 0.07f);
	star->crownColor = point3d(0.87f, 0.25f, 0.15f);
	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->radius = 75;
	sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	GravityPoint* gravityPoint = entity->AddComponent<GravityPoint>();
	gravityPoint->mass = 500;
	gravityPoint->radius = 150;

	entity = m_World->entityStorage->CreateEntity("Ray", worldFolder);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(20, 20, 0);
	Beam* beam = entity->AddComponent<Beam>();
	beam->point1 = point3d();
	beam->point2 = point3d(0, 0, 25);
	RayDamager* rayDamager = entity->AddComponent<RayDamager>();
	rayDamager->direction = point3d(0, 0, 25);
	rayDamager->repeats = -1;
	rayDamager->damage = 0.1f;*/

	/////////////////////////
	
	CreateSpaceBackground(worldFolder, 1);
	CreateAries(worldFolder);
	CreateZenithLocation(worldFolder, 2);
	//CreateNebula(worldFolder,2);
	//CreateStarQuestLoc(worldFolder, 2);

	/////////////////////////

	/*Entity* holder = m_World->entityStorage->CreateEntity("Holder", folder);
	transform = holder->AddComponent<Transform>();
	transform->scale = point3d(10, 10, 10);
	transform->position = point3d(0.0f, 0.0f, -50.0f);
	mesh = holder->AddComponent<Mesh>();
	mesh->index = 1;*/

	CreateUI();

	//////////////////////////////////////////////////////////////////////////////////////////////
	// WORLD CREATING END //
	//////////////////////////////////////////////////////////////////////////////////////////////

	InitSystems();

	playerController = new PlayerController();
	playerController->Initialize(player);

	

	// Тестовый враг для ИИ amogus
	//testEnemy = m_World->entityStorage->CreateEntity("TestEnemy", worldFolder);
	//Transform* testTransform = testEnemy->AddComponent<Transform>();
	//point3d CentralPatrolPoint = point3d(10.0f, 5.0f, 15.0f);
	//testTransform->position = CentralPatrolPoint + point3d(5.0f, 0.0f, 0.0f); // Стартовая позиция

	//sphereCollider = testEnemy->AddComponent<SphereCollider>();
	//sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	//testEnemy->AddComponent<CameraTarget>();

	//health = testEnemy->AddComponent<Health>();
	//health->fraction = Fraction::Enemy;
	//health->maxHp = 50;

	//// Кодовое слово: amogus
	//PhysicBody* testPhysic = testEnemy->AddComponent<PhysicBody>();
	//testPhysic->airFriction = 0.01f; // Обычное трение

	//testPhysic->velocity = point3d(0.0f, 10.0f, 0.0f);

	//// Визуальная составляющая (можно использовать Star или любой другой компонент)
	//Star* testStar = testEnemy->AddComponent<Star>();
	//testStar->radius = 1.0f;
	//star->color1 = point3d(0.5f, 0.5f, 0.5f); // Цвет (бесполезен)

	//// Компонент ИИ
	//AIComponent* ai = testEnemy->AddComponent<AIComponent>();
	//ai->enabled = true;
	//ai->behaviorType = AIBehaviorType::PATROL; // Начинаем с патруля

	//// Точки патрулирования (локальные координаты относительно врага? 
	//// В текущей реализации patrolPoints задаются в мировых координатах, 
	//// поэтому зададим абсолютные позиции)
	//
	//ai->patrolPoints = {
	//	CentralPatrolPoint + point3d(-2.0f, 2.0f, 0.0f),
	//	CentralPatrolPoint + point3d(2.0f, 2.0f, 0.0f),
	//	CentralPatrolPoint + point3d(2.0f, -2.0f, 0.0f),
	//	CentralPatrolPoint + point3d(-2.0f, -2.0f, 0.0f)
	//};
	////ai->patrolPoints = { CentralPatrolPoint };

	//ai->movementSpeed = 3.0f;
	//ai->arrivalDistance = 1.0f;

	//// Параметры обнаружения
	//ai->detectionRange = 12.0f;  // Радиус, в котором замечает игрока
	//ai->chaseRange = 18.0f;      // Радиус преследования
	//ai->attackRange = 3.0f;      // Радиус атаки

	//// Параметры атаки (пока атака не реализована, но можно оставить)
	//ai->attackCooldown = 1.5f;
	//ai->attackDamage = 5.0f;

	//// Таймеры состояний
	//ai->idleDuration = 2.0f;
	//ai->fleeDuration = 4.0f;

	////Параметры ускорения
	//ai->accelerationStrength = 10;
	//ai->maxAcceleration = 100;


	auto spawnSkinned = [this](
		const char* name,
		const point3d& pos,
		const point3d& scale,
		const SkinnedMesh& srcMesh,
		Skeleton* skeleton,
		std::vector<AnimationClip>* animations,
		const char* preferredClip = nullptr,
		bool autoPlay = true) -> Entity*
	{
		Entity* e = m_World->entityStorage->CreateEntity(name, worldFolder);
		Transform* t = e->AddComponent<Transform>();
		t->position = pos;
		t->scale = scale;

		SkinnedMesh* meshComp = e->AddComponent<SkinnedMesh>();
		meshComp->vertices = srcMesh.vertices;
		meshComp->indices = srcMesh.indices;
		meshComp->gpuModelIndex = srcMesh.gpuModelIndex;
		meshComp->active = false;

		PointCloud* pointCloud = e->AddComponent<PointCloud>();
		pointCloud->index = srcMesh.gpuModelIndex;
		pointCloud->vShader = 28;
		pointCloud->gShader = 17;
		pointCloud->pShader = 17;
		pointCloud->pointSize = 1.0f;
		pointCloud->brightness = 1.25f;
		pointCloud->color = point3d(1.0f, 0.95f, 0.85f);
		pointCloud->frustumRadius = 12.0f;
		pointCloud->compress = RenderCompress::x2;

		SkeletalAnimationComponent* animComp = e->AddComponent<SkeletalAnimationComponent>();
		animComp->skeleton = skeleton;
		animComp->animationClips = animations;
		animComp->rootMotionMode = RootMotionMode::Accumulate;
		animComp->CaptureBindPose();
		if (animations && !animations->empty())
		{
			animComp->SetAnimationByIndex(0, true);
			if (preferredClip && preferredClip[0] != '\0')
			{
				animComp->SetAnimationByName(preferredClip, true);
			}
		}
		animComp->isPlaying = autoPlay && animComp->animationClip != nullptr;
		animComp->isLooping = true;
		animComp->currentTime = 0.0f;
		return e;
	};

	//spawnSkinned("Fox", point3d(0.0f, 0.0f, 10.0f), point3d(0.2f, 0.2f, 0.2f), m_FoxMesh, &m_FoxSkeleton, &m_FoxAnimations);
	//spawnSkinned("CesiumMan", point3d(3.0f, 0.0f, 10.0f), point3d(1.0f, 1.0f, 1.0f), m_CesiumMesh, &m_CesiumSkeleton, &m_CesiumAnimations);
	//spawnSkinned("PunchComboNew", point3d(3.0f, 0.0f, 10.0f), point3d(100.0f, 100.0f, 100.0f), m_PunchComboNewMesh, &m_PunchComboNewSkeleton, &m_PunchComboNewAnimations);
	//m_TestAnimEntity = spawnSkinned("TestMannequin", point3d(6.0f, 0.0f, 10.0f), point3d(1.0f, 1.0f, 1.0f), m_TestAnimMesh, &m_TestAnimSkeleton, &m_TestAnimAnimations, nullptr, false);
	m_TestAnimCycleIndex = 0;
	if (m_TestAnimEntity)
	{
		if (SkeletalAnimationComponent* animComp = m_TestAnimEntity->GetComponent<SkeletalAnimationComponent>())
		{
			animComp->ResetToBindPose();
			animComp->animationClip = nullptr;
			animComp->isPlaying = false;
			animComp->currentTime = 0.0f;
		}
	}
	


	return true;
}


void LevelManagerClass::Shutdown()
{
	if (playerController)
	{
		playerController->Shutdown();
		delete playerController;
		playerController = 0;
	}

	if (m_World)
	{
		m_World->Shutdown();
		delete m_World;
		m_World = 0;
	}

	if (collisionManager)
	{
		collisionManager->Shutdown();
		delete collisionManager;
		collisionManager = 0;
	}

	if (questManager)
	{
		questManager->Shutdown();
		delete questManager;
		questManager = 0;
	}

	if (mouse)
	{
		delete mouse;
		mouse = 0;
	}

	if (window)
	{
		delete window;
		window = 0;
	}

	if (m_BoneBuffer)
	{
		m_BoneBuffer->Release();
		m_BoneBuffer = nullptr;
	}
}

double shotTime = 0;

void LevelManagerClass::Frame()
{
	mouse->UpdateSystemCursorVisibility();
	if (!window->IsActive())
		return;

	mouse->Update();

	UpdateTestAnimationToggle();

	interp::UpdateTweens();

	playerController->ProcessInput();
	playerController->ProcessMouse();
	playerController->abilities->Update();
	playerController->ProccessUI();

	questManager->UpdateQuests();

	// DEBUG
	Entity* player = m_World->entityStorage->GetEntityByName("Player");
	if (IsEntityValid(player) && !m_ShowVictoryMessage)
	{
		Health* playerHealth = player->GetComponent<Health>();
		if (playerHealth && playerHealth->hp <= 0 && !m_ShowGameOverMessage)
		{
			m_ShowGameOverMessage = true;
			m_MessageTimer = 3.0f;

			// Создаём сообщение
			Entity* msgContainer = m_World->entityStorage->CreateEntity("GameOverMsg", nullptr);
			Transform2D* msgTransform = msgContainer->AddComponent<Transform2D>();
			msgTransform->anchorPoint = point3d(0, 0, 0);
			msgTransform->ratio = ScreenAspectRatio::XY;
			msgTransform->position = point3d(-0.35f, -0.1f, 0);

			TextLabel* text = msgContainer->AddComponent<TextLabel>();
			text->textW = L"ВЫ ПОГИБЛИ";
			text->fontFamilyW = L"Impact";
			text->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
			text->fontWeight = 900;
			text->fontSizePx = 70;
			text->fontScale = 1.2f;
			text->color = point3d(1.0f, 0.2f, 0.2f);

			// Автоудаление через 3 секунды
			DelayedDestroy* delayed = msgContainer->AddComponent<DelayedDestroy>();
			delayed->lifeTime = 3000;

			// Отключаем управление игроком
			if (playerController)
			{
				//playerController->SetEnabled(false);
			}
		}
	}

	if (m_CurrentBoss && m_CurrentBoss->IsActive() && !m_IsExecutionActive)
	{
		Health* bossHealth = m_CurrentBoss->GetComponent<Health>();
		if (bossHealth)
		{
			float healthPercent = bossHealth->hp / bossHealth->maxHp;

			// Если HP меньше 5% и ещё не в режиме казни
			if (healthPercent <= 0.05f)
			{
				TriggerExecution();
			}
		}
	}

	if (m_IsExecutionActive && m_CurrentBoss)
	{
		// Обновляем таймер
		m_ExecutionTimer -= 1.0f / 60.0f;  // Приблизительно deltaTime

		// Обновляем UI таймера
		if (m_ExecutionUI)
		{
			// Ищем дочерний Entity с таймером
			const std::vector<Entity*>& children = m_ExecutionUI->GetChildren();
			for (Entity* child : children)
			{
				if (child->name == "ExecutionTimer")
				{
					TextLabel* timerLabel = child->GetComponent<TextLabel>();
					if (timerLabel)
					{
						wchar_t buffer[16];
						swprintf(buffer, 16, L"%.1f", m_ExecutionTimer);
						timerLabel->textW = buffer;

						// Меняем цвет при приближении времени
						if (m_ExecutionTimer < 2.0f)
							timerLabel->color = point3d(1.0f, 0.0f, 0.0f);
						else if (m_ExecutionTimer < 4.0f)
							timerLabel->color = point3d(1.0f, 1.0f, 0.0f);
					}
					break;
				}
			}
		}

		// Если время вышло - босс восстанавливается
		if (m_ExecutionTimer <= 0)
		{
			m_IsExecutionActive = false;
			HideExecutionUI();

			// Восстанавливаем босса
			AIComponent* bossAI = m_CurrentBoss->GetComponent<AIComponent>();
			if (bossAI) bossAI->enabled = true;

			Star* bossStar = m_CurrentBoss->GetComponent<Star>();
			if (bossStar)
			{
				bossStar->color1 = point3d(1.0f, 0.2f, 0.2f);
				bossStar->crownColor = point3d(0.8f, 0.3f, 0.3f);
				bossStar->radius = 5.0f;
			}

			// Сбрасываем флаг, чтобы при следующем падении HP ниже 5% QTE сработало снова
			// Нужно добавить флаг в BossComponent
		}

		// ===== ПРОВЕРКА НАЖАТИЯ КНОПКИ R =====
		if (input::IsKeyPressed('R'))
		{
			static int pressCount = 0;
			
			pressCount++;

			if (pressCount >= 5)
			{
				ExecuteBoss();
				pressCount = 0; // Сброс после казни
			}
			
			
		}
	}

	if (input::IsKeyDown('K'))
	{
		if (m_CurrentBoss && m_CurrentBoss->IsActive())
		{
			Health* bossHealth = m_CurrentBoss->GetComponent<Health>();
			if (bossHealth)
			{
				bossHealth->hp -= 100.0f;
				//Log("Test damage! Boss HP: %.0f\n", bossHealth->hp);
			}
		}
	}
	// ===== ПРОВЕРКА ПОБЕДЫ НАД БОССОМ =====
	if (m_CurrentBoss && m_CurrentBoss->IsActive())
	{
		Health* bossHealth = m_CurrentBoss->GetComponent<Health>();
		if (bossHealth)
		{
			static float lastLog = 0;
			if (timer::currentTime - lastLog > 1.0f)
			{
				lastLog = timer::currentTime;
				//Log("Boss HP: %.0f / %.0f\n", bossHealth->hp, bossHealth->maxHp);
			}
		}
	}

	
	static bool victoryShown = false;

	if (m_CurrentBoss && m_CurrentBoss->IsActive())
	{
		Health* bossHealth = m_CurrentBoss->GetComponent<Health>();
		if (bossHealth && bossHealth->hp <= 0 && !victoryShown)
		{
			victoryShown = true;

			//Log("BOSS DEFEATED! Showing victory message...\n");

			// Сообщение победы
			Entity* msg = m_World->entityStorage->CreateEntity("VictoryMsg", nullptr);
			Transform2D* t = msg->AddComponent<Transform2D>();
			t->anchorPoint = point3d(0, 0, 0);
			t->ratio = ScreenAspectRatio::XY;
			t->position = point3d(-0.25f, -0.1f, 0);

			TextLabel* text = msg->AddComponent<TextLabel>();
			text->textW = L"ПОБЕДА!";
			text->fontFamilyW = L"Impact";
			text->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
			text->fontWeight = 900;
			text->fontSizePx = 80;
			text->fontScale = 1.5f;
			text->color = point3d(0.2f, 0.8f, 0.2f);

			DelayedDestroy* d = msg->AddComponent<DelayedDestroy>();
			d->lifeTime = 3000;

			// Отключаем босса
			AIComponent* bossAI = m_CurrentBoss->GetComponent<AIComponent>();
			if (bossAI) bossAI->enabled = false;
		}
	}

	// Обновляем таймер сообщений
	if (m_ShowGameOverMessage || m_ShowVictoryMessage)
	{
		m_MessageTimer -= 1.0f / 60.0f;  // Примерно 1 кадр
		if (m_MessageTimer <= 0)
		{
			m_ShowGameOverMessage = false;
			m_ShowVictoryMessage = false;
		}
	}
	 // Обновление полоски здоровья босса
	if (m_CurrentBoss && m_CurrentBoss->IsActive() && m_BossHealthFill) {

		Health* bossHealth = m_CurrentBoss->GetComponent<Health>();
		if (bossHealth) {
			// Показываем контейнер
			Entity* bossContainer = m_BossHealthFill->GetParent();
			if (bossContainer) bossContainer->SetActive(true);

			// Обновляем полоску
			float healthPercent = bossHealth->hp / bossHealth->maxHp;
			Transform2D* barTransform = m_BossHealthFill->GetComponent<Transform2D>();
			if (barTransform) {
				barTransform->scale.x = 0.5f * healthPercent;
			}

			// Меняем цвет
			Rect* barRect = m_BossHealthFill->GetComponent<Rect>();
			if (barRect) {
				if (healthPercent > 0.6f) barRect->color = point3d(0.2f, 0.8f, 0.2f);
				else if (healthPercent > 0.3f) barRect->color = point3d(0.8f, 0.8f, 0.2f);
				else barRect->color = point3d(0.8f, 0.2f, 0.2f);
			}

			// Обновляем цифры (прямо по указателю)
			if (m_BossNumbersText) {
				wchar_t buffer[64];
				swprintf(buffer, 64, L"%.0f / %.0f", bossHealth->hp, bossHealth->maxHp);
				m_BossNumbersText->textW = buffer;
			}
		}
	}
	else {
		// Скрываем UI босса
		if (m_BossHealthFill) {
			Entity* bossContainer = m_BossHealthFill->GetParent();
			if (bossContainer) bossContainer->SetActive(false);
		}
	}


	if (m_CurrentBoss && m_CurrentBoss->IsActive())
	{
		Entity* player = m_World->entityStorage->GetEntityByName("Player");
		if (player)
		{
			Transform* playerTransform = player->GetComponent<Transform>();
			if (playerTransform)
			{
				// Используй magnitude() если есть, или sqrt
				float dx = playerTransform->position.x - m_BossArenaCenter.x;
				float dz = playerTransform->position.z - m_BossArenaCenter.z;
				float dist = sqrt(dx * dx + dz * dz);  // ИЛИ playerTransform->position.distance(m_BossArenaCenter)

				// Вход на арену
				if (dist < m_BossArenaRadius && !m_IsInBossArena)
				{
					m_IsInBossArena = true;
					AIComponent* bossAI = m_CurrentBoss->GetComponent<AIComponent>();
					if (bossAI) bossAI->enabled = true;
					Entity* bossUIContainer = m_World->entityStorage->GetEntityByName("BossUIContainer");
					if (bossUIContainer) bossUIContainer->SetActive(true);
				}

				

				// Блокируем границы
				if (m_IsInBossArena)
				{
					if (playerTransform->position.x < -50.0f) playerTransform->position.x = -50.0f;
					if (playerTransform->position.x > 50.0f) playerTransform->position.x = 50.0f;
					if (playerTransform->position.z < 150.0f) playerTransform->position.z = 150.0f;   // min Z = центр - 50
					if (playerTransform->position.z > 250.0f) playerTransform->position.z = 250.0f;   // max Z = центр + 50
				}
			}
		}
	}
	else
	{
		m_IsInBossArena = false;
	}
	//if (worldFolder->localTime - shotTime >= 500) {
	//	shotTime = worldFolder->localTime;

	//	// Physic damage
	//	Entity* projectile = m_World->entityStorage->CreateEntity("TestProjectile", worldFolder);
	//	Transform* transform = projectile->AddComponent<Transform>();
	//	transform->position = point3d(0, 20, 0);

	//	PhysicBody* physicBody = projectile->AddComponent<PhysicBody>();
	//	physicBody->airFriction = 0.0f;
	//	physicBody->velocity = point3d(0, 0, 1) * 20.0f;

	//	Star* star = projectile->AddComponent<Star>();
	//	star->radius = 0.8f;
	//	star->color1 = point3d(0.9f, 1.0f, 0.99f);
	//	star->color2 = point3d(0.34f, 0.8f, 0.45f);
	//	star->crownColor = point3d(0.27f, 0.63f, 1.0f);

	//	SingleDamager* singleDamager = projectile->AddComponent<SingleDamager>();
	//	singleDamager->target = Fraction::Player;
	//	singleDamager->damage = 5.0f;
	//	singleDamager->destroyable = true;
	//	singleDamager->damageType = DamageType::Physic;

	//	SphereCollider* sphereCollider = projectile->AddComponent<SphereCollider>();
	//	sphereCollider->isTouchable = false;
	//	sphereCollider->radius = 0.8f;

	//	DelayedDestroy* delayedDestroy = projectile->AddComponent<DelayedDestroy>();
	//	delayedDestroy->lifeTime = 2000;

	//	// Magic damage
	//	projectile = m_World->entityStorage->CreateEntity("TestProjectile", worldFolder);
	//	transform = projectile->AddComponent<Transform>();
	//	transform->position = point3d(10, 20, 0);

	//	physicBody = projectile->AddComponent<PhysicBody>();
	//	physicBody->airFriction = 0.0f;
	//	physicBody->velocity = point3d(0, 0, 1) * 20.0f;

	//	star = projectile->AddComponent<Star>();
	//	star->radius = 0.8f;
	//	star->color1 = point3d(1, 0.6, 0);
	//	star->color2 = point3d(0.93, 0.28, 0);
	//	star->crownColor = point3d(1, 0.87, 0.25);

	//	singleDamager = projectile->AddComponent<SingleDamager>();
	//	singleDamager->target = Fraction::Player;
	//	singleDamager->damage = 5.0f;
	//	singleDamager->destroyable = true;
	//	singleDamager->damageType = DamageType::Magic;

	//	sphereCollider = projectile->AddComponent<SphereCollider>();
	//	sphereCollider->isTouchable = false;
	//	sphereCollider->radius = 0.8f;

	//	delayedDestroy = projectile->AddComponent<DelayedDestroy>();
	//	delayedDestroy->lifeTime = 2000;
	//}

	// DEBUG

	ConstBuf::frame.aspect = XMFLOAT4{ float(window->aspect), float(window->iaspect), float(window->width), float(window->height) };

	m_World->UpdateCompute();
	m_World->UpdatePhysic();

	// Изменение цвета testStar в зависимости от состояния ИИ
	if (testEnemy && testEnemy->IsActive()) {
		AIComponent* ai = testEnemy->GetComponent<AIComponent>();
		Star* star = testEnemy->GetComponent<Star>();
		if (ai && star) {
			switch (ai->behaviorType) {
			case AIBehaviorType::PATROL:
				star->color1 = point3d(0.2f, 0.8f, 0.2f); // зелёный
				break;
			case AIBehaviorType::CHASE:
				star->color1 = point3d(1.0f, 0.5f, 0.0f); // оранжевый
				break;
			case AIBehaviorType::ATTACK:
				star->color1 = point3d(1.0f, 0.0f, 0.0f); // красный
				break;
			case AIBehaviorType::FLEE:
				star->color1 = point3d(0.0f, 0.0f, 1.0f); // синий
				break;
			case AIBehaviorType::IDLE:
				star->color1 = point3d(0.5f, 0.5f, 0.5f); // серый
				break;
			}
		}
	}

	playerController->ProcessCamera();

	m_World->UpdateRender();

	mouse->RenderCursor();
	Draw::Present();
}


////////////////////////////////////////////////////////////////////////////////////////////////


void LevelManagerClass::InitSystems()
{
	m_World->AddComputeSystem<TimeSystem>();
	m_World->AddComputeSystem<DelayedDestroySystem>();
	m_World->AddComputeSystem<AISystem>();
	m_World->AddComputeSystem<QuestSystem>();
	m_World->AddComputeSystem<RayDamageSystem>();

	m_World->AddPhysicSystem<GravitySystem>();
	m_World->AddPhysicSystem<PhysicSystem>();
	m_World->AddPhysicSystem<CollisionSystem>();
	m_World->AddPhysicSystem<CombatSystem>();
	m_World->AddPhysicSystem<SkeletalAnimationSystem>(context, m_BoneBuffer);

	m_World->AddRenderSystem<MeshSystem>();
	m_World->AddRenderSystem<StarClaySystem>();
	m_World->AddRenderSystem<SkinnedMeshSystem>(m_World->m_Camera->frustum, m_World->m_Camera, m_BoneBuffer);

	if (SHOW_COLLIDERS) {
		m_World->AddRenderSystem<CollisionDrawSystem>();
	}
	if (SHOW_GRAVITY) {
		m_World->AddRenderSystem<GravityDrawSystem>();
	}

	m_World->AddRenderSystem<SpriteSystem>(m_World->m_Camera->frustum, m_BoneBuffer);
	m_World->AddRenderSystem<NebulaSystem>();
	m_World->AddRenderSystem<UISystem>();
	m_World->AddRenderSystem<UITextSystem>();
}


void LevelManagerClass::LoadModels()
{
	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\FistBasePos.obj");
	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\Arrow.obj");

	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\AriesBody.obj");
	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\AriesArmor.obj");
	Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\Fox.glb", m_FoxMesh, m_FoxSkeleton, m_FoxAnimations);
	Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\CesiumMan.glb", m_CesiumMesh, m_CesiumSkeleton, m_CesiumAnimations);
	Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\PunchComboNew.glb", m_PunchComboNewMesh, m_PunchComboNewSkeleton, m_PunchComboNewAnimations);

	if (Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\TestAnims\\1.glb", m_TestAnimMesh, m_TestAnimSkeleton, m_TestAnimAnimations))
	{
		Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\2.glb", m_TestAnimSkeleton, m_TestAnimAnimations, false);
		Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\3.glb", m_TestAnimSkeleton, m_TestAnimAnimations, true);
		Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\4.glb", m_TestAnimSkeleton, m_TestAnimAnimations, true);
	}

	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\SnakeModel.obj");
}

// TODO: Remove, only for test animation change
void LevelManagerClass::UpdateTestAnimationToggle()
{
	const bool isTogglePressed = input::IsKeyDown('Y');
	if (!isTogglePressed)
	{
		m_WasToggleAnimationPressed = false;
		return;
	}

	if (m_WasToggleAnimationPressed)
	{
		return;
	}

	m_WasToggleAnimationPressed = true;

	if (!m_TestAnimEntity)
	{
		return;
	}

	SkeletalAnimationComponent* animComp = m_TestAnimEntity->GetComponent<SkeletalAnimationComponent>();
	if (!animComp)
	{
		return;
	}

	const int clipCount = animComp->animationClips ? static_cast<int>(animComp->animationClips->size()) : 0;
	if (clipCount <= 0)
	{
		animComp->ResetToBindPose();
		animComp->animationClip = nullptr;
		animComp->isPlaying = false;
		animComp->currentTime = 0.0f;
		m_TestAnimCycleIndex = 0;
		return;
	}

	m_TestAnimCycleIndex = (m_TestAnimCycleIndex + 1) % (clipCount + 1);
	if (m_TestAnimCycleIndex == 0)
	{
		animComp->ResetToBindPose();
		animComp->animationClip = nullptr;
		animComp->isPlaying = false;
		animComp->currentTime = 0.0f;
		return;
	}

	animComp->SetAnimationByIndex(static_cast<size_t>(m_TestAnimCycleIndex - 1), true);
	animComp->isPlaying = animComp->animationClip != nullptr;
	animComp->isLooping = true;
	animComp->currentTime = 0.0f;
}


Entity* LevelManagerClass::CreatePlayer(Entity* folder)
{
	Entity* player = m_World->entityStorage->CreateEntity("Player", folder);

	Transform* transform = player->AddComponent<Transform>();
	transform->position = point3d(0.0f, 0.0f, 0.0f);

	PhysicBody* physicBody = player->AddComponent<PhysicBody>();
	physicBody->preciseMovement = true;

	SphereCollider* sphereCollider = player->AddComponent<SphereCollider>();
	sphereCollider->radius = 0.75f;
	sphereCollider->collisionGroup = CollisionFilter::Group::Player;

	Health* health = player->AddComponent<Health>();
	health->fraction = Fraction::Player;
	health->destroyOnDeath = false;

	/*Constellation* constellation = player->AddComponent<Constellation>();
	constellation->stars = {
		point3d(-0.09, -0.7, 0),
		point3d(-0.05, -0.15, 0),
		point3d(0, 0, 0),
		point3d(-0.4, 0.5, 0),
		point3d(0, 0, 0),
		point3d(0.4, 0.3, 0)
	};
	constellation->links = {
		{0,1},
		{1,2},
		{2,3},
		{2,5}
	};*/

	StarClay* starClay = player->AddComponent<StarClay>();
	starClay->blobsRadius = { 0.2f, 0.4f };
	starClay->rate = 20;

	PointCloud* pointCloud = player->AddComponent<PointCloud>();
	pointCloud->index = 0;
	pointCloud->scale = point3d(0.3f, 0.3f, 0.3f);
	pointCloud->pointSize = 0.01f;
	pointCloud->brightness = 0.2f;
	pointCloud->color = point3d(1, 0.6f, 0.9f);

	Entity* grabHitbox = m_World->entityStorage->CreateEntity("GrabHitbox", player);
	transform = grabHitbox->AddComponent<Transform>();
	transform->position = point3d(0, 0, 4);
	sphereCollider = grabHitbox->AddComponent<SphereCollider>();
	sphereCollider->radius = 4;
	sphereCollider->isTouchable = false;

	m_World->entityStorage->SaveEntityToFile(player, "Player");

	return player;
}


void LevelManagerClass::CreateUI()
{
	Entity* entity;
	Transform2D* transform2D;
	Rect* rect;
	Button* button;
	TextLabel* textLabel;

	Entity* uiFolder = m_World->entityStorage->CreateEntity("UI");



	entity = m_World->entityStorage->CreateEntity("HealthHolder", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(-1, 0, 0);
	transform2D->ratio = ScreenAspectRatio::XY;
	transform2D->position = point3d(-0.9f, -0.6f, 0.0f);
	transform2D->scale = point3d(0.18f, 0.04f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.5f, 0.5f, 0.5f);
	rect->opacity = 0.5f;

	entity = m_World->entityStorage->CreateEntity("HealthBar", entity);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(-1, 0, 0);
	transform2D->ratio = ScreenAspectRatio::XY;
	//transform2D->position = point3d(-1, 0, 0);
	rect = entity->AddComponent<Rect>();

	entity = m_World->entityStorage->CreateEntity("StaminaHolder", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(-1, 0, 0);
	transform2D->ratio = ScreenAspectRatio::XY;
	transform2D->position = point3d(-0.9f, -0.7f, 0.0f);
	transform2D->scale = point3d(0.18f, 0.02f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.5f, 0.5f, 0.5f);
	rect->opacity = 0.5f;

	entity = m_World->entityStorage->CreateEntity("StaminaBar", entity);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(-1, 0, 0);
	transform2D->ratio = ScreenAspectRatio::XY;
	//transform2D->position = point3d(-1, 0, 0);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.8f, 0.8f, 1);

	entity = m_World->entityStorage->CreateEntity("HealthLabel", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->ratio = ScreenAspectRatio::XY;
	transform2D->position = point3d(-0.9f, -0.64f, 0.0f);
	textLabel = entity->AddComponent<TextLabel>();
	textLabel->textW = L"ЗДОРОВЬЕ";
	textLabel->fontFamilyW = L"Impact";
	textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	textLabel->fontWeight = 900;
	textLabel->fontSizePx = 44;
	textLabel->fontScale = 0.40f;
	textLabel->letterSpacingPx = 1.0f;

	entity = m_World->entityStorage->CreateEntity("StaminaLabel", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->ratio = ScreenAspectRatio::XY;
	transform2D->position = point3d(-0.9f, -0.74f, 0.0f);
	textLabel = entity->AddComponent<TextLabel>();
	textLabel->textW = L"ВЫНОСЛИВОСТЬ";
	textLabel->fontFamilyW = L"Impact";
	textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	textLabel->fontWeight = 900;
	textLabel->fontSizePx = 38;
	textLabel->fontScale = 0.34f;
	textLabel->letterSpacingPx = 1.0f;

	entity = m_World->entityStorage->CreateEntity("ElementLabel", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->ratio = ScreenAspectRatio::XY;
	transform2D->position = point3d(0.8f, -0.64f, 0.0f);
	textLabel = entity->AddComponent<TextLabel>();
	textLabel->textW = L"НЕТ";
	textLabel->fontFamilyW = L"Impact";
	textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	textLabel->fontWeight = 900;
	textLabel->fontSizePx = 44;
	textLabel->fontScale = 0.40f;
	textLabel->letterSpacingPx = 1.0f;


	// UI Prototypes
	// Selected weapon - circle
	entity = m_World->entityStorage->CreateEntity("Rect", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(-1, 0, 0);
	transform2D->ratio = ScreenAspectRatio::XX;
	transform2D->position = point3d(-0.9f, -0.85f, 0.0f);
	transform2D->scale = point3d(0.05f, 0.05f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.75f, 0.0f, 0.0f);
	//rect->opacity = 0.5f;
	rect->cornerRadius = 1.f;

	// Charged attack - circle
	entity = m_World->entityStorage->CreateEntity("Rect", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(-1, 0, 0);
	transform2D->ratio = ScreenAspectRatio::XX;
	transform2D->position = point3d(-0.75f, -0.85f, 0.0f);
	transform2D->scale = point3d(0.05f, 0.05f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.75f, 0.0f, 0.0f);
	rect->cornerRadius = 1.f;

	// Battle end timer - icon
	entity = m_World->entityStorage->CreateEntity("Rect", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(-1, 0, 0);
	transform2D->ratio = ScreenAspectRatio::XX;
	transform2D->position = point3d(-0.9f, 0.85f, 0.0f);
	transform2D->scale = point3d(0.05f, 0.05f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.75f, 0.0f, 0.0f);

	// Battle end timer - text
	entity = m_World->entityStorage->CreateEntity("Rect", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(-1, 0, 0);
	transform2D->ratio = ScreenAspectRatio::XX;
	transform2D->position = point3d(-0.75f, 0.85f, 0.0f);
	transform2D->scale = point3d(0.05f, 0.05f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.75f, 0.0f, 0.0f);

	// Enemy name - text
	entity = m_World->entityStorage->CreateEntity("Rect", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(0, 0, 0);
	transform2D->ratio = ScreenAspectRatio::XX;
	transform2D->position = point3d(0, 0.85f, 0.0f);
	transform2D->scale = point3d(0.2f, 0.05f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.75f, 0.0f, 0.0f);

	// Enemy HP bar - rectangle
	entity = m_World->entityStorage->CreateEntity("BossHealth", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(0, 0, 0);
	transform2D->ratio = ScreenAspectRatio::XY;
	transform2D->position = point3d(0, 0.7f, 0.0f);
	transform2D->scale = point3d(0.5f, 0.025f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.75f, 0.0f, 0.0f);

	/*entity = m_World->entityStorage->CreateEntity("BossHealth", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(1, 0, 0);
	transform2D->ratio = ScreenAspectRatio::YY;
	transform2D->position = point3d(0.75f, 0.0f, 0.0f);
	transform2D->scale = point3d(0.2f, 0.2f, 0.0f);
	ImageLabel* imageLabel = entity->AddComponent<ImageLabel>();
	imageLabel->textureName = "comicsSpot";
	imageLabel->color = point3d(0.12, 0.91, 0.62);*/
	//////////////////////////////////////////////////////////////////////
	Entity* bossUIContainer = m_World->entityStorage->CreateEntity("BossUIContainer", uiFolder);

	// Фон полоски здоровья босса
	entity = m_World->entityStorage->CreateEntity("BossHealthBg", bossUIContainer);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(0, 0, 0);
	transform2D->ratio = ScreenAspectRatio::XY;
	transform2D->position = point3d(0, 0.7f, 0.0f);
	transform2D->scale = point3d(0.5f, 0.035f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.2f, 0.2f, 0.2f);
	rect->opacity = 0.7f;

	// Сама полоска здоровья (будет менять ширину)
	entity = m_World->entityStorage->CreateEntity("BossHealthBar", bossUIContainer);
	m_BossHealthFill = entity;  // Сохраняем указатель на полоску
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(-1, 0, 0);  // Привязка к левому краю
	transform2D->ratio = ScreenAspectRatio::XY;
	transform2D->position = point3d(-0.5f, 0.7f, 0.0f);
	transform2D->scale = point3d(0.5f, 0.035f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.8f, 0.2f, 0.2f);

	// Имя босса
	entity = m_World->entityStorage->CreateEntity("BossName", bossUIContainer);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(0, 0, 0);
	transform2D->ratio = ScreenAspectRatio::XY;
	transform2D->position = point3d(-0.16f, 0.91f, 0.0f);
	textLabel = entity->AddComponent<TextLabel>();
	textLabel->textW = L"ГИПЕРЗВЕЗДА";
	textLabel->fontFamilyW = L"Impact";
	textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	textLabel->fontWeight = 900;
	textLabel->fontSizePx = 52;
	textLabel->fontScale = 1.0f;
	textLabel->color = point3d(1.0f, 0.85f, 0.2f);
	textLabel->opacity = 1.0f;

	// Цифры здоровья (текущее / максимальное)
	entity = m_World->entityStorage->CreateEntity("BossHealthNumbers", bossUIContainer);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(0, 0, 0);
	transform2D->ratio = ScreenAspectRatio::XY;
	transform2D->position = point3d(-0.1f, 0.74f, 0.0f);
	m_BossNumbersText = entity->AddComponent<TextLabel>();  // Сохраняем указатель
	m_BossNumbersText->textW = L"2000 / 2000";
	m_BossNumbersText->fontFamilyW = L"Impact";
	m_BossNumbersText->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	m_BossNumbersText->fontWeight = 700;
	m_BossNumbersText->fontSizePx = 36;
	m_BossNumbersText->fontScale = 1.0f;
	m_BossNumbersText->color = point3d(1.0f, 1.0f, 1.0f);
	m_BossNumbersText->opacity = 0.9f;

	// Изначально скрываем весь UI босса
	bossUIContainer->SetActive(false);

	entity = m_World->entityStorage->CreateEntity("ExecutionLabel", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->ratio = ScreenAspectRatio::XY;
	transform2D->position = point3d(0.9f, -0.74f, 0.0f);
	textLabel = entity->AddComponent<TextLabel>();
	textLabel->textW = L"НИЧЕГО";
	textLabel->fontFamilyW = L"Impact";
	textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	textLabel->fontWeight = 900;
	textLabel->fontSizePx = 38;
	textLabel->fontScale = 0.34f;
	textLabel->letterSpacingPx = 1.0f;




}




void LevelManagerClass::CreateSpaceBackground(Entity* folder, int quality)
{
	int outerSpace_cnt = 6853 / quality;

	Entity* entity = m_World->entityStorage->CreateEntity("OuterSpace", folder);

	Nebula* nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 25;
	nebula->count = outerSpace_cnt;
	nebula->mode = pMode::point;
	nebula->isOnBackground = true;
}


void LevelManagerClass::CreateAries(Entity* folder)
{
	Entity* entity;
	Constellation* constellation;
	Transform* transform;
	PhysicBody* physicBody;

	SphereCollider* sphereCollider;

	PointCloud* pointCloud;
	DelayedDestroy* delayedDestroy;
	Health* health;
	CameraTarget* cameraTarget;

	Entity* aries = m_World->entityStorage->CreateEntity("Aries", folder);
	transform = aries->AddComponent<Transform>();
	transform->scale = point3d(4, 4, 4);
	transform->position = point3d(0.0f, 20.0f, 50.0f);
	//pointCloud = aries->AddComponent<PointCloud>();
	//pointCloud->index = 2;
	//pointCloud->pointSize = 1.0f;
	//pointCloud->brightness = 0.4f;
	////pointCloud->color = point3d(1, 0.2, 0.25);
	//pointCloud->instances = 1;
	//pointCloud->frustumRadius = 8;
	//pointCloud->compress = RenderCompress::x2;
	/*health = aries->AddComponent<Health>();
	health->hp = 1000;
	health->maxHp = 1000;
	cameraTarget = aries->AddComponent<CameraTarget>();*/

	/*constellation = aries->AddComponent<Constellation>();
	constellation->starSize = 0.04f;
	constellation->stars = {
		point3d(0, 2.5f, 3.0f),
		point3d(0, 1.7f, -1.4f),
		point3d(0, 0.4f, -3.9f),
		point3d(0, -0.9f, -6.0f),

		point3d(1.8f, -2.0f, 0.4f),
		point3d(-1.8f, -2.0f, 0.4f),
		point3d(1.9f, -4.2f, 0.6f),
		point3d(-1.9f, -4.2f, 0.6f),
		point3d(1.9f, -6.6f, 1.1f),
		point3d(-1.9f, -6.6f, 1.1f),

		point3d(1.8f, -2.0f, -6.0f),
		point3d(-1.8f, -2.0f, -6.0f),
		point3d(1.6f, -3.5f, -6.8f),
		point3d(-1.6f, -3.5f, -6.8f),
		point3d(1.6f, -6.6f, -6.6f),
		point3d(-1.6f, -6.6f, -6.6f),
	};
	constellation->links = {
		{0,1},
		{1,2},
		{2,3},

		{1,4},
		{1,5},
		{4,6},
		{5,7},
		{6,8},
		{7,9},

		{3,10},
		{3,11},
		{10,12},
		{11,13},
		{12,14},
		{13,15},
	};*/

	//entity = m_World->entityStorage->CreateEntity("Armor", aries);
	//transform = entity->AddComponent<Transform>();
	//pointCloud = entity->AddComponent<PointCloud>();
	//pointCloud->index = 3;
	//pointCloud->pointSize = 0.75f;
	//pointCloud->brightness = 0.2f;
	//pointCloud->color = point3d(1, 0.9f, 0.2f);
	//pointCloud->frustumRadius = 8;
	////pointCloud->compress = RenderCompress::x2;

	////Body

	//entity = m_World->entityStorage->CreateEntity("Collider", aries);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(0, 0.08, 0.1);
	//sphereCollider = entity->AddComponent<SphereCollider>();
	//sphereCollider->softness = 0.7;
	//sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	//sphereCollider->radius = 3;

	//entity = m_World->entityStorage->CreateEntity("Collider", aries);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(0, -0.05, -1.5);
	//sphereCollider = entity->AddComponent<SphereCollider>();
	//sphereCollider->softness = 0.7;
	//sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	//sphereCollider->radius = 2.2;

	//entity = m_World->entityStorage->CreateEntity("Collider", aries);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(0, -0.1, -0.7);
	//sphereCollider = entity->AddComponent<SphereCollider>();
	//sphereCollider->softness = 0.7;
	//sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	//sphereCollider->radius = 2.7;

	////Head

	//entity = m_World->entityStorage->CreateEntity("Collider", aries);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(0, 0.9, 0.8);
	//sphereCollider = entity->AddComponent<SphereCollider>();
	//sphereCollider->softness = 0.7;
	//sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	//sphereCollider->radius = 2;

	//entity = m_World->entityStorage->CreateEntity("Collider", aries);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(0, 0.7, 1.4);
	//sphereCollider = entity->AddComponent<SphereCollider>();
	//sphereCollider->softness = 0.7;
	//sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	//sphereCollider->radius = 1.1;

	////Horns

	//entity = m_World->entityStorage->CreateEntity("Collider", aries);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(0.5, 1.2, 0.5);
	//sphereCollider = entity->AddComponent<SphereCollider>();
	//sphereCollider->softness = 0.7;
	//sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	//sphereCollider->radius = 1.6;

	//entity = m_World->entityStorage->CreateEntity("Collider", aries);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(-0.5, 1.2, 0.5);
	//sphereCollider = entity->AddComponent<SphereCollider>();
	//sphereCollider->softness = 0.7;
	//sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	//sphereCollider->radius = 1.6;
}

void LevelManagerClass::CreateArenaBarrier(Entity* parent, const point3d& center, float radius, int starCount)
{
	// Создаём родительскую сущность для барьера
	Entity* barrier = m_World->entityStorage->CreateEntity("ArenaBarrier", parent);
	barrier->AddComponent<Transform>();  // Просто контейнер

	// Параметры барьера
	float heightMin = -40.0f;
	float heightMax = 40.0f;
	float radiusInner = radius - 2.0f;
	float radiusOuter = radius + 2.0f;

	// Количество звёзд в окружности (на каждый градус примерно)
	int starsPerCircle = starCount / 4;

	// ===== 1. НИЖНЯЯ ОКРУЖНОСТЬ (на земле) =====
	for (int i = 0; i < starsPerCircle; i++)
	{
		float angle = (i * 2.0f * PI / starsPerCircle);
		float x = center.x + cos(angle) * radius;
		float z = center.z + sin(angle) * radius;

		Entity* starEntity = m_World->entityStorage->CreateEntity("BarrierStar", barrier);
		Transform* starTransform = starEntity->AddComponent<Transform>();
		starTransform->position = point3d(x, center.y + heightMin, z);

		Star* star = starEntity->AddComponent<Star>();
		star->radius = 0.6f;
		star->crownRadius = 0.9f;
		star->color1 = point3d(0.3f, 0.6f, 1.0f);  // Голубой
		star->color2 = point3d(0.1f, 0.3f, 0.8f);
		star->crownColor = point3d(0.5f, 0.8f, 1.0f);

		// Добавляем мерцание (через ParticleEmitter)
		ParticleEmitter* particles = starEntity->AddComponent<ParticleEmitter>();
		particles->rate = 30;
		particles->lifetime = 500;
		particles->color = point3d(0.3f, 0.6f, 1.0f);
		particles->size = { 0.1f, 0.3f };
		particles->opacity = { 0.8f, 0.0f };
		particles->emitDirection = EmitDirection::Up;
		particles->spread = { 1.57f, 1.57f };
		particles->speed = { 2.0f, 1.0f };
	}

	// ===== 2. ВЕРХНЯЯ ОКРУЖНОСТЬ =====
	for (int i = 0; i < starsPerCircle; i++)
	{
		float angle = (i * 2.0f * PI / starsPerCircle);
		float x = center.x + cos(angle) * radius;
		float z = center.z + sin(angle) * radius;

		Entity* starEntity = m_World->entityStorage->CreateEntity("BarrierStar", barrier);
		Transform* starTransform = starEntity->AddComponent<Transform>();
		starTransform->position = point3d(x, center.y + heightMax, z);

		Star* star = starEntity->AddComponent<Star>();
		star->radius = 0.6f;
		star->crownRadius = 0.9f;
		star->color1 = point3d(1.0f, 0.4f, 0.8f);  // Розовый
		star->color2 = point3d(0.8f, 0.2f, 0.5f);
		star->crownColor = point3d(1.0f, 0.6f, 1.0f);

		ParticleEmitter* particles = starEntity->AddComponent<ParticleEmitter>();
		particles->rate = 30;
		particles->lifetime = 500;
		particles->color = point3d(1.0f, 0.4f, 0.8f);
		particles->size = { 0.1f, 0.3f };
		particles->opacity = { 0.8f, 0.0f };
		particles->emitDirection = EmitDirection::Up;
		particles->spread = { 1.57f, 1.57f };
		particles->speed = { 2.0f, 1.0f };
	}

	// ===== 3. ВЕРТИКАЛЬНЫЕ СТОЛБЫ ПО УГЛАМ (4 угла) =====
	int starsPerColumn = starCount / 8;
	float corners[4][2] = {
		{center.x + radius, center.z + radius},
		{center.x + radius, center.z - radius},
		{center.x - radius, center.z + radius},
		{center.x - radius, center.z - radius}
	};

	for (int c = 0; c < 4; c++)
	{
		for (int i = 0; i <= starsPerColumn; i++)
		{
			float t = (float)i / starsPerColumn;  // 0..1
			float y = center.y + heightMin + t * (heightMax - heightMin);

			Entity* starEntity = m_World->entityStorage->CreateEntity("BarrierStar", barrier);
			Transform* starTransform = starEntity->AddComponent<Transform>();
			starTransform->position = point3d(corners[c][0], y, corners[c][1]);

			Star* star = starEntity->AddComponent<Star>();
			star->radius = 0.5f;
			star->crownRadius = 0.7f;

			// Градиент цвета от низа к верху
			if (t < 0.5f)
			{
				star->color1 = point3d(0.3f, 0.6f, 1.0f);
				star->crownColor = point3d(0.5f, 0.8f, 1.0f);
			}
			else
			{
				star->color1 = point3d(1.0f, 0.4f, 0.8f);
				star->crownColor = point3d(1.0f, 0.6f, 1.0f);
			}
			star->color2 = star->color1 * 0.5f;

			ParticleEmitter* particles = starEntity->AddComponent<ParticleEmitter>();
			particles->rate = 20;
			particles->lifetime = 400;
			particles->color = star->color1;
			particles->size = { 0.05f, 0.2f };
			particles->opacity = { 0.6f, 0.0f };
			particles->emitDirection = EmitDirection::Up;
			particles->spread = { 1.57f, 1.57f };
			particles->speed = { 1.0f, 0.5f };
		}
	}

	// ===== 4. ЛЕТАЮЩИЕ ЗВЁЗДЫ ВДОЛЬ ГРАНИЦ (эффект барьера) =====
	int flyingStarsCount = starCount / 3;
	for (int i = 0; i < flyingStarsCount; i++)
	{
		Entity* starEntity = m_World->entityStorage->CreateEntity("FlyingBarrierStar", barrier);
		Transform* starTransform = starEntity->AddComponent<Transform>();

		// Случайная позиция на границе
		float angle = (rand() % 360) * PI / 180.0f;
		float x = center.x + cos(angle) * radius;
		float z = center.z + sin(angle) * radius;
		float y = center.y + heightMin + (rand() % (int)(heightMax - heightMin));

		starTransform->position = point3d(x, y, z);

		Star* star = starEntity->AddComponent<Star>();
		star->radius = 0.4f;
		star->crownRadius = 0.6f;
		star->color1 = point3d(0.5f, 0.7f, 1.0f);
		star->color2 = point3d(0.3f, 0.4f, 0.8f);
		star->crownColor = point3d(0.7f, 0.9f, 1.0f);

		// Движение по кругу
		PhysicBody* physicBody = starEntity->AddComponent<PhysicBody>();
		float speed = 5.0f + (rand() % 50) / 10.0f;
		point3d tangent = point3d(-sin(angle), 0, cos(angle));
		physicBody->velocity = tangent * speed;

		// Ограничитель для кругового движения
		struct CircleMover {
			point3d center;
			float radius;
			float speed;
			float angle;
		};

		// Сохраняем параметры в userData (или создаём отдельный компонент для движения по кругу)
		// Для простоты - пусть летят по прямой, будут сталкиваться с углами
	}

	// ===== 5. ДОПОЛНИТЕЛЬНО: ЛУЧИ МЕЖДУ ЗВЁЗДАМИ (опционально, через Beam) =====
	// Соединяем соседние звёзды в нижней окружности лучами
	for (int i = 0; i < starsPerCircle; i++)
	{
		float angle1 = (i * 2.0f * PI / starsPerCircle);
		float angle2 = ((i + 1) * 2.0f * PI / starsPerCircle);

		float x1 = center.x + cos(angle1) * radius;
		float z1 = center.z + sin(angle1) * radius;
		float x2 = center.x + cos(angle2) * radius;
		float z2 = center.z + sin(angle2) * radius;

		Entity* beamEntity = m_World->entityStorage->CreateEntity("BarrierBeam", barrier);
		Transform* beamTransform = beamEntity->AddComponent<Transform>();

		Beam* beam = beamEntity->AddComponent<Beam>();
		beam->point1 = point3d(x1, center.y + heightMin, z1);
		beam->point2 = point3d(x2, center.y + heightMin, z2);
		beam->size1 = 0.1f;
		beam->size2 = 0.1f;
		beam->color1 = point3d(0.3f, 0.5f, 0.9f);
		beam->color2 = point3d(0.3f, 0.5f, 0.9f);
		beam->opacity1 = 0.5f;
		beam->opacity2 = 0.5f;

		// Автоудаление не нужно, пусть висит
	}

	// Соединяем нижнюю и верхнюю окружность вертикальными лучами
	for (int i = 0; i < starsPerCircle; i += 5)  // Каждую 5-ю звезду, чтобы не перегружать
	{
		float angle = (i * 2.0f * PI / starsPerCircle);
		float x = center.x + cos(angle) * radius;
		float z = center.z + sin(angle) * radius;

		Entity* beamEntity = m_World->entityStorage->CreateEntity("VerticalBeam", barrier);
		Transform* beamTransform = beamEntity->AddComponent<Transform>();

		Beam* beam = beamEntity->AddComponent<Beam>();
		beam->point1 = point3d(x, center.y + heightMin, z);
		beam->point2 = point3d(x, center.y + heightMax, z);
		beam->size1 = 0.08f;
		beam->size2 = 0.08f;
		beam->color1 = point3d(0.4f, 0.6f, 1.0f);
		beam->color2 = point3d(0.6f, 0.4f, 1.0f);
		beam->opacity1 = 0.4f;
		beam->opacity2 = 0.4f;
	}
}

void LevelManagerClass::CreateZenithLocation(Entity* folder, int quality)
{
	Entity* entity;
	Nebula* nebula;
	Transform* transform;
	SphereCollider* sphereCollider;
	MultiDamager* multiDamager;
	Star* star;

	int pillars_cnt = 3725470 / 2 / quality;
	int galaxy_cnt = 182361 / quality;

	Entity* location = m_World->entityStorage->CreateEntity("Zenith location", folder);

	transform = location->AddComponent<Transform>();
	transform->position = point3d(0, 0, 200);

	// Pillars hand | point
	entity = m_World->entityStorage->CreateEntity("PHP", folder);
	transform = entity->AddComponent<Transform>();

	nebula = location->AddComponent<Nebula>();
	nebula->vShader = 24;
	nebula->count = pillars_cnt;
	nebula->mode = pMode::point;
	nebula->scale = 5;
	nebula->frustumRadius = 40;

	// Inside nebula | point
	entity = m_World->entityStorage->CreateEntity("INP", location);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(150, 0, 0);

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 23;
	nebula->count = pillars_cnt;
	nebula->mode = pMode::point;
	nebula->color = point3d(0.3, 0.5, 1.0);
	nebula->scale = 10;
	nebula->frustumRadius = 40;

	// Pillars hand | glow
	entity = m_World->entityStorage->CreateEntity("PHG", location);
	transform = entity->AddComponent<Transform>();

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 24;
	nebula->count = pillars_cnt;
	nebula->skipper = 1394 / 2;
	nebula->mode = pMode::glow;
	nebula->scale = 5;
	nebula->frustumRadius = 40;

	// Inside nebula | glow
	entity = m_World->entityStorage->CreateEntity("ING", location);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(150, 0, 0);

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 23;
	nebula->count = pillars_cnt;
	nebula->skipper = 1394;
	nebula->mode = pMode::glow;
	nebula->color = point3d(0.3, 0.5, 1.0);
	nebula->scale = 10;
	nebula->frustumRadius = 40;

	// 2 Nebula
	entity = m_World->entityStorage->CreateEntity("INP1", location);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0, 0, 0);

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 23;
	nebula->count = pillars_cnt;
	nebula->mode = pMode::point;
	nebula->color = point3d(0.8, 0.4, 0.2);
	nebula->scale = 10;
	nebula->frustumRadius = 40;

	entity = m_World->entityStorage->CreateEntity("ING1", location);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0, 0, 0);

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 23;
	nebula->count = pillars_cnt;
	nebula->skipper = 1394;
	nebula->mode = pMode::glow;
	nebula->color = point3d(0.8, 0.4, 0.2);
	nebula->scale = 10;
	nebula->frustumRadius = 40;

	// Звезда с гравитацией в локации

	//entity = m_World->entityStorage->CreateEntity("Gravity", location);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(0, 0, 0);

	//sphereCollider = entity->AddComponent<SphereCollider>();
	//sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	//sphereCollider->radius = 20.0f;
	//
	//star = entity->AddComponent<Star>();
	//star->radius = 20.0f;
	//star->crownRadius = 21.5f;
	//star->color1 = point3d(0.1f, 0.1f, 0.1f);
	//star->color2 = point3d(0.4f, 0.4f, 0.4f);
	//star->crownColor = point3d(0.5f, 0.5f, 0.5f);

	//GravityPoint* gravityPoint = entity->AddComponent<GravityPoint>();
	//gravityPoint->mass = 100;
	//gravityPoint->radius = 150;



	CreateArenaBarrier(location, point3d(0, 0, 0), 52.0f, 200);
	m_BossArenaCenter = point3d(0, 0, 200);
	m_BossArenaRadius = 60.0f;

	// ========== БОСС ==========
	Entity* BossEntity = m_World->entityStorage->CreateEntity("BossEnemy", location);
	m_CurrentBoss = BossEntity;

	Transform* bossTransform = BossEntity->AddComponent<Transform>();
	bossTransform->position = point3d(0.0f, 0.0f, 0.0f); 

	// === КОЛЛАЙДЕР ===
	sphereCollider = BossEntity->AddComponent<SphereCollider>();
	sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	sphereCollider->radius = 3.5f;
	BossEntity->AddComponent<CameraTarget>();

	// === ЗДОРОВЬЕ ===
	Health* health = BossEntity->AddComponent<Health>();
	health->fraction = Fraction::Enemy;
	health->maxHp = 2000.0f;
	health->hp = 2000.0f;

	// === ФИЗИКА ===
	PhysicBody* testPhysic = BossEntity->AddComponent<PhysicBody>();
	testPhysic->airFriction = 0.1f;
	testPhysic->mass = 100.0f;
	testPhysic->velocity = point3d(0.0f, 0.0f, 0.0f);

	/*GravityPoint* gravityP = BossEntity->AddComponent<GravityPoint>();
	gravityP->radius = 50;
	gravityP->mass = 100;*/

	// === ВИЗУАЛ ===
	Star* testStar = BossEntity->AddComponent<Star>();
	testStar->radius = 3.0f;
	testStar->crownRadius = 3.5f;
	testStar->color1 = point3d(0.8f, 0.2f, 0.8f);
	testStar->color2 = point3d(0.5f, 0.1f, 0.5f);
	testStar->crownColor = point3d(0.3f, 0.6f, 0.8f);

	// === КОМПОНЕНТ ИИ ===
	AIComponent* ai = BossEntity->AddComponent<AIComponent>();
	ai->enabled = true;
	ai->behaviorType = AIBehaviorType::BOSS_PHASE_1;

	ai->detectionRange = 50.0f;      // Большой радиус обнаружения
	ai->chaseRange = 80.0f;
	ai->attackRange = 4.0f;

	ai->movementSpeed = 8.0f;
	ai->arrivalDistance = 2.0f;
	ai->accelerationStrength = 5.0f;
	ai->maxAcceleration = 20.0f;

	ai->attackCooldown = 1.2f;
	ai->attackDamage = 20.0f;

	ai->stateTimer = 0.0f;
	ai->searchDuration = 5.0f;
	ai->searchPatrolRadius = 15.0f;
	ai->targetId = -1;

	ai->patrolPoints = {
		point3d(-5.0f, 0.0f, -5.0f),
		point3d(5.0f, 0.0f, -5.0f),
		point3d(5.0f, 0.0f, 5.0f),
		point3d(-5.0f, 0.0f, 5.0f)
	};
	ai->currentPatrolIndex = 0;

	ai->visual.originalRadius = testStar->radius;
	ai->visual.originalColor = testStar->color1;
	ai->visual.attackScale = 1.5f;
	ai->visual.attackDuration = 0.3f;
	ai->visual.specialCastDuration = 0.5f;
	ai->visual.specialAttackColor = point3d(1.0f, 0.3f, 0.8f);
	ai->visual.aoePulseSpeed = 5.0f;

	// === КОМПОНЕНТ БОССА (РАСКОММЕНТИРОВАН) ===
	BossComponent* boss = BossEntity->AddComponent<BossComponent>();

	// Фазы
	boss->currentPhase = 3;
	boss->phaseHealthThresholds[0] = 0.7f;  // 1400 HP
	boss->phaseHealthThresholds[1] = 0.3f;  // 600 HP

	// АРЕНА 100x100x100 (относительно центра арены 0,0,200)
	boss->arenaMinX = -50.0f;
	boss->arenaMaxX = 50.0f;
	boss->arenaMinY = -50.0f;
	boss->arenaMaxY = 50.0f;
	boss->arenaMinZ = -50.0f;
	boss->arenaMaxZ = 50.0f;

	// Атаки
	boss->dashCooldown = 3.0f;
	boss->dashSpeed = 100.0f;
	boss->dashDamage = 5.0f;

	boss->starShotCooldown = 5.5f;
	boss->starShotCount = 20.f;
	boss->starShotSpeed = 15.0f;
	boss->starShotDamage = 0.5f;

	boss->sideDashCooldown = 4.5f;
	boss->sideDashSpeed = 10.0f;

	boss->aoeAttackRange = 12.0f;
	boss->aoeDamage = 15.0f;

	boss->specialAttackCooldown = 10.0f;
	boss->rageSpeedMultiplier = 1.0f;

	// Обнуляем таймеры (ВАЖНО!)
	boss->lastSpecialAttackTime = 0.0f;
	boss->lastDashTime = 0.0f;
	boss->lastSideDashTime = 0.0f;
	boss->lastStarShotTime = 0.0f;



}

void LevelManagerClass::CreateNebula(Entity* folder, int quality) {

	Entity* entity;
	Nebula* nebula;
	Transform* transform;
	SphereCollider* sphereCollider;
	MultiDamager* multiDamager;

	int pillars_cnt = 3725470 / 2 / quality;
	int galaxy_cnt = 182361 / quality;

	Entity* location = m_World->entityStorage->CreateEntity("Zenith location", folder);

	transform = location->AddComponent<Transform>();
	transform->position = point3d(0, 50, 0);

	// first nebula Heal
	entity = m_World->entityStorage->CreateEntity("INP", location);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(-150, 50, 0);

	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->radius = 30.f;
	sphereCollider->isTouchable = false;

	multiDamager = entity->AddComponent<MultiDamager>();
	multiDamager->damage = -5;
	multiDamager->repeats = -1;
	multiDamager->inverval = 1000;

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 26;
	nebula->count = pillars_cnt;
	nebula->mode = pMode::point;
	nebula->color = point3d(1, 0.3, 0.5);	// Розоватый
	nebula->color = point3d(0.2, 0.8, 0.3); // Зеленый
	nebula->color = point3d(0.3, 0.5, 1.0); // Голубой
	nebula->scale = 1;
	nebula->frustumRadius = 40;

	// first nebula Heal Glow
	entity = m_World->entityStorage->CreateEntity("ING", location);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(-150, 50, 0);


	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 26;
	nebula->count = pillars_cnt;
	nebula->skipper = 1394;
	nebula->mode = pMode::glow;
	nebula->color = point3d(1, 0.3, 0.5);   // Розоватый
	nebula->color = point3d(0.2, 0.8, 0.3); // Зеленый
	nebula->color = point3d(0.3, 0.5, 1.0); // Голубой
	nebula->scale = 1;
	nebula->frustumRadius = 40;


	// First Nebula DMG

	entity = m_World->entityStorage->CreateEntity("INP1", location);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(-450, 50, 0);

	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->radius = 30.f;
	sphereCollider->isTouchable = false;

	multiDamager = entity->AddComponent<MultiDamager>();
	multiDamager->damage = 5;
	multiDamager->repeats = -1;
	multiDamager->inverval = 1000;

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 26;
	nebula->count = pillars_cnt;
	nebula->mode = pMode::point;
	nebula->color = point3d(0.8, 0.4, 0.2);
	nebula->scale = 1;
	nebula->frustumRadius = 40;
	nebula->isInteractive = true;

	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->radius = 40;
	sphereCollider->isTouchable = false;

	// First Nebula DMG Glow
	entity = m_World->entityStorage->CreateEntity("ING1", location);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(-450, 50, 0);

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 26;
	nebula->count = pillars_cnt;
	nebula->skipper = 1394;
	nebula->mode = pMode::glow;
	nebula->color = point3d(0.8, 0.4, 0.2);
	nebula->scale = 1;
	nebula->frustumRadius = 40;


	// Test Nebula

	entity = m_World->entityStorage->CreateEntity("INP1", location);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0, 50, 0);

	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->radius = 30.f;
	sphereCollider->isTouchable = false;

	multiDamager = entity->AddComponent<MultiDamager>();
	multiDamager->damage = 5;
	multiDamager->repeats = -1;
	multiDamager->inverval = 1000;

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 29;
	nebula->count = pillars_cnt;
	nebula->mode = pMode::point;
	nebula->color = point3d(0.6, 0.3, 0.1);

	nebula->scale = 1;
	nebula->frustumRadius = 40;



	// СОЗДАНИЕ ПОРТАЛА
	//Entity* portal = m_World->entityStorage->CreateEntity("CosmicPortal", folder);
	//Transform* portalTransform = portal->AddComponent<Transform>();
	//portalTransform->position = point3d(0, 0, 0);  // Позиция портала

	//// Добавляем Nebula компонент с шейдером портала
	//Nebula* portalNebula = portal->AddComponent<Nebula>();
	//portalNebula->vShader = 32;      // Ваш вершинный шейдер портала
	////portalNebula->pShader = 23;      // Пиксельный шейдер (базовый)
	//portalNebula->count = 50000;     // Количество частиц
	//portalNebula->mode = pMode::point; // Режим отображения (точки)
	//portalNebula->scale = 2.0f;      // Масштаб портала
	//portalNebula->frustumRadius = 30.0f; // Радиус отсечения

	//// Добавляем свечение портала
	//Entity* portalGlow = m_World->entityStorage->CreateEntity("PortalGlow", portal);
	//Nebula* glowNebula = portalGlow->AddComponent<Nebula>();
	//glowNebula->vShader = 32;
	////glowNebula->pShader = 24;        // Шейдер для свечения
	//glowNebula->count = 50000;
	//glowNebula->skipper = 1394;      // Пропуск частиц для эффекта свечения
	//glowNebula->mode = pMode::glow;  // Режим свечения
	//glowNebula->scale = 2.2f;        // Чуть больше основного портала
	//glowNebula->frustumRadius = 35.0f;

	//// Добавляем коллайдер для взаимодействия
	//SphereCollider* portalCollider = portal->AddComponent<SphereCollider>();
	//portalCollider->radius = 25.0f;
	//portalCollider->isTouchable = true;
	//portalCollider->collisionGroup = CollisionFilter::Group::Player;

	//// Делаем портал видимым для камеры
	//CameraTarget* cameraTarget = portal->AddComponent<CameraTarget>();
	//cameraTarget->active = true;

}



void LevelManagerClass::CreateStarQuestLoc(Entity* folder, int quality)
{
	Entity* location = m_World->entityStorage->CreateEntity("StarQuestLocation", folder);
	Transform* transform = location->AddComponent<Transform>();
	transform->position = point3d(0, 0, 150);

	
	Entity* m_CentralStar = m_World->entityStorage->CreateEntity("CentralStar", location);
	transform = m_CentralStar->AddComponent<Transform>();
	transform->position = point3d(0, 0, 0);

	QuestGiver* questGiver = m_CentralStar->AddComponent<QuestGiver>();
	questGiver->radius = 40;
	questGiver->questType = "QuestStarCollection";

	Star* star = m_CentralStar->AddComponent<Star>();
	star->radius = 20.0f;
	star->crownRadius = 25.0f;
	star->color1 = point3d(0.99, 1, 0.51);
	star->color2 = point3d(0.75f, 0.2f, 0.37f);
	star->crownColor = point3d(0.87f, 0.25f, 0.15f);

	SphereCollider* sphereCollider = m_CentralStar->AddComponent<SphereCollider>();
	sphereCollider->radius = 20.0f;
	sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	sphereCollider->isTouchable = true;

	Health* health = m_CentralStar->AddComponent<Health>();
	health->hp = 100;
	health->maxHp = 100;
	health->fraction = Fraction::Player;

	m_CentralStar->AddComponent<Grabbable>();


	Entity* collectionTrigger = m_World->entityStorage->CreateEntity("CollectionTrigger", m_CentralStar);
	Transform* triggerTransform = collectionTrigger->AddComponent<Transform>();
	triggerTransform->position = point3d(0, 0, 0);

	SphereCollider* triggerCollider = collectionTrigger->AddComponent<SphereCollider>();
	triggerCollider->radius = 40;
	triggerCollider->collisionGroup = CollisionFilter::Group::Projectile;
	triggerCollider->isTouchable = false;
}

void LevelManagerClass::ShowGameOverMessage(const wchar_t* message, const point3d& color)
{
	// Создаём UI контейнер для сообщения
	Entity* gameOverContainer = m_World->entityStorage->CreateEntity("GameOverContainer", nullptr);
	Transform2D* containerTransform = gameOverContainer->AddComponent<Transform2D>();
	containerTransform->anchorPoint = point3d(0, 0, 0);
	containerTransform->ratio = ScreenAspectRatio::XY;
	containerTransform->position = point3d(0, 0, 0);
	containerTransform->scale = point3d(1, 1, 0);

	// Фон
	Entity* bg = m_World->entityStorage->CreateEntity("GameOverBG", gameOverContainer);
	Transform2D* bgTransform = bg->AddComponent<Transform2D>();
	bgTransform->anchorPoint = point3d(0, 0, 0);
	bgTransform->ratio = ScreenAspectRatio::XY;
	bgTransform->position = point3d(-0.5f, -0.2f, 0);
	bgTransform->scale = point3d(1, 0.4f, 0);
	Rect* bgRect = bg->AddComponent<Rect>();
	bgRect->color = point3d(0, 0, 0);
	bgRect->opacity = 0.8f;

	// Текст сообщения
	Entity* text = m_World->entityStorage->CreateEntity("GameOverText", gameOverContainer);
	Transform2D* textTransform = text->AddComponent<Transform2D>();
	textTransform->anchorPoint = point3d(0, 0, 0);
	textTransform->ratio = ScreenAspectRatio::XY;
	textTransform->position = point3d(-0.25f, -0.1f, 0);
	TextLabel* textLabel = text->AddComponent<TextLabel>();
	textLabel->textW = message;
	textLabel->fontFamilyW = L"Impact";
	textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	textLabel->fontWeight = 900;
	textLabel->fontSizePx = 80;
	textLabel->fontScale = 1.5f;
	textLabel->color = color;
	textLabel->opacity = 1.0f;

	// Автоудаление через 3 секунды
	DelayedDestroy* delayed = gameOverContainer->AddComponent<DelayedDestroy>();
	delayed->lifeTime = 3000;
}

void LevelManagerClass::ShowExecutionUI()
{
	if (m_ExecutionUI) return;

	// Создаём UI для QTE
	m_ExecutionUI = m_World->entityStorage->CreateEntity("ExecutionUI", nullptr);

	// Фон
	Entity* bg = m_World->entityStorage->CreateEntity("ExecutionBG", m_ExecutionUI);
	Transform2D* bgTransform = bg->AddComponent<Transform2D>();
	bgTransform->anchorPoint = point3d(0, 0, 0);
	bgTransform->ratio = ScreenAspectRatio::XY;
	bgTransform->position = point3d(-0.4f, -0.15f, 0);
	bgTransform->scale = point3d(0.8f, 0.3f, 0);
	Rect* bgRect = bg->AddComponent<Rect>();
	bgRect->color = point3d(0, 0, 0);
	bgRect->opacity = 0.85f;
	bgRect->cornerRadius = 0.05f;

	// Текст "НАЖМИ R!"
	Entity* text = m_World->entityStorage->CreateEntity("ExecutionText", m_ExecutionUI);
	Transform2D* textTransform = text->AddComponent<Transform2D>();
	textTransform->anchorPoint = point3d(0, 0, 0);
	textTransform->ratio = ScreenAspectRatio::XY;
	textTransform->position = point3d(-0.2f, -0.05f, 0);
	TextLabel* label = text->AddComponent<TextLabel>();
	label->textW = L"НАЖМИ  R";
	label->fontFamilyW = L"Impact";
	label->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	label->fontWeight = 900;
	label->fontSizePx = 60;
	label->fontScale = 1.2f;
	label->color = point3d(1.0f, 0.8f, 0.2f);

	// Текст "ДЛЯ КАЗНИ"
	Entity* subText = m_World->entityStorage->CreateEntity("ExecutionSubText", m_ExecutionUI);
	Transform2D* subTransform = subText->AddComponent<Transform2D>();
	subTransform->anchorPoint = point3d(0, 0, 0);
	subTransform->ratio = ScreenAspectRatio::XY;
	subTransform->position = point3d(-0.25f, 0.05f, 0);
	TextLabel* subLabel = subText->AddComponent<TextLabel>();
	subLabel->textW = L"ДЛЯ КАЗНИ";
	subLabel->fontFamilyW = L"Impact";
	subLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	subLabel->fontWeight = 700;
	subLabel->fontSizePx = 35;
	subLabel->fontScale = 1.0f;
	subLabel->color = point3d(1.0f, 1.0f, 1.0f);

	// Таймер обратного отсчёта
	Entity* timerText = m_World->entityStorage->CreateEntity("ExecutionTimer", m_ExecutionUI);
	Transform2D* timerTransform = timerText->AddComponent<Transform2D>();
	timerTransform->anchorPoint = point3d(0, 0, 0);
	timerTransform->ratio = ScreenAspectRatio::XY;
	timerTransform->position = point3d(-0.05f, -0.2f, 0);
	TextLabel* timerLabel = timerText->AddComponent<TextLabel>();
	timerLabel->textW = L"5.0";
	timerLabel->fontFamilyW = L"Impact";
	timerLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	timerLabel->fontWeight = 700;
	timerLabel->fontSizePx = 40;
	timerLabel->fontScale = 1.0f;
	timerLabel->color = point3d(1.0f, 0.3f, 0.3f);
}

void LevelManagerClass::HideExecutionUI()
{
	if (m_ExecutionUI)
	{
		m_ExecutionUI->SetActive(false);
		m_ExecutionUI = nullptr;
	}
}

void LevelManagerClass::ExecuteBoss()
{
	if (!m_CurrentBoss) return;

	Health* bossHealth = m_CurrentBoss->GetComponent<Health>();
	if (bossHealth)
	{
		bossHealth->hp = 0;  // Убиваем босса
		//Log("BOSS EXECUTED!\n");
	}

	HideExecutionUI();
	m_IsExecutionActive = false;

	// Показываем победу
	Entity* msg = m_World->entityStorage->CreateEntity("VictoryMsg", nullptr);
	Transform2D* t = msg->AddComponent<Transform2D>();
	t->anchorPoint = point3d(0, 0, 0);
	t->ratio = ScreenAspectRatio::XY;
	t->position = point3d(-0.25f, -0.1f, 0);

	TextLabel* text = msg->AddComponent<TextLabel>();
	text->textW = L"ПОБЕДА!";
	text->fontFamilyW = L"Impact";
	text->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	text->fontWeight = 900;
	text->fontSizePx = 80;
	text->fontScale = 1.5f;
	text->color = point3d(0.2f, 0.8f, 0.2f);

	DelayedDestroy* d = msg->AddComponent<DelayedDestroy>();
	d->lifeTime = 3000;

	// Отключаем босса
	AIComponent* bossAI = m_CurrentBoss->GetComponent<AIComponent>();
	if (bossAI) bossAI->enabled = false;
}

void LevelManagerClass::TriggerExecution()
{
	if (m_IsExecutionActive) return;

	// Проверяем, что босс ещё жив
	Health* bossHealth = m_CurrentBoss->GetComponent<Health>();
	if (!bossHealth || bossHealth->hp <= 0) return;

	m_IsExecutionActive = true;
	m_ExecutionTimer = 5.0f;  // 5 секунд на казнь

	// Останавливаем босса
	AIComponent* bossAI = m_CurrentBoss->GetComponent<AIComponent>();
	if (bossAI) bossAI->enabled = false;

	// Визуальный эффект - босс становится беззащитным
	Star* bossStar = m_CurrentBoss->GetComponent<Star>();
	if (bossStar)
	{
		bossStar->color1 = point3d(0.5f, 0.5f, 0.5f);  // Серый
		bossStar->crownColor = point3d(0.3f, 0.3f, 0.3f);
	}

	// Показываем UI
	ShowExecutionUI();
}
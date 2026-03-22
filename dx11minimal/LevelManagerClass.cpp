#include "LevelManagerClass.h"

#include "Engine/Mesh/Animation.h"
#include "Engine/Mesh/Skeleton.h"
#include "Engine/Mesh/SkinnedMesh.h"
#include "dx11.h"
#include "Components/SkeletalAnimationComponent.h"
#include "Systems/SkinnedMeshSystem.h"
#include "Systems/SkeletalAnimationSystem.h"

LevelManagerClass::LevelManagerClass()
{
	window = 0;
	mouse = 0;

	m_World = 0;
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
	collisionManager->Initialize();

	mouse = Singleton::GetInstance<MouseClass>();
	mouse->Initialize();

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
	if (modelsLoadingThread.joinable())
	{
		modelsLoadingThread.join();
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


	Entity* folder = m_World->entityStorage->CreateEntity("World");
	folder->SetActive(true);

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

	/*entity = m_World->entityStorage->CreateEntity("Particles", folder);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(-10.0f, 0.0f, -20.0f);
	particleEmitter = entity->AddComponent<ParticleEmitter>();
	particleEmitter->rate = 100;
	particleEmitter->lifetime = 1000;
	particleEmitter->color = point3d(0.15f, 0.95f, 0.35f);
	particleEmitter->size = { 1.0f, 4.0f };
	particleEmitter->opacity = { 0.75f, 0.0f };
	particleEmitter->emitDirection = EmitDirection::Up;
	particleEmitter->speed = { 10.0f, 0.0f };
	particleEmitter->spread = { PI / 8, PI / 8 };*/

	//entity = m_World->entityStorage->CreateEntity("Particles", folder);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(-20.0f, 0.0f, -20.0f);
	//particleEmitter = entity->AddComponent<ParticleEmitter>();
	//particleEmitter->rate = 150;
	//particleEmitter->lifetime = 1000;
	//particleEmitter->color = point3d(1.0f, 0.15f, 0.1f);
	//particleEmitter->size = { 0.0f, 4.0f };
	//particleEmitter->opacity = { 1.0f, 0.0f };
	//particleEmitter->emitDirection = EmitDirection::Up;
	//particleEmitter->speed = { 10.0f, 0.0f };
	//particleEmitter->spread = { PI, PI };
	//particleEmitter->isReverse = true;

	//entity = m_World->entityStorage->CreateEntity("Particles", folder);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(-30.0f, 0.0f, -20.0f);
	//particleEmitter = entity->AddComponent<ParticleEmitter>();
	//particleEmitter->rate = 100;
	//particleEmitter->lifetime = 750;
	//particleEmitter->color = point3d(1, 1, 1);
	//particleEmitter->size = { 0.0f, 6.0f };
	//particleEmitter->opacity = { 0.75f, 0.0f };
	//particleEmitter->emitDirection = EmitDirection::Front;
	//particleEmitter->speed = { 15.0f, 0.0f };
	//particleEmitter->spread = { 0, PI };
	//particleEmitter->isHeapEmit = true;

	//entity = m_World->entityStorage->CreateEntity("Particles", folder);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(-40.0f, 0.0f, -20.0f);
	//particleEmitter = entity->AddComponent<ParticleEmitter>();
	//particleEmitter->rate = 100;
	//particleEmitter->lifetime = 1000;
	//particleEmitter->color = point3d(0.95f, 0.65f, 0.25f);
	//particleEmitter->size = { 3.0f, 0.0f };
	//particleEmitter->opacity = { 1.0f, 0.0f };
	//particleEmitter->emitDirection = EmitDirection::Up;
	//particleEmitter->speed = { 10.0f, 0.0f };

	//for (int i = 0; i < 5; i++) {
	//	entity = m_World->entityStorage->CreateEntity("Star", folder);
	//	transform = entity->AddComponent<Transform>();
	//	transform->position = point3d(10.0f + 5.0f * i, 0.0f, -35.0f);
	//	star = entity->AddComponent<Star>();
	//	star->radius = 2.0f;
	//	star->crownRadius = 3.5f;
	//	star->color1 = point3d(0.87f, 0.79f, 1.0f);
	//	star->color2 = point3d(0.7f, 0.0f, 0.47f);
	//	star->crownColor = point3d(0.47f, 0.65f, 1.0f);
	//	sphereCollider = entity->AddComponent<SphereCollider>();
	//	sphereCollider->radius = 2.0f;
	//	sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	//	health = entity->AddComponent<Health>();
	//	health->hp = 10;
	//	health->maxHp = 10;
	//}

	entity = m_World->entityStorage->CreateEntity("Star", folder);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(50, 0.0f, -35.0f);
	star = entity->AddComponent<Star>();
	star->radius = 8.0f;
	star->crownRadius = 12.0f;
	star->color1 = point3d(0.87f, 0.24f, 0.13f);
	star->color2 = point3d(0.35f, 0.0f, 0.07f);
	star->crownColor = point3d(0.87f, 0.25f, 0.15f);
	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->radius = 8.0f;
	sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	health = entity->AddComponent<Health>();
	health->hp = 100;
	health->maxHp = 100;

	/////////////////////////
	
	CreateSpaceBackground(folder, 1);
	CreateAries(folder);
	//CreateZenithLocation(folder, 2);
	CreateNebulas(folder, 2);

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

	/*if (aiSystem)
	{
		aiSystem->SetPlayerEntity(player);
	}*/

	auto spawnSkinned = [this, folder](
		const char* name,
		const point3d& pos,
		const point3d& scale,
		const SkinnedMesh& srcMesh,
		Skeleton* skeleton,
		std::vector<AnimationClip>* animations,
		const char* preferredClip = nullptr,
		bool autoPlay = true) -> Entity*
	{
		Entity* e = m_World->entityStorage->CreateEntity(name, folder);
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
		pointCloud->compress = RenderCompress::x2;
		pointCloud->frustumRadius = 12.0f;

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

	spawnSkinned("Fox", point3d(0.0f, 0.0f, 10.0f), point3d(0.2f, 0.2f, 0.2f), m_FoxMesh, &m_FoxSkeleton, &m_FoxAnimations);
	spawnSkinned("CesiumMan", point3d(3.0f, 0.0f, 10.0f), point3d(1.0f, 1.0f, 1.0f), m_CesiumMesh, &m_CesiumSkeleton, &m_CesiumAnimations);
	m_TestAnimEntity = spawnSkinned("TestMannequin", point3d(6.0f, 0.0f, 10.0f), point3d(1.0f, 1.0f, 1.0f), m_TestAnimMesh, &m_TestAnimSkeleton, &m_TestAnimAnimations, nullptr, false);
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

void LevelManagerClass::Frame()
{
	mouse->UpdateSystemCursorVisibility();
	if (!window->IsActive())
		return;

	mouse->Update();
	UpdateTestAnimationToggle();
	playerController->ProcessInput();
	playerController->ProcessMouse();
	playerController->abilities->Update();
	playerController->ProccessUI();


	//// FAST DEBUG CODE (DELETE LATER) ////
	/*count++;
	if (count > 30) {
		count = 0;

		Entity* projectile = m_World->entityStorage->CreateEntity("PlayerProjectile");
		Transform* transform = projectile->AddComponent<Transform>();
		transform->position = point3d(0, 23, 55);

		PhysicBody* physicBody = projectile->AddComponent<PhysicBody>();
		physicBody->airFriction = 0.0f;
		physicBody->velocity = transform->GetLookVector() * 25.0f;

		Star* star = projectile->AddComponent<Star>();
		star->radius = 1.0f;
		star->crownRadius = 1.5f;
		star->color1 = point3d(0.9f, 1.0f, 0.99f);
		star->color2 = point3d(0.34f, 0.8f, 0.45f);
		star->crownColor = point3d(0.27f, 0.63f, 1.0f);

		SingleDamager* singleDamager = projectile->AddComponent<SingleDamager>();
		singleDamager->target = Fraction::Player;
		singleDamager->damage = 5.0f;

		SphereCollider* sphereCollider = projectile->AddComponent<SphereCollider>();
		sphereCollider->isTouchable = false;
		sphereCollider->radius = 1.0f;

		DelayedDestroy* delayedDestroy = projectile->AddComponent<DelayedDestroy>();
		delayedDestroy->lifeTime = 5000;
	}*/
	//// FAST DEBUG CODE (DELETE LATER) ////

	// ??????????????????????????? ????????????????????? ????????? ???????????? ??????????????????
	//DWORD currentTime = timer::currentTime;
	//srand(time(0));
	// ??????????????????: ??????????????? ?????????????????? 3 ?????????????????????
	//if (currentTime - Star->LastTime > 3000) {
	//	Star->FartingEffect();
	//	// ??????????????????????????? ??????????????? ???????????????
	//	int attackType = rand() % 3;
	//	switch (attackType) {
	//	case 0:
	//		Star->Flash();
	//		break;
	//	case 1:
	//		Star->CoronalEjection();
	//		break;
	//	case 2:
	//		Star->SunWind();
	//		break;
	//	}
	//	//Star->LifeTimeParticl();
	//	Star->LastTime = currentTime;
	//}

	/*if (currentTime - smallConstellation->LastTime > 5000) {

		smallConstellation->LastTime = currentTime;

		int attackType = rand() % 3;

		switch (attackType) {
		case 0:
			smallConstellation->VolleyStart();
			break;
		case 1:
			smallConstellation->LatticeStart();
			break;
		case 2:
			smallConstellation->TransformationStart();
			break;
		}
	}
	smallConstellation->VolleyUpdate(0.01f);
	smallConstellation->LatticeUpdate(0.01f);
	smallConstellation->TransformationUpdate();
	smallConstellation->RamUpdate();*/

	ConstBuf::frame.aspect = XMFLOAT4{ float(window->aspect), float(window->iaspect), float(window->width), float(window->height) };

	m_World->UpdateCompute();
	m_World->UpdatePhysic();

	playerController->ProcessCamera();

	m_World->UpdateRender();

	mouse->RenderCursor();
	Draw::Present();
}


////////////////////////////////////////////////////////////////////////////////////////////////


void LevelManagerClass::LoadModels()
{
	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\HeroFists.obj");
	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\Arrow.obj");

	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\AriesBody.obj");
	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\AriesArmor.obj");
	Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\Fox.glb", m_FoxMesh, m_FoxSkeleton, m_FoxAnimations);
	Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\CesiumMan.glb", m_CesiumMesh, m_CesiumSkeleton, m_CesiumAnimations);
	if (Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\TestAnims\\1.glb", m_TestAnimMesh, m_TestAnimSkeleton, m_TestAnimAnimations))
	{
		Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\2.glb", m_TestAnimSkeleton, m_TestAnimAnimations, false);
		Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\3.glb", m_TestAnimSkeleton, m_TestAnimAnimations, true);
		Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\4.glb", m_TestAnimSkeleton, m_TestAnimAnimations, true);
	}
}

// TODO: Remove, only for test animation change
void LevelManagerClass::UpdateTestAnimationToggle()
{
	const bool isTogglePressed = input::IsKeyDown('T');
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

	Constellation* constellation = player->AddComponent<Constellation>();
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
	};

	PointCloud* pointCloud = player->AddComponent<PointCloud>();
	pointCloud->index = 0;
	pointCloud->position = point3d(-0.83, -0.8, 0);
	pointCloud->scale = point3d(0.3f, 0.3f, 0.3f);
	pointCloud->pointSize = 0.04f;
	pointCloud->brightness = 0.1f;
	pointCloud->color = point3d(1, 0.6f, 0.9f);
	pointCloud->compress = RenderCompress::x2;

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
	transform2D->scale = point3d(0.05f, 0.05f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.75f, 0.0f, 0.0f);

	// Enemy HP bar - rectangle
	entity = m_World->entityStorage->CreateEntity("Rect", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(0, 0, 0);
	transform2D->ratio = ScreenAspectRatio::XY;
	transform2D->position = point3d(0, 0.7f, 0.0f);
	transform2D->scale = point3d(0.5f, 0.025f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.75f, 0.0f, 0.0f);
}

void LevelManagerClass::InitSystems()
{
	m_World->AddComputeSystem<TimeSystem>();
	m_World->AddComputeSystem<EntityManagerSystem>();

	m_World->AddPhysicSystem<PhysicSystem>();
	m_World->AddPhysicSystem<CollisionSystem>();
	m_World->AddPhysicSystem<CombatSystem>();
	//AISystem* aiSystem = m_World->AddPhysicSystem<AISystem>();
	m_World->AddPhysicSystem<SkeletalAnimationSystem>(context, m_BoneBuffer);

	m_World->AddRenderSystem<MeshSystem>();
	m_World->AddRenderSystem<SkinnedMeshSystem>(m_World->m_Camera->frustum, m_World->m_Camera, m_BoneBuffer);
	if (SHOW_COLLIDERS) {
		m_World->AddRenderSystem<CollisionDrawSystem>();
	}
	m_World->AddRenderSystem<SpriteSystem>(m_World->m_Camera->frustum, m_BoneBuffer);
	m_World->AddRenderSystem<NebulaSystem>();
	m_World->AddRenderSystem<UISystem>();
	m_World->AddRenderSystem<UITextSystem>();
}


void LevelManagerClass::CreateSpaceBackground(Entity* folder, int quality)
{
	int outerSpace_cnt = 6853 / quality;

	Entity* entity = m_World->entityStorage->CreateEntity("OuterSpace", folder);

	Nebula* nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 25;
	nebula->count = outerSpace_cnt;
	nebula->mode = pMode::point;
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

	Entity* aries = m_World->entityStorage->CreateEntity("Aries", folder);
	transform = aries->AddComponent<Transform>();
	transform->scale = point3d(4, 4, 4);
	transform->position = point3d(0.0f, 20.0f, 50.0f);
	pointCloud = aries->AddComponent<PointCloud>();
	pointCloud->index = 2;
	pointCloud->pointSize = 1.0f;
	pointCloud->brightness = 0.4f;
	//pointCloud->color = point3d(1, 0.2, 0.25);
	pointCloud->instances = 1;
	pointCloud->frustumRadius = 8;
	pointCloud->compress = RenderCompress::x2;
	health = aries->AddComponent<Health>();
	health->hp = 1000;
	health->maxHp = 1000;

	constellation = aries->AddComponent<Constellation>();
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
	};

	entity = m_World->entityStorage->CreateEntity("Armor", aries);
	transform = entity->AddComponent<Transform>();
	pointCloud = entity->AddComponent<PointCloud>();
	pointCloud->index = 3;
	pointCloud->pointSize = 0.75f;
	pointCloud->brightness = 0.2f;
	pointCloud->color = point3d(1, 0.9f, 0.2f);
	pointCloud->frustumRadius = 8;
	pointCloud->compress = RenderCompress::x2;

	//Body

	entity = m_World->entityStorage->CreateEntity("Collider", aries);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0, 0.08, 0.1);
	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->softness = 0.7;
	sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	sphereCollider->radius = 3;

	entity = m_World->entityStorage->CreateEntity("Collider", aries);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0, -0.05, -1.5);
	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->softness = 0.7;
	sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	sphereCollider->radius = 2.2;

	entity = m_World->entityStorage->CreateEntity("Collider", aries);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0, -0.1, -0.7);
	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->softness = 0.7;
	sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	sphereCollider->radius = 2.7;

	//Head

	entity = m_World->entityStorage->CreateEntity("Collider", aries);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0, 0.9, 0.8);
	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->softness = 0.7;
	sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	sphereCollider->radius = 2;

	entity = m_World->entityStorage->CreateEntity("Collider", aries);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0, 0.7, 1.4);
	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->softness = 0.7;
	sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	sphereCollider->radius = 1.1;

	//Horns

	entity = m_World->entityStorage->CreateEntity("Collider", aries);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0.5, 1.2, 0.5);
	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->softness = 0.7;
	sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	sphereCollider->radius = 1.6;

	entity = m_World->entityStorage->CreateEntity("Collider", aries);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(-0.5, 1.2, 0.5);
	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->softness = 0.7;
	sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	sphereCollider->radius = 1.6;
}


void LevelManagerClass::CreateZenithLocation(Entity* folder, int quality)
{
	Entity* entity;
	Nebula* nebula;
	Transform* transform;

	int pillars_cnt = 3725470 / 2 / quality;
	int galaxy_cnt = 182361 / quality;

	Entity* location = m_World->entityStorage->CreateEntity("Zenith location", folder);

	transform = location->AddComponent<Transform>();
	//transform->position = point3d(0, 50, 0);

	// Pillars hand | point

	entity = m_World->entityStorage->CreateEntity("PHP", folder);
	transform = entity->AddComponent<Transform>();

	nebula = location->AddComponent<Nebula>();
	nebula->vShader = 24;
	nebula->count = pillars_cnt;
	nebula->mode = pMode::point;
	nebula->scale = 1;
	nebula->frustumRadius = 40;

	// Inside nebula | point

	entity = m_World->entityStorage->CreateEntity("INP", location);
	transform = entity->AddComponent<Transform>();

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 23;
	nebula->count = pillars_cnt;
	nebula->mode = pMode::point;
	nebula->color = point3d(1, 0.3, 0.5);
	nebula->scale = 1;
	nebula->frustumRadius = 40;

	// Pillars hand | glow

	entity = m_World->entityStorage->CreateEntity("PHG", location);
	transform = entity->AddComponent<Transform>();

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 24;
	nebula->count = pillars_cnt;
	nebula->skipper = 1394 / 2;
	nebula->mode = pMode::glow;
	nebula->scale = 1;
	nebula->frustumRadius = 40;

	// Inside nebula | glow

	entity = m_World->entityStorage->CreateEntity("ING", location);
	transform = entity->AddComponent<Transform>();

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 23;
	nebula->count = pillars_cnt;
	nebula->skipper = 1394;
	nebula->mode = pMode::glow;
	nebula->color = point3d(1, 0.3, 0.5);
	nebula->scale = 1;
	nebula->frustumRadius = 40;
}


void LevelManagerClass::CreateNebulas(Entity* folder, int quality)
{
	Entity* entity;
	Nebula* nebula;
	Transform* transform;
	SphereCollider* sphereCollider;

	int pillars_cnt = 3725470 / 2 / quality;

	Entity* location = m_World->entityStorage->CreateEntity("Zenith location", folder);

	transform = location->AddComponent<Transform>();
	transform->position = point3d(0, 50, 0);

	// 1 Nebula

	entity = m_World->entityStorage->CreateEntity("INP", location);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(150, 0, 0);

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 26;
	nebula->count = pillars_cnt;
	nebula->mode = pMode::point;
	nebula->color = point3d(1, 0.3, 0.5);  // Розоватый
	nebula->color = point3d(0.2, 0.8, 0.3); // Зеленый
	nebula->color = point3d(0.3, 0.5, 1.0); // Голубой
	nebula->scale = 1;
	nebula->frustumRadius = 40;
	nebula->isInteractive = true;

	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->radius = 40;
	sphereCollider->isTouchable = false;

	entity = m_World->entityStorage->CreateEntity("ING", location);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(150, 0, 0);

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 26;
	nebula->count = pillars_cnt;
	nebula->skipper = 1394;
	nebula->mode = pMode::glow;
	nebula->color = point3d(1, 0.3, 0.5);  // Розоватый
	nebula->color = point3d(0.2, 0.8, 0.3); // Зеленый
	nebula->color = point3d(0.3, 0.5, 1.0); // Голубой
	nebula->scale = 1;
	nebula->frustumRadius = 40;

	// 2 Nebula

	entity = m_World->entityStorage->CreateEntity("INP1", location);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(-150, 0, 0);

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


	entity = m_World->entityStorage->CreateEntity("ING1", location);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(-150, 0, 0);

	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 26;
	nebula->count = pillars_cnt;
	nebula->skipper = 1394;
	nebula->mode = pMode::glow;
	nebula->color = point3d(0.8, 0.4, 0.2);
	nebula->scale = 1;
	nebula->frustumRadius = 40;

}

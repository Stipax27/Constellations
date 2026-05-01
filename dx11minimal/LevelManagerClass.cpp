#include "LevelManagerClass.h"

using namespace std;



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
	Transform* transform;

	SphereCollider* sphereCollider;

	PlaneCollider* planeCollider;
	ParticleEmitter* particleEmitter;

	Mesh* mesh;
	PointCloud* pointCloud;
	Star* star;


	worldFolder = m_World->entityStorage->CreateEntity("World");

	Entity* player = CreatePlayer();

	entity = m_World->entityStorage->CreateEntity("TestStar", worldFolder);
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

	/////////////////////////
	
	CreateSpaceBackground(worldFolder, 1);
	CreateNebula(worldFolder,2);

	CreateRoom();

	/////////////////////////

	CreateUI();

	//////////////////////////////////////////////////////////////////////////////////////////////
	// WORLD CREATING END //
	//////////////////////////////////////////////////////////////////////////////////////////////

	InitSystems();

	playerController = new PlayerController();
	playerController->Initialize(player, m_World->entityStorage);

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

	interp::UpdateTweens();

	playerController->ProcessInput();
	playerController->ProcessMouse();
	playerController->abilities->Update();
	playerController->ProccessUI();

	ConstBuf::frame.aspect = XMFLOAT4{ float(window->aspect), float(window->iaspect), float(window->width), float(window->height) };

	m_World->UpdateCompute();
	m_World->UpdatePhysic();

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

	if (Models::LoadSkinnedModel("..\\dx11minimal\\Resourses\\Models\\TestAnims\\1.glb", m_TestAnimMesh, m_TestAnimSkeleton, m_TestAnimAnimations))
	{
		Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\2.glb", m_TestAnimSkeleton, m_TestAnimAnimations, false);
		Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\3.glb", m_TestAnimSkeleton, m_TestAnimAnimations, true);
		Models::LoadAndRemapAnimations("..\\dx11minimal\\Resourses\\Models\\TestAnims\\4.glb", m_TestAnimSkeleton, m_TestAnimAnimations, true);
	}

	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\SnakeModel.obj");
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

	StarClay* starClay = player->AddComponent<StarClay>();
	starClay->blobsRadius = { 0.2f, 0.4f };
	starClay->rate = 20;

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

	/*entity = m_World->entityStorage->CreateEntity("BossHealth", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(1, 0, 0);
	transform2D->ratio = ScreenAspectRatio::YY;
	transform2D->position = point3d(0.75f, 0.0f, 0.0f);
	transform2D->scale = point3d(0.2f, 0.2f, 0.0f);
	ImageLabel* imageLabel = entity->AddComponent<ImageLabel>();
	imageLabel->textureName = "comicsSpot";
	imageLabel->color = point3d(0.12, 0.91, 0.62);*/
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
	pointCloud = aries->AddComponent<PointCloud>();
	pointCloud->index = 2;
	pointCloud->pointSize = 1.0f;
	pointCloud->brightness = 0.4f;
	//pointCloud->color = point3d(1, 0.2, 0.25);
	pointCloud->instances = 1;
	pointCloud->frustumRadius = 8;
	//pointCloud->compress = RenderCompress::x2;
	health = aries->AddComponent<Health>();
	health->hp = 1000;
	health->maxHp = 1000;
	cameraTarget = aries->AddComponent<CameraTarget>();

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
	//pointCloud->compress = RenderCompress::x2;

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
	SphereCollider* sphereCollider;
	MultiDamager* multiDamager;
	

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
	transform->position = point3d(150, 0, 0);

	
	nebula = entity->AddComponent<Nebula>();
	nebula->vShader = 23;
	nebula->count = pillars_cnt;
	nebula->mode = pMode::point;
	nebula->color = point3d(0.3, 0.5, 1.0); // Голубой
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
	nebula->scale = 1;
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
	nebula->color = point3d(0.3, 0.5, 1.0); // Голубой
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
	nebula->color = point3d(1, 0.3, 0.5);  // Розоватый
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
	nebula->color = point3d(1, 0.3, 0.5);  // Розоватый
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

void LevelManagerClass::CreateRoom()
{
	Entity* room = m_World->entityStorage->CreateEntity("Room");
	Transform* transform = room->AddComponent<Transform>();

	Entity* wall = m_World->entityStorage->CreateEntity("Wall", room);
	transform = wall->AddComponent<Transform>();
	transform->position = point3d(0, -10, 0);
	PlaneCollider* planeCollider = wall->AddComponent<PlaneCollider>();
	planeCollider->normal = point3d(0, 1, 0);
	wall->AddComponent<Star>();

	wall = m_World->entityStorage->CreateEntity("Wall", room);
	transform = wall->AddComponent<Transform>();
	transform->position = point3d(10, 0, 0);
	planeCollider = wall->AddComponent<PlaneCollider>();
	planeCollider->normal = point3d(-1, 0, 0);
	wall->AddComponent<Star>();

	wall = m_World->entityStorage->CreateEntity("Wall", room);
	transform = wall->AddComponent<Transform>();
	transform->position = point3d(-10, 0, 0);
	planeCollider = wall->AddComponent<PlaneCollider>();
	planeCollider->normal = point3d(1, 0, 0);
	wall->AddComponent<Star>();

	wall = m_World->entityStorage->CreateEntity("Wall", room);
	transform = wall->AddComponent<Transform>();
	transform->position = point3d(0, 0, 10);
	planeCollider = wall->AddComponent<PlaneCollider>();
	planeCollider->normal = point3d(0, 0, -1);
	wall->AddComponent<Star>();

	wall = m_World->entityStorage->CreateEntity("Wall", room);
	transform = wall->AddComponent<Transform>();
	transform->position = point3d(0, 0, -10);
	planeCollider = wall->AddComponent<PlaneCollider>();
	planeCollider->normal = point3d(0, 0, 1);
	wall->AddComponent<Star>();
}
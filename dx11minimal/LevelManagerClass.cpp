#include "LevelManagerClass.h"

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
		window = new WindowClass;
	}
}


bool LevelManagerClass::Initialize()
{
	InitWindow();

	m_World = new World;
	m_World->Initialize(window->iaspect);

	collisionManager = new CollisionManagerClass;
	collisionManager->Initialize(m_World->entityStorage);

	mouse = new MouseClass;
	mouse->Initialize(window, m_World->m_Camera);

	Dx11Init(window->hWnd, window->width, window->height);

	// window params into const buffer
	ConstBuf::frame.aspect = XMFLOAT4(window->aspect, window->iaspect, float(window->width), float(window->height));

	ConstBuf::factors.AriesNebulaLerpFactor = 0;
	ConstBuf::UpdateFactors();

	//Textures::LoadTexture("..\\dx11minimal\\Resourses\\Textures\\testTexture.tga");

	std::thread thr(&LevelManagerClass::LoadModels, this);
	thr.detach();

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

	entity = m_World->entityStorage->CreateEntity("AriesNebulaLocation", folder);
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
	spriteCluster->compress = RenderCompress::x2;
	/*sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->radius = 25.0f;
	sphereCollider->softness = 0.5f;*/
	//entity->AddComponent<SurfaceCollider>();

	entity = m_World->entityStorage->CreateEntity("StarsBackground", folder);
	spriteCluster = entity->AddComponent<SpriteCluster>();
	spriteCluster->vShader = 2;
	spriteCluster->pShader = 2;
	spriteCluster->pointsNum = 10000;

	/////////////////////////

	entity = m_World->entityStorage->CreateEntity("Star", folder);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0.0f, 0.0f, -20.0f);
	star = entity->AddComponent<Star>();
	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->radius = 0.5f;
	sphereCollider->active = false;
	sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
	//sphereCollider->softness = 0.5f;
	singleDamager = entity->AddComponent<SingleDamager>();
	singleDamager->damage = 1000;

	entity = m_World->entityStorage->CreateEntity("Particles", folder);
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
	particleEmitter->spread = { PI / 8, PI / 8 };

	entity = m_World->entityStorage->CreateEntity("Particles", folder);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(-20.0f, 0.0f, -20.0f);
	particleEmitter = entity->AddComponent<ParticleEmitter>();
	particleEmitter->rate = 150;
	particleEmitter->lifetime = 1000;
	particleEmitter->color = point3d(1.0f, 0.15f, 0.1f);
	particleEmitter->size = { 0.0f, 4.0f };
	particleEmitter->opacity = { 1.0f, 0.0f };
	particleEmitter->emitDirection = EmitDirection::Up;
	particleEmitter->speed = { 10.0f, 0.0f };
	particleEmitter->spread = { PI, PI };
	particleEmitter->isReverse = true;

	entity = m_World->entityStorage->CreateEntity("Particles", folder);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(-30.0f, 0.0f, -20.0f);
	particleEmitter = entity->AddComponent<ParticleEmitter>();
	particleEmitter->rate = 100;
	particleEmitter->lifetime = 750;
	particleEmitter->color = point3d(1, 1, 1);
	particleEmitter->size = { 0.0f, 6.0f };
	particleEmitter->opacity = { 0.75f, 0.0f };
	particleEmitter->emitDirection = EmitDirection::Front;
	particleEmitter->speed = { 15.0f, 0.0f };
	particleEmitter->spread = { 0, PI };
	particleEmitter->isHeapEmit = true;

	entity = m_World->entityStorage->CreateEntity("Particles", folder);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(-40.0f, 0.0f, -20.0f);
	particleEmitter = entity->AddComponent<ParticleEmitter>();
	particleEmitter->rate = 100;
	particleEmitter->lifetime = 1000;
	particleEmitter->color = point3d(0.95f, 0.65f, 0.25f);
	particleEmitter->size = { 3.0f, 0.0f };
	particleEmitter->opacity = { 1.0f, 0.0f };
	particleEmitter->emitDirection = EmitDirection::Up;
	particleEmitter->speed = { 10.0f, 0.0f };

	for (int i = 0; i < 5; i++) {
		entity = m_World->entityStorage->CreateEntity("Star", folder);
		transform = entity->AddComponent<Transform>();
		transform->position = point3d(10.0f + 5.0f * i, 0.0f, -35.0f);
		star = entity->AddComponent<Star>();
		star->radius = 2.0f;
		star->crownRadius = 3.5f;
		star->color1 = point3d(0.87f, 0.79f, 1.0f);
		star->color2 = point3d(0.7f, 0.0f, 0.47f);
		star->crownColor = point3d(0.47f, 0.65f, 1.0f);
		sphereCollider = entity->AddComponent<SphereCollider>();
		sphereCollider->radius = 2.0f;
		sphereCollider->collisionGroup = CollisionFilter::Group::Enemy;
		health = entity->AddComponent<Health>();
		health->hp = 10;
		health->maxHp = 10;
	}

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

	CreateAries(folder);

	/////////////////////////

	Entity* holder = m_World->entityStorage->CreateEntity("Holder", folder);
	transform = holder->AddComponent<Transform>();
	transform->scale = point3d(10, 10, 10);
	transform->position = point3d(0.0f, 0.0f, -50.0f);
	mesh = holder->AddComponent<Mesh>();
	mesh->index = 1;

	CreateUI();

	//////////////////////////////////////////////////////////////////////////////////////////////
	// WORLD CREATING END //
	//////////////////////////////////////////////////////////////////////////////////////////////

	InitSystems();

	//m_World->PreCalculations();

	playerController = new PlayerController();
	playerController->Initialize(player, m_World, mouse, window, collisionManager);

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
}

void LevelManagerClass::Frame()
{
	mouse->Update();
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


void LevelManagerClass::LoadModels()
{
	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\HeroFists.obj");
	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\Arrow.obj");

	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\AriesBody.obj");
	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\AriesArmor.obj");
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


void LevelManagerClass::CreateZenithLocation(int quality)
{
	int pillars_cnt = 3725470 / 2 / quality;
	int outerSpace_cnt = 6853 / quality;
	int galaxy_cnt = 182361 / quality;

	//hi
	//RenderTarget::Set({ texture::pBuf,0 });
	Draw::Clear({ 0,0,0,0 });

	PillarsHand(pillars_cnt, 1, pMode::point);
	InsideNebula(pillars_cnt, 1, pMode::point, 100, 200, 600);
	OuterSpace(outerSpace_cnt, 1, pMode::point);

	//mid
	//RenderTarget::Set({ texture::pBufMid,0 });
	Draw::Clear({ 0,0,0,0 });

	//low
	//RenderTarget::Set({ texture::pBufLow,0 });
	Draw::Clear({ 0,0,0,0 });

	PillarsHand(pillars_cnt, 1394 / 2, pMode::glow);
	InsideNebula(pillars_cnt, 1394, pMode::glow ,100, 200, 600);
	//	OuterSpace(outerSpace_cnt, 64, pMode::glow);
}


void LevelManagerClass::PillarsHand(int count, int skipper, pMode mode)
{
	int gX = sqrt(count / skipper);
	int gY = sqrt(count / skipper);

	//psModeSet(mode);

	ConstBuf::locationInfo.model = XMMatrixTranspose(XMMatrixTranslation(0, 0, 0));
	ConstBuf::locationInfo.gX = gX;
	ConstBuf::locationInfo.gY = gY;
	ConstBuf::locationInfo.mode = (int)mode;
	ConstBuf::locationInfo.skipper = skipper;

	Shaders::vShader(14);
	ConstBuf::UpdateLocationInfo();
	ConstBuf::ConstToVertex(11);

	Draw::NullDrawer(1, (int)gX * (int)gY);
}


void LevelManagerClass::InsideNebula(int count, int skipper, pMode mode, int r, int g, int b)
{
	int gX = sqrt(count / skipper);
	int gY = sqrt(count / skipper);

	//psModeSet(mode);

	ConstBuf::locationInfo.model = XMMatrixTranspose(XMMatrixTranslation(0, 0, 0));
	ConstBuf::locationInfo.gX = gX;
	ConstBuf::locationInfo.gY = gY;
	ConstBuf::locationInfo.mode = (int)mode;
	ConstBuf::locationInfo.skipper = skipper;
	ConstBuf::locationInfo.base_color = XMFLOAT4(r / 100., g / 100., b / 100., 1);

	Shaders::vShader(5);
	ConstBuf::UpdateLocationInfo();
	ConstBuf::ConstToVertex(11);

	Draw::NullDrawer(1, (int)gX * (int)gY);
}


void LevelManagerClass::OuterSpace(int count, int skipper, pMode mode)
{
	int gX = sqrt(count / skipper);
	int gY = sqrt(count / skipper);

	//psModeSet(mode);

	ConstBuf::locationInfo.model = XMMatrixTranspose(XMMatrixTranslation(0, 0, 0));
	ConstBuf::locationInfo.gX = gX;
	ConstBuf::locationInfo.gY = gY;
	ConstBuf::locationInfo.mode = (int)mode;
	ConstBuf::locationInfo.skipper = skipper;

	Shaders::vShader(19);
	ConstBuf::UpdateLocationInfo();
	ConstBuf::ConstToVertex(11);

	Draw::NullDrawer(1, (int)gX * (int)gY);
}


void LevelManagerClass::InitSystems()
{
	m_World->AddComputeSystem<TimeSystem>();
	m_World->AddComputeSystem<EntityManagerSystem>();

	m_World->AddPhysicSystem<PhysicSystem>();
	m_World->AddPhysicSystem<CollisionSystem>(collisionManager);
	m_World->AddPhysicSystem<CombatSystem>();
	//AISystem* aiSystem = m_World->AddPhysicSystem<AISystem>();

	m_World->AddRenderSystem<MeshSystem>(m_World->m_Camera->frustum, m_World->m_Camera);
	if (SHOW_COLLIDERS) {
		m_World->AddRenderSystem<CollisionDrawSystem>();
	}
	m_World->AddRenderSystem<SpriteSystem>(m_World->m_Camera->frustum);
	m_World->AddRenderSystem<UISystem>(mouse, m_World->entityStorage);
}


void LevelManagerClass::psModeSet(pMode mode)
{
	switch (mode)
	{
	case pMode::point:
	{
		Shaders::pShader(0);
		break;
	}
	case pMode::glow:
	{
		Shaders::pShader(1);
		break;
	}
	}
}
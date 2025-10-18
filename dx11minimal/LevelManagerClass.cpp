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
	char bitmapFilename[128];
	char modelFilename[128];
	char textureFilename[128];
	bool result;

	InitWindow();

	m_World = new World;
	result = m_World->Initialize(window->iaspect);
	if (!result)
	{
		return false;
	}

	mouse = new MouseClass;
	mouse->Initialize(window, m_World->m_Camera);

	Dx11Init(window->hWnd, window->width, window->height);

	// window params into const buffer
	ConstBuf::frame.aspect = XMFLOAT4(window->aspect, window->iaspect, float(window->width), float(window->height));

	ConstBuf::factors.AriesNebulaLerpFactor = 0;
	ConstBuf::UpdateFactors();

	//////////////////////////////////////////////////////////////////////////////////////////////
	// WORLD CREATING START //
	//////////////////////////////////////////////////////////////////////////////////////////////

	Entity* player;
	Entity* entity;
	Constellation* constellation;
	Transform* transform;
	PhysicBody* physicBody;
	//SphereCollider* sphereCollider;
	SpriteCluster* spriteCluster;

	player = m_World->CreateEntity();
	transform = player->AddComponent<Transform>();
	transform->position = point3d(0.0f, 0.0f, 0.0f);
	transform->scale = point3d(1, 0, 0);
	physicBody = player->AddComponent<PhysicBody>();
	//star->AddComponent<SphereCollider>();
	constellation = player->AddComponent<Constellation>();
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

	entity = m_World->CreateEntity();
	transform = entity->AddComponent<Transform>();
	spriteCluster = entity->AddComponent<SpriteCluster>();
	spriteCluster->vShader = 7;
	spriteCluster->pShader = 7;
	spriteCluster->pointsNum = 900000;

	entity = m_World->CreateEntity();
	spriteCluster = entity->AddComponent<SpriteCluster>();
	spriteCluster->vShader = 2;
	spriteCluster->pShader = 2;
	spriteCluster->pointsNum = 10000;

	entity = m_World->CreateEntity();
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(2.0f, 0.0f, 0.0f);
	transform->scale = point3d(1, 0, 0);
	//star->AddComponent<SphereCollider>();
	constellation = entity->AddComponent<Constellation>();
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

	m_World->AddPhysicSystem<PhysicSystem>();
	m_World->AddPhysicSystem<CollisionSystem>();
	m_World->AddRenderSystem<RenderSystem>(m_World->m_Camera);

	//////////////////////////////////////////////////////////////////////////////////////////////
	// WORLD CREATING END //
	//////////////////////////////////////////////////////////////////////////////////////////////

	m_World->PreCalculations();

	playerController = new PlayerController();
	playerController->Initialize(player, m_World->m_Camera, mouse, window);

	return true;
}


void LevelManagerClass::Shutdown()
{
	if (m_World)
	{
		m_World->Shutdown();
		delete m_World;
		m_World = 0;
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

	if (playerController)
	{
		playerController->Shutdown();
		delete playerController;
		playerController = 0;
	}
}


bool LevelManagerClass::Frame()
{
	bool result;

	mouse->Update();
	playerController->ProcessInput();
	playerController->ProcessMouse();

	ConstBuf::frame.aspect = XMFLOAT4{ float(window->aspect), float(window->iaspect), float(window->width), float(window->height) };

	result = m_World->UpdatePhysic();
	if (!result)
	{
		return false;
	}

	playerController->ProcessCamera();

	result = m_World->UpdateRender();
	if (!result)
	{
		return false;
	}

	return true;
}
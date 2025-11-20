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

	Textures::LoadTexture("..\\dx11minimal\\Resourses\\Textures\\testTexture.tga");
	Models::LoadModelFromTxtFile("..\\dx11minimal\\Resourses\\Models\\Cube.txt");

	//////////////////////////////////////////////////////////////////////////////////////////////
	// WORLD CREATING START //
	//////////////////////////////////////////////////////////////////////////////////////////////

	Entity* player;
	Entity* entity;
	Explosion* explosion;
	Constellation* constellation;
	Transform* transform;
	PhysicBody* physicBody;

	AIComponent* aiComponent;
	//SphereCollider* sphereCollider;

	SphereCollider* sphereCollider;

	SpriteCluster* spriteCluster;
	PlaneCollider* planeCollider;
	Rect* rect;
	TextLabel* textLabel;
	Mesh* mesh;

	Entity* folder = m_World->CreateEntity("WorldFolder");
	folder->SetActive(true);

	player = m_World->CreateEntity("Player", folder);
	transform = player->AddComponent<Transform>();
	transform->position = point3d(0.0f, 0.0f, -20.0f);
	transform->scale = point3d(1, 0, 0);
	physicBody = player->AddComponent<PhysicBody>();
	sphereCollider = player->AddComponent<SphereCollider>();
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


	/*entity = m_World->CreateEntity();
	explosion = entity->AddComponent<Explosion>();
	transform = entity->AddComponent<Transform>();*/

	entity = m_World->CreateEntity();

	entity = m_World->CreateEntity("AriesNebulaLocation", folder);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0.0f, 0.0f, 50.0f);
	transform->scale = point3d(1, 0, 0);
	spriteCluster = entity->AddComponent<SpriteCluster>();
	spriteCluster->vShader = 7;
	spriteCluster->pShader = 7;
	spriteCluster->pointsNum = 900000;
	entity->AddComponent<SurfaceCollider>();
	//planeCollider = entity->AddComponent<PlaneCollider>();
	//planeCollider->gravityDistance = 20.0f;

	entity = m_World->CreateEntity("StarsBackground", folder);
	spriteCluster = entity->AddComponent<SpriteCluster>();
	spriteCluster->vShader = 2;
	spriteCluster->pShader = 2;
	spriteCluster->pointsNum = 10000;

	entity = m_World->CreateEntity("Cube", folder);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(20.0f, 10.0f, 0.0f);
	transform->scale = point3d(5.0f, 0.2f, 1.0f);
	mesh = entity->AddComponent<Mesh>();

	entity = m_World->CreateEntity("Enemy", folder);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(2.0f, 0.0f, 0.0f);
	transform->scale = point3d(1, 0, 0);
	physicBody = entity->AddComponent<PhysicBody>();
	aiComponent = entity->AddComponent<AIComponent>();

	aiComponent->patrolPoints = {
		 point3d(-50.0f, 0.0f, 0.0f),
		 point3d(0.0f, 0.0f, 25.0f),
		 point3d(25.0f, 0.0f, 0.0f),
		 point3d(0.0f, 0.0f, -50.0f)
	};
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
	
	entity = m_World->CreateEntity();
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(30.0f, 3.0f, 0.0f);
	transform->scale = point3d(1, 0, 0);
	physicBody = entity->AddComponent<PhysicBody>();
	//star->AddComponent<SphereCollider>();
	constellation = entity->AddComponent<Constellation>();
	constellation->stars = {
		point3d(0, 0, 0)
	};
	aiComponent = entity->AddComponent<AIComponent>();

	aiComponent->patrolPoints = {
		 point3d(-10.0f, 0.0f, 0.0f),
		 point3d(0.0f, 0.0f, 10.0f)
	};


	// MAIN MENU //
	
	/*entity = m_World->CreateEntity();
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0.0f, 0.1f, 0.0f);
	transform->scale = point3d(0.25f, 0.05f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.5f, 0.25f, 0.8f);
	rect->opacity = 1.f;
	rect->anchorPoint = point3d(0, 0, 0);
	rect->ratio = ScreenAspectRatio::YY;
	rect->cornerRadius = 0.25f;
	rect->cornerType = CornerType::Strict;

	entity = m_World->CreateEntity();
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0.0f, -0.1f, 0.0f);
	transform->scale = point3d(0.25f, 0.05f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.5f, 0.25f, 0.8f);
	rect->opacity = 1.f;
	rect->anchorPoint = point3d(0, 0, 0);
	rect->ratio = ScreenAspectRatio::YY;
	rect->cornerRadius = 0.25f;
	rect->cornerType = CornerType::Strict;

	entity = m_World->CreateEntity();
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0.0f, -0.3f, 0.0f);
	transform->scale = point3d(0.25f, 0.05f, 0.0f);
	rect = entity->AddComponent<Rect>();
	rect->color = point3d(0.5f, 0.25f, 0.8f);
	rect->opacity = 1.f;
	rect->anchorPoint = point3d(0, 0, 0);
	rect->ratio = ScreenAspectRatio::YY;
	rect->cornerRadius = 0.25f;
	rect->cornerType = CornerType::Strict;*/

	// MAIN MENU END //

	/*entity = m_World->CreateEntity();
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0, 0, 0);
	transform->scale = point3d(0.5, 0.5, 0);
	textLabel = entity->AddComponent<TextLabel>();
	textLabel->text = "Test text";*/

	//////////////////////////////////////////////////////////////////////////////////////////////
	// WORLD CREATING END //
	//////////////////////////////////////////////////////////////////////////////////////////////

	
	m_World->AddPhysicSystem<PhysicSystem>();
	m_World->AddPhysicSystem<CollisionSystem>();
	m_World->AddRenderSystem<MeshSystem>();
	m_World->AddRenderSystem<SpriteSystem>();
	m_World->AddRenderSystem<UISystem>();

	//m_World->PreCalculations();
	Star = new BaseStar();
	Star->Init(m_World, entity);
	

	playerController = new PlayerController();
	playerController->Initialize(player, m_World->m_Camera, mouse, window);

	AISystem* aiSystem = m_World->AddPhysicSystem<AISystem>();

	if (aiSystem)
	{
		aiSystem->SetPlayerEntity(player);
	}


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


// � ������ Frame ��� � ������ ����� �� ��������
bool LevelManagerClass::Frame()
{
	bool result;

	mouse->Update();
	playerController->ProcessInput();
	playerController->ProcessMouse();

	
	// ��������� ������� ��� ���� ������
	DWORD currentTime = timer::currentTime;
	srand(time(0));
	// ������: ����� ������ 3 �������
	if (currentTime - Star->LastTime > 3000) {
		Star->FartingEffect();
		// ��������� ����� �����
		int attackType = rand() % 3;
		switch (attackType) {
		case 0:
			Star->Flash();
			break;
		case 1:
			Star->CoronalEjection();
			break;
		case 2:
			Star->SunWind();
			break;
		}
		//Star->LifeTimeParticl();
		Star->LastTime = currentTime;
	}

	ConstBuf::frame.aspect = XMFLOAT4{ float(window->aspect), float(window->iaspect), float(window->width), float(window->height) };

	result = m_World->UpdatePhysic();
	if (!result) {
		return false;
	}

	playerController->ProcessCamera();

	result = m_World->UpdateRender();
	if (!result) {
		return false;
	}

	mouse->RenderCursor();
	Draw::Present();

	return true;
}
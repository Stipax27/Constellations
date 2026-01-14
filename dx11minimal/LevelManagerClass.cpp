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

	collisionManager = new CollisionManagerClass;
	collisionManager->Initialize();

	mouse = new MouseClass;
	mouse->Initialize(window, m_World->m_Camera);

	Dx11Init(window->hWnd, window->width, window->height);

	// window params into const buffer
	ConstBuf::frame.aspect = XMFLOAT4(window->aspect, window->iaspect, float(window->width), float(window->height));

	ConstBuf::factors.AriesNebulaLerpFactor = 0;
	ConstBuf::UpdateFactors();

	Textures::LoadTexture("..\\dx11minimal\\Resourses\\Textures\\testTexture.tga");

	Models::LoadTxtModel("..\\dx11minimal\\Resourses\\Models\\Cube.txt");
	Models::LoadTxtModel("..\\dx11minimal\\Resourses\\Models\\Cube2.txt");
	Models::LoadTxtModel("..\\dx11minimal\\Resourses\\Models\\Cat.txt", true);
	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\Cube.obj");
	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\cat2.obj");
	//Models::LoadModelFromGltfFile("..\\dx11minimal\\Resourses\\Models\\Cube.gltf");

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
	PointCloud* pointCloud;
	DelayedDestroy* delayedDestroy;
	Star* star;
	Health* health;
	SingleDamageGiver* singleDamageGiver;

	Entity* folder = m_World->CreateEntity("WorldFolder");
	folder->SetActive(true);

	player = m_World->CreateEntity("Player", folder);
	transform = player->AddComponent<Transform>();
	transform->position = point3d(0.0f, 0.0f, 0.0f);
	physicBody = player->AddComponent<PhysicBody>();
	sphereCollider = player->AddComponent<SphereCollider>();
	sphereCollider->radius = 0.75f;
	health = player->AddComponent<Health>();
	/*constellation = player->AddComponent<Constellation>();
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
	pointCloud = player->AddComponent<PointCloud>();
	pointCloud->index = 4;
	pointCloud->position = point3d(0, -0.8, 0);
	pointCloud->scale = point3d(7, 7, 7);
	pointCloud->mRotation = XMMatrixRotationAxis(XMVectorSet(0, 1, 0, 0), -PI / 2);
	pointCloud->brightness = 0.25f;
	pointCloud->color = point3d(1, 0.6f, 0.9f);


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
	spriteCluster->gShader = 7;
	spriteCluster->pShader = 7;
	spriteCluster->pointsNum = 900000;
	spriteCluster->vertexNum = 1;
	spriteCluster->frustumRadius = 60;
	spriteCluster->topology = InputAssembler::topology::pointList;
	spriteCluster->compress = RenderCompress::x2;
	sphereCollider = entity->AddComponent<SphereCollider>(); 
	sphereCollider->radius = 25.0f;
	sphereCollider->softness = 0.5f;
	//entity->AddComponent<SurfaceCollider>();
	//planeCollider = entity->AddComponent<PlaneCollider>();
	//planeCollider->gravityDistance = 20.0f;
	/*delayedDestroy = entity->AddComponent<DelayedDestroy>();
	delayedDestroy->lifeTime = 5000;*/

	entity = m_World->CreateEntity("StarsBackground", folder);
	spriteCluster = entity->AddComponent<SpriteCluster>();
	spriteCluster->vShader = 2;
	spriteCluster->pShader = 2;
	spriteCluster->pointsNum = 10000;

	/////////////////////////

	entity = m_World->CreateEntity("Star", folder);
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(0.0f, 0.0f, -20.0f);
	star = entity->AddComponent<Star>();
	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->radius = 0.5f;
	//sphereCollider->softness = 0.5f;
	singleDamageGiver = entity->AddComponent<SingleDamageGiver>();
	singleDamageGiver->damage = 1000;

	/////////////////////////

	Entity* holder = m_World->CreateEntity("Holder", folder);
	transform = holder->AddComponent<Transform>();
	transform->scale = point3d(10, 10, 10);
	transform->position = point3d(0.0f, 0.0f, -50.0f);
	transform->mRotation = XMMatrixRotationAxis(XMVectorSet(0, 1, 0, 0), -180 * RAD) * transform->mRotation;
	mesh = holder->AddComponent<Mesh>();
	mesh->index = 4;

	holder = m_World->CreateEntity("Holder", folder);
	transform = holder->AddComponent<Transform>();
	transform->scale = point3d(1, 6, 1);
	transform->position = point3d(0.0f, 10.0f, -50.0f);
	transform->mRotation = XMMatrixRotationAxis(XMVectorSet(0, 1, 0, 0), -180 * RAD) * transform->mRotation;
	mesh = holder->AddComponent<Mesh>();
	mesh->index = 1;

	entity = m_World->CreateEntity("Cat", folder);
	transform = entity->AddComponent<Transform>();
	transform->scale = point3d(30, 30, 30);
	transform->position = point3d(0.0f, 15.0f, 50.0f);
	pointCloud = entity->AddComponent<PointCloud>();
	pointCloud->index = 4;
	pointCloud->pointSize = 2.0f;
	pointCloud->brightness = 0.25f;
	pointCloud->color = point3d(1, 0.1f, 0.2f);

	//entity = m_World->CreateEntity("Cube", holder);
	//transform = entity->AddComponent<Transform>();
	//transform->scale = point3d(2.0f, 10.0f, 2.0f);
	//mesh = entity->AddComponent<Mesh>();

	//entity = m_World->CreateEntity("Cube", holder);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(0.0f, 12.0f, 0.0f);
	//transform->scale = point3d(2.25f, 2.75f, 2.25f);
	//mesh = entity->AddComponent<Mesh>();

	//entity = m_World->CreateEntity("Cube", holder);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(-3.25f, -12.0f, 0.0f);
	//transform->scale = point3d(3.0f, 3.0f, 3.0f);
	//mesh = entity->AddComponent<Mesh>();

	//entity = m_World->CreateEntity("Cube", holder);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(3.25f, -12.0f, 0.0f);
	//transform->scale = point3d(2.75f, 2.75f, 2.75f);
	//mesh = entity->AddComponent<Mesh>();

	/////////////////////////

	//entity = m_World->CreateEntity("Enemy", folder);
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(2.0f, 0.0f, 0.0f);
	//transform->scale = point3d(1, 0, 0);
	//physicBody = entity->AddComponent<PhysicBody>();
	//aiComponent = entity->AddComponent<AIComponent>();
	//
	//aiComponent->patrolPoints = {
	//	 point3d(-50.0f, 0.0f, 0.0f),
	//	 point3d(0.0f, 0.0f, 25.0f),
	//	 point3d(25.0f, 0.0f, 0.0f),
	//	 point3d(0.0f, 0.0f, -50.0f)
	//};
	////star->AddComponent<SphereCollider>();
	//constellation = entity->AddComponent<Constellation>();
	//constellation->stars = {
	//	point3d(-0.09, -0.7, 0),
	//	point3d(-0.05, -0.15, 0),
	//	point3d(0, 0, 0),
	//	point3d(-0.4, 0.5, 0),
	//	point3d(0, 0, 0),
	//	point3d(0.4, 0.3, 0)
	//};
	//constellation->links = {
	//	{0,1},
	//	{1,2},
	//	{2,3},
	//	{2,5}
	//};

	entity = m_World->CreateEntity();
	transform = entity->AddComponent<Transform>();
	transform->position = point3d(2.0f, 0.0f, 0.0f);
	sphereCollider = entity->AddComponent<SphereCollider>();
	sphereCollider->radius = 0.5f;
	physicBody = entity->AddComponent<PhysicBody>();
	constellation = entity->AddComponent<Constellation>();
	constellation->stars = {
		point3d(0, 0, 0),
		point3d(0, 1, 0),
		point3d(1, 1, 0),
		point3d(1, 0, 0),

		point3d(2, 0, 0),
		point3d(2, 1, 0),
		point3d(3, 1, 0),
		point3d(3, 0, 0),

		point3d(0, -1, 0),
		point3d(1, -2, 0),
		point3d(2, -2, 0),
		point3d(3, -1, 0),
	};
	constellation->links = {
		{0,1},
		{1,2},
		{2,3},
		{3,0},

		{4,5},
		{5,6},
		{6,7},
		{7,4},

		{8,9},
		{9,10},
		{10,11}
	};

	aiComponent = entity->AddComponent<AIComponent>();

	vector<point3d> patrolPoints = {
		 point3d(25.0f, 0.0f, 0.0f),
		 point3d(25.0f, 25.0f, 0.0f),
		 point3d(0.0f, 25.0f, 0.0f),
		 point3d(0.0f, 0.0f, 0.0f)
	};

	patrolPoints = smoothCornersPath(patrolPoints, 3);

	aiComponent->patrolPoints = patrolPoints;

	aiComponent->movementSpeed = 10.0;

	//entity = m_World->CreateEntity();
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(30.0f, 3.0f, 0.0f);
	//transform->scale = point3d(1, 0, 0);
	//physicBody = entity->AddComponent<PhysicBody>();
	////star->AddComponent<SphereCollider>();
	//constellation = entity->AddComponent<Constellation>();
	//constellation->stars = {
	//	point3d(0, 0, 0)
	//};
	//aiComponent = entity->AddComponent<AIComponent>();

	//aiComponent->patrolPoints = {
	//	 point3d(-10.0f, 0.0f, 0.0f),
	//	 point3d(0.0f, 0.0f, 10.0f)
	//};


	// MAIN MENU //
	
	//entity = m_World->CreateEntity();
	//transform = entity->AddComponent<Transform>();
	//transform->position = point3d(0.0f, 0.1f, 0.0f);
	//transform->scale = point3d(0.25f, 0.05f, 0.0f);
	//rect = entity->AddComponent<Rect>();
	//rect->color = point3d(0.5f, 0.25f, 0.8f);
	//rect->opacity = 1.f;
	////rect->anchorPoint = point3d(-1, 0, 0);
	//rect->ratio = ScreenAspectRatio::YY;
	//rect->cornerRadius = 0.25f;
	//rect->cornerType = CornerType::Strict;
	////entity->AddComponent<Button>();

	/*entity = m_World->CreateEntity();
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
	m_World->AddPhysicSystem<CollisionSystem>(collisionManager);
	m_World->AddPhysicSystem<CombatSystem>();
	m_World->AddPhysicSystem<EntityManagerSystem>();

	m_World->AddRenderSystem<MeshSystem>(m_World->m_Camera->frustum, m_World->m_Camera);
	m_World->AddRenderSystem<CollisionDrawSystem>(); // DEBUG //
	m_World->AddRenderSystem<SpriteSystem>(m_World->m_Camera->frustum);
	m_World->AddRenderSystem<UISystem>(mouse);

	//m_World->PreCalculations();
	bStar = new BaseStar();
	bStar->Init(m_World, entity);
	
	smallConstellation = new SmallConstellation();
	smallConstellation->Init(m_World, entity, player);

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

	
	// ��������� ������� ��� ���� ������
	DWORD currentTime = timer::currentTime;
	srand(time(0));
	// ������: ����� ������ 3 �������
	//if (currentTime - Star->LastTime > 3000) {
	//	Star->FartingEffect();
	//	// ��������� ����� �����
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

	if (currentTime - smallConstellation->LastTime > 5000) {

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
	smallConstellation->RamUpdate();

	ConstBuf::frame.aspect = XMFLOAT4{ float(window->aspect), float(window->iaspect), float(window->width), float(window->height) };

	m_World->UpdatePhysic();

	playerController->ProcessCamera();

	m_World->UpdateRender();

	mouse->RenderCursor();
	Draw::Present();
}
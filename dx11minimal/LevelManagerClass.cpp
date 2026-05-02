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


vector<Entity*> VPlants;

// __METODS GAMEJAM__ //
void Metamorf(TypePlant ColorPlant, Mutation Gain, std::string& TexturePlant)
{
	int PathSeed;

	PathSeed = ColorPlant + 1;

	string s = to_string(PathSeed);

	if (Gain == Mutation::SEED)
	{
		s = "Plant" + to_string(PathSeed) + "SEED";
		TexturePlant = s;
	}
	else if (Gain == Mutation::PLANT)
	{
		s = "Plant" + to_string(PathSeed) + "NORMAL";
		TexturePlant = s;
	}
	else if (Gain == Mutation::KIND_PLANT)
	{
		s = "Plant" + to_string(PathSeed) + "KIND";
		TexturePlant = s;
	}
	else if (Gain == Mutation::EVIL_PLANT)
	{
		s = "Plant" + to_string(PathSeed) + "EVIL";
		TexturePlant = s;
	}
	else if (Gain == Mutation::KILLER_PLANT)
	{
		s = "Plant" + to_string(PathSeed) + "KILER";
		TexturePlant = s;
	}

}
void StatusMood(float scaleS, StatusPlant Status, std::string& TextureEmogy)
{
	if (Status == StatusPlant::GOOD)
	{
		TextureEmogy = "LOVE_EMOGY";
	}
	else if (Status == StatusPlant::NORMAL)
	{
		TextureEmogy = "NORMAL_EMOGY";
	}
	else if (Status == StatusPlant::BAD)
	{
		TextureEmogy = "ANGRY_EMOGY";
	}
	else if (Status == StatusPlant::DEAD)
	{
		TextureEmogy = "DEAD_EMOGY";
	}
}
void MutationPlantation(ComponentPlants& PropPlant, float scaleS)
{
	float sec = 10;
	PropPlant.TimeGaine -= 1. / sec;
	float Scale = ((300 * PropPlant.TimeGaine) / 1000.);
	float scaleGain = Scale / 100;

	if (scaleGain >= 0.8)
		PropPlant.GainPlant = Mutation::SEED;
	else if (scaleGain >= 0.6 && scaleGain < 0.8)
		PropPlant.GainPlant = Mutation::PLANT;
	else if (scaleGain < 0.6 && scaleS > 0.7)
		PropPlant.GainPlant = Mutation::KIND_PLANT;
	else if (scaleGain < 0.6 && scaleS < 0.7 && scaleS > 0.3)
		PropPlant.GainPlant = Mutation::EVIL_PLANT;
	else if (scaleGain < 0.6 && scaleS < 0.3)
		PropPlant.GainPlant = Mutation::KILLER_PLANT;
}


void GameJamMetod(ComponentPlants& PropPlant)
{
	float sec = 1;
	PropPlant.LoyaltyScale -= 1. / sec;
	float Scale = ((120 * PropPlant.LoyaltyScale) / 1000.);
	float scaleS = Scale / 120;
	string str;
	point3d* scaleUI = &PropPlant.UiLine->GetComponent<Transform>()->scale;

	scaleUI->x = scaleS;
	if (scaleS >= 0.7)
	{
		PropPlant.Status = StatusPlant::GOOD;
		PropPlant.UiLine->GetComponent<Sprite>()->textureName = "ScaleLineG";
	}
	else if (scaleS >= 0.3 && scaleS < 0.7)
	{
		PropPlant.Status = StatusPlant::NORMAL;
		PropPlant.UiLine->GetComponent<Sprite>()->textureName = "ScaleLineY";
	}
	else if (scaleS < 0.3 && scaleS > 0.2)
	{
		PropPlant.Status = StatusPlant::BAD;
		PropPlant.UiLine->GetComponent<Sprite>()->textureName = "ScaleLineR";
	}
	else if (scaleS < 0.2)
	{
		PropPlant.Status = StatusPlant::DEAD;
		PropPlant.UiLine->GetComponent<Sprite>()->textureName = "ScaleLineR";
	}


	if (GetAsyncKeyState('E'))
		PropPlant.LoyaltyScale += 2;
	if (GetAsyncKeyState('Q'))
		PropPlant.LoyaltyScale -= 2;



	StatusMood(scaleS, PropPlant.Status, PropPlant.TextureEmogy);
	MutationPlantation(PropPlant, scaleS);
	Metamorf(PropPlant.TypeColorPlant, PropPlant.GainPlant, PropPlant.TexturePlant);
	PropPlant.Plant->GetComponent<Sprite>()->textureName = PropPlant.TexturePlant;
	PropPlant.Emoji->GetComponent<Sprite>()->textureName = PropPlant.TextureEmogy;

	if (scaleS < 0.01)
	{
		PropPlant.CheckCreate = false;
		return;
	}
}
// __METODS GAMEJAM__ //



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
	//Textures::LoadDDSTexture("gta", L"..\\dx11minimal\\Resourses\\Textures\\gta.dds");
	//Textures::LoadDDSTexture("aperture", L"..\\dx11minimal\\Resourses\\Textures\\aperture.dds");
	Textures::LoadPNGTexture("comicsSpot", L"..\\dx11minimal\\Resourses\\Textures\\comicsSpot.png");
	Textures::LoadPNGTexture("omniman", L"..\\dx11minimal\\Resourses\\Textures\\omniman.png");
	Textures::LoadPNGTexture("garden", L"..\\dx11minimal\\Resourses\\Textures\\garden.png");
	
	Textures::LoadPNGTexture("Plant1KILER" , L"..\\dx11minimal\\Resourses\\Textures\\A\\A.png");
	Textures::LoadPNGTexture("Plant1KIND"  , L"..\\dx11minimal\\Resourses\\Textures\\A\\B.png");
	Textures::LoadPNGTexture("Plant1SEED"  , L"..\\dx11minimal\\Resourses\\Textures\\A\\C.png");
	Textures::LoadPNGTexture("Plant1NORMAL", L"..\\dx11minimal\\Resourses\\Textures\\A\\D.png");
	Textures::LoadPNGTexture("Plant1EVIL"  , L"..\\dx11minimal\\Resourses\\Textures\\A\\E.png");

	Textures::LoadPNGTexture("Plant2KILER" , L"..\\dx11minimal\\Resourses\\Textures\\B\\A.png");
	Textures::LoadPNGTexture("Plant2KIND"  , L"..\\dx11minimal\\Resourses\\Textures\\B\\B.png");
	Textures::LoadPNGTexture("Plant2SEED"  , L"..\\dx11minimal\\Resourses\\Textures\\B\\C.png");
	Textures::LoadPNGTexture("Plant2NORMAL", L"..\\dx11minimal\\Resourses\\Textures\\B\\D.png");
	Textures::LoadPNGTexture("Plant2EVIL"  , L"..\\dx11minimal\\Resourses\\Textures\\B\\E.png");

	Textures::LoadPNGTexture("Plant3KILER" , L"..\\dx11minimal\\Resourses\\Textures\\C\\A.png");
	Textures::LoadPNGTexture("Plant3KIND"  , L"..\\dx11minimal\\Resourses\\Textures\\C\\B.png");
	Textures::LoadPNGTexture("Plant3SEED"  , L"..\\dx11minimal\\Resourses\\Textures\\C\\C.png");
	Textures::LoadPNGTexture("Plant3NORMAL", L"..\\dx11minimal\\Resourses\\Textures\\C\\D.png");
	Textures::LoadPNGTexture("Plant3EVIL"  , L"..\\dx11minimal\\Resourses\\Textures\\C\\E.png");

	Textures::LoadPNGTexture("Plant4KILER" , L"..\\dx11minimal\\Resourses\\Textures\\D\\A.png");
	Textures::LoadPNGTexture("Plant4KIND"  , L"..\\dx11minimal\\Resourses\\Textures\\D\\B.png");
	Textures::LoadPNGTexture("Plant4SEED"  , L"..\\dx11minimal\\Resourses\\Textures\\D\\C.png");
	Textures::LoadPNGTexture("Plant4NORMAL", L"..\\dx11minimal\\Resourses\\Textures\\D\\D.png");
	Textures::LoadPNGTexture("Plant4EVIL"  , L"..\\dx11minimal\\Resourses\\Textures\\D\\E.png");

	Textures::LoadPNGTexture("Plant5KILER" , L"..\\dx11minimal\\Resourses\\Textures\\F\\A.png");
	Textures::LoadPNGTexture("Plant5KIND"  , L"..\\dx11minimal\\Resourses\\Textures\\F\\B.png");
	Textures::LoadPNGTexture("Plant5SEED"  , L"..\\dx11minimal\\Resourses\\Textures\\F\\C.png");
	Textures::LoadPNGTexture("Plant5NORMAL", L"..\\dx11minimal\\Resourses\\Textures\\F\\D.png");
	Textures::LoadPNGTexture("Plant5EVIL"  , L"..\\dx11minimal\\Resourses\\Textures\\F\\E.png");

	Textures::LoadPNGTexture("Plant6KILER" , L"..\\dx11minimal\\Resourses\\Textures\\G\\A.png");
	Textures::LoadPNGTexture("Plant6KIND"  , L"..\\dx11minimal\\Resourses\\Textures\\G\\B.png");
	Textures::LoadPNGTexture("Plant6SEED"  , L"..\\dx11minimal\\Resourses\\Textures\\G\\C.png");
	Textures::LoadPNGTexture("Plant6NORMAL", L"..\\dx11minimal\\Resourses\\Textures\\G\\D.png");
	Textures::LoadPNGTexture("Plant6EVIL"  , L"..\\dx11minimal\\Resourses\\Textures\\G\\E.png");

	Textures::LoadPNGTexture("ScaleBar", L"..\\dx11minimal\\Resourses\\Textures\\I\\A.png");
	Textures::LoadPNGTexture("ScaleLineG", L"..\\dx11minimal\\Resourses\\Textures\\I\\B.png");
	Textures::LoadPNGTexture("ScaleLineY", L"..\\dx11minimal\\Resourses\\Textures\\I\\C.png");
	Textures::LoadPNGTexture("ScaleLineR", L"..\\dx11minimal\\Resourses\\Textures\\I\\D.png");

	Textures::LoadPNGTexture("ANGRY_EMOGY", L"..\\dx11minimal\\Resourses\\Textures\\L\\A.png");
	Textures::LoadPNGTexture("NORMAL_EMOGY", L"..\\dx11minimal\\Resourses\\Textures\\L\\B.png");
	Textures::LoadPNGTexture("LOVE_EMOGY", L"..\\dx11minimal\\Resourses\\Textures\\L\\C.png");
	Textures::LoadPNGTexture("DEAD_EMOGY", L"..\\dx11minimal\\Resourses\\Textures\\L\\D.png");

	Textures::LoadPNGTexture("Menu_Backpack", L"..\\dx11minimal\\Resourses\\Textures\\MENU_BACKPACK.png");
	Textures::LoadPNGTexture("Menu_Exit", L"..\\dx11minimal\\Resourses\\Textures\\MENU_EXIT.png");

	Textures::LoadPNGTexture("Menu_Dressing", L"..\\dx11minimal\\Resourses\\Textures\\MENU_1.png");
	Textures::LoadPNGTexture("Menu_Drink", L"..\\dx11minimal\\Resourses\\Textures\\MENU_2.png");
	Textures::LoadPNGTexture("Menu_Plants", L"..\\dx11minimal\\Resourses\\Textures\\MENU_3.png");

	Textures::LoadPNGTexture("Menu_Bar", L"..\\dx11minimal\\Resourses\\Textures\\MENU_BAR.png");
	Textures::LoadPNGTexture("Menu_Slot", L"..\\dx11minimal\\Resourses\\Textures\\MENU_Slot.png");

	Textures::LoadPNGTexture("Menu_Water", L"..\\dx11minimal\\Resourses\\Textures\\MENU_Water.png");
	Textures::LoadPNGTexture("Menu_Milk", L"..\\dx11minimal\\Resourses\\Textures\\MENU_Milk.png");
	Textures::LoadPNGTexture("Menu_Tea", L"..\\dx11minimal\\Resourses\\Textures\\MENU_Tea.png");
	Textures::LoadPNGTexture("Menu_Espresso", L"..\\dx11minimal\\Resourses\\Textures\\MENU_Coffe2.png");
	Textures::LoadPNGTexture("Menu_Americano", L"..\\dx11minimal\\Resourses\\Textures\\MENU_Coffe1.png");
	Textures::LoadPNGTexture("Menu_Raf", L"..\\dx11minimal\\Resourses\\Textures\\MENU_Coffe3.png");

	Textures::LoadPNGTexture("MENU_UP1", L"..\\dx11minimal\\Resourses\\Textures\\MENU_UP1.png");
	Textures::LoadPNGTexture("MENU_UP2", L"..\\dx11minimal\\Resourses\\Textures\\MENU_UP2.png");
	Textures::LoadPNGTexture("MENU_UP3", L"..\\dx11minimal\\Resourses\\Textures\\MENU_UP3.png");
	Textures::LoadPNGTexture("MENU_UP4", L"..\\dx11minimal\\Resourses\\Textures\\MENU_UP4.png");

	Textures::LoadPNGTexture("MENU_PlantBlue", L"..\\dx11minimal\\Resourses\\Textures\\MENU_Plant1.png");
	Textures::LoadPNGTexture("MENU_PlantYellow", L"..\\dx11minimal\\Resourses\\Textures\\MENU_Plant2.png");
	Textures::LoadPNGTexture("MENU_PlantRed", L"..\\dx11minimal\\Resourses\\Textures\\MENU_Plant3.png");
	Textures::LoadPNGTexture("MENU_PlantPurple", L"..\\dx11minimal\\Resourses\\Textures\\MENU_Plant4.png");
	Textures::LoadPNGTexture("MENU_PlantOrange", L"..\\dx11minimal\\Resourses\\Textures\\MENU_Plant5.png");
	Textures::LoadPNGTexture("MENU_PlantCyan", L"..\\dx11minimal\\Resourses\\Textures\\MENU_Plant6.png");



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

	CreateUI();
	CreateRoom();

	//////////////////////////////////////////////////////////////////////////////////////////////
	// WORLD CREATING END //
	//////////////////////////////////////////////////////////////////////////////////////////////

	InitSystems();

	playerController = new PlayerController();
	playerController->Initialize(player);

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
	//if (!window->IsActive())
	//	return;

	mouse->Update();

	interp::UpdateTweens();

	playerController->ProcessInput();
	playerController->ProcessMouse();
	playerController->abilities->Update();
	playerController->ProccessUI();


	if (PlayerBackPack.whatChange == true && PlayerBackPack.PlantInHand == false)
	{
		PlayerBackPack.ChangeItemInHands(m_World->entityStorage);
	}

	int i = 0;
	while (i < VPlants.size())
	{
		ComponentPlants* com = VPlants[i]->GetComponent<ComponentPlants>();
		if (com->CheckCreate == true)
		{
			i++;
			GameJamMetod(*com);
		}
		else
		{
			com->Plant->Destroy();
			VPlants.erase(VPlants.begin() + i);
		}
	}

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
	m_World->AddRenderSystem<Sprite3dSystem>();
	m_World->AddRenderSystem<UISystem>();
	m_World->AddRenderSystem<UITextSystem>();
}


void LevelManagerClass::LoadModels()
{
	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\FistBasePos.obj");
	Models::LoadObjModel("..\\dx11minimal\\Resourses\\Models\\Arrow.obj");
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

	/////////////////////////////////////////////////

	Entity* playerSprite = m_World->entityStorage->CreateEntity("WallSprite", player);

	transform = playerSprite->AddComponent<Transform>();
	transform->scale = point3d(1.5);
	transform->position = point3d(0, 0.2, 0.75);
	transform->mRotation = GetMatrixFromDirection(point3d(0, 1, 0).normalized(), point3d(0, 0, 1));

	Sprite* sprite = playerSprite->AddComponent<Sprite>();
	sprite->textureName = "omniman";

	return player;
}


void LevelManagerClass::CreateUI()
{
	Entity* entity;
	Transform2D* transform2D;
	Rect* rect;
	Button* button;
	TextLabel* textLabel;
	ImageLabel* imageLabel;

	Entity* uiFolder = m_World->entityStorage->CreateEntity("UI");

	entity = m_World->entityStorage->CreateEntity("HandSlot", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->position = point3d(-0.9f, -0.85f, 0.0f);
	transform2D->ratio = ScreenAspectRatio::YY;
	transform2D->scale = point3d(0.1f, 0.1f, 0.0f);
	imageLabel = entity->AddComponent<ImageLabel>();
	imageLabel->textureName = "Menu_Slot";

	CreateInventoryUI(uiFolder);
}


void LevelManagerClass::CreateInventoryUI(Entity* uiFolder)
{
	Entity* entity;
	Transform2D* transform2D;
	Rect* rect;
	Button* button;
	TextLabel* textLabel;
	ImageLabel* imageLabel;

	// Инвентарь
	entity = m_World->entityStorage->CreateEntity("InventoryButton", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->ratio = ScreenAspectRatio::YY;
	transform2D->position = point3d(0.9f, 0.85f, 0.0f);
	transform2D->scale = point3d(0.1f, 0.1f, 0.0f);
	imageLabel = entity->AddComponent<ImageLabel>();
	imageLabel->textureName = "Menu_Backpack";
	button = entity->AddComponent<Button>();
	button->opacity = 0;

	// Окно инвентаря
	Entity* inventoryWindow = m_World->entityStorage->CreateEntity("InventoryWindow", uiFolder);
	transform2D = inventoryWindow->AddComponent<Transform2D>();
	transform2D->ratio = ScreenAspectRatio::YY;
	transform2D->position = point3d(0.0f, 0.0f, 0.0f);
	transform2D->scale = point3d(0.5f, 0.25f, 0.0f);
	imageLabel = inventoryWindow->AddComponent<ImageLabel>();
	imageLabel->textureName = "Menu_Bar";
	inventoryWindow->SetActive(false);

	Entity* drinksList = m_World->entityStorage->CreateEntity("DrinksList", inventoryWindow);
	transform2D = drinksList->AddComponent<Transform2D>();
	transform2D->ratio = ScreenAspectRatio::YY;

	Entity* dressingList = m_World->entityStorage->CreateEntity("DressingList", inventoryWindow);
	transform2D = dressingList->AddComponent<Transform2D>();
	transform2D->ratio = ScreenAspectRatio::YY;
	dressingList->SetActive(false);

	Entity* plantsList = m_World->entityStorage->CreateEntity("PlantsList", inventoryWindow);
	transform2D = plantsList->AddComponent<Transform2D>();
	transform2D->ratio = ScreenAspectRatio::YY;
	plantsList->SetActive(false);

	for (int b = 1; b > -2; b -= 2) {
		for (int a = -1; a < 2; a++) {
			entity = m_World->entityStorage->CreateEntity("Slot", drinksList);
			transform2D = entity->AddComponent<Transform2D>();
			transform2D->position = point3d((float)a * 0.3f, (float)b * 0.5f, 0.0f);
			transform2D->ratio = ScreenAspectRatio::YY;
			transform2D->scale = point3d(0.2f, 0.4f, 0.0f);
			imageLabel = entity->AddComponent<ImageLabel>();
			imageLabel->textureName = "Menu_Slot";
			button = entity->AddComponent<Button>();
			button->opacity = 0;

			entity = m_World->entityStorage->CreateEntity("Count", entity);
			transform2D = entity->AddComponent<Transform2D>();
			transform2D->ratio = ScreenAspectRatio::YY;
			transform2D->position = point3d(0.35, -0.5, 0);
			TextLabel* textLabel = entity->AddComponent<TextLabel>();
			textLabel->textW = L"0";
			textLabel->fontFamilyW = L"Impact";
			textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
			textLabel->fontWeight = 900;
			textLabel->fontSizePx = 44;
			textLabel->fontScale = 0.5f;
			textLabel->letterSpacingPx = 1.0f;
			textLabel->color = point3d();
		}
	}
	
	for (int b = 1; b > -2; b -= 2) {
		for (int a = -1; a < 2; a++) {
			entity = m_World->entityStorage->CreateEntity("Slot", dressingList);
			transform2D = entity->AddComponent<Transform2D>();
			transform2D->position = point3d((float)a * 0.3f, (float)b * 0.5f, 0.0f);
			transform2D->ratio = ScreenAspectRatio::YY;
			transform2D->scale = point3d(0.2f, 0.4f, 0.0f);
			imageLabel = entity->AddComponent<ImageLabel>();
			imageLabel->textureName = "Menu_Slot";
			button = entity->AddComponent<Button>();
			button->opacity = 0;

			entity = m_World->entityStorage->CreateEntity("Count", entity);
			transform2D = entity->AddComponent<Transform2D>();
			transform2D->ratio = ScreenAspectRatio::YY;
			transform2D->position = point3d(0.35, -0.5, 0);
			TextLabel* textLabel = entity->AddComponent<TextLabel>();
			textLabel->textW = L"0";
			textLabel->fontFamilyW = L"Impact";
			textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
			textLabel->fontWeight = 900;
			textLabel->fontSizePx = 44;
			textLabel->fontScale = 0.5f;
			textLabel->letterSpacingPx = 1.0f;
			textLabel->color = point3d();
		}
	}

	for (int b = 1; b > -2; b -= 2) {
		for (int a = -1; a < 2; a++) {
			entity = m_World->entityStorage->CreateEntity("Slot", plantsList);
			transform2D = entity->AddComponent<Transform2D>();
			transform2D->position = point3d((float)a * 0.3f, (float)b * 0.5f, 0.0f);
			transform2D->ratio = ScreenAspectRatio::YY;
			transform2D->scale = point3d(0.2f, 0.4f, 0.0f);
			imageLabel = entity->AddComponent<ImageLabel>();
			imageLabel->textureName = "Menu_Slot";
			button = entity->AddComponent<Button>();
			button->opacity = 0;

			entity = m_World->entityStorage->CreateEntity("Count", entity);
			transform2D = entity->AddComponent<Transform2D>();
			transform2D->ratio = ScreenAspectRatio::YY;
			transform2D->position = point3d(0.35, -0.5, 0);
			TextLabel* textLabel = entity->AddComponent<TextLabel>();
			textLabel->textW = L"0";
			textLabel->fontFamilyW = L"Impact";
			textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
			textLabel->fontWeight = 900;
			textLabel->fontSizePx = 44;
			textLabel->fontScale = 0.5f;
			textLabel->letterSpacingPx = 1.0f;
			textLabel->color = point3d();
		}
	}

	// Кнопка закрытия инвентаря
	entity = m_World->entityStorage->CreateEntity("InventoryExit", inventoryWindow);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(-1.0f, -1.0f, 0.0f);
	transform2D->position = point3d(0.6f, 1.0f, 0.0f);
	transform2D->ratio = ScreenAspectRatio::YY;
	transform2D->scale = point3d(0.13f, 0.25f, 0.0f);
	imageLabel = entity->AddComponent<ImageLabel>();
	imageLabel->textureName = "Menu_Exit";
	button = entity->AddComponent<Button>();
	button->opacity = 0;

	// Напитки
	entity = m_World->entityStorage->CreateEntity("DrinksButton", inventoryWindow);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(0.0f, -1.0f, 0.0f);
	transform2D->position = point3d(0.0f, 1.05f, 0.0f);
	transform2D->ratio = ScreenAspectRatio::YY;
	transform2D->scale = point3d(0.24f, 0.5f, 0.0f);
	imageLabel = entity->AddComponent<ImageLabel>();
	imageLabel->textureName = "Menu_Drink";
	button = entity->AddComponent<Button>();
	button->opacity = 0;

	// Удобрения
	entity = m_World->entityStorage->CreateEntity("DressingButton", inventoryWindow);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(0.0f, -1.0f, 0.0f);
	transform2D->position = point3d(-0.3f, 1.05f, 0.0f);
	transform2D->ratio = ScreenAspectRatio::YY;
	transform2D->scale = point3d(0.24f, 0.5f, 0.0f);
	imageLabel = entity->AddComponent<ImageLabel>();
	imageLabel->textureName = "Menu_Dressing";
	imageLabel->opacity = 0.65f;
	button = entity->AddComponent<Button>();
	button->opacity = 0;

	// Растения
	entity = m_World->entityStorage->CreateEntity("PlantsButton", inventoryWindow);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->anchorPoint = point3d(0.0f, -1.0f, 0.0f);
	transform2D->position = point3d(0.3f, 1.05f, 0.0f);
	transform2D->ratio = ScreenAspectRatio::YY;
	transform2D->scale = point3d(0.24f, 0.5f, 0.0f);
	imageLabel = entity->AddComponent<ImageLabel>();
	imageLabel->textureName = "Menu_Plants";
	imageLabel->opacity = 0.65f;
	button = entity->AddComponent<Button>();
	button->opacity = 0;
}




void LevelManagerClass::CreateRoom()
{
	Entity* room;
	Entity* wall;
	Entity* wallSprite;
	Transform* transform;
	PlaneCollider* planeCollider;
	Sprite* sprite;

	room = m_World->entityStorage->CreateEntity("Room");
	transform = room->AddComponent<Transform>();

	////////////////////////////////

	wall = m_World->entityStorage->CreateEntity("Wall", room);
	transform = wall->AddComponent<Transform>();
	transform->position = point3d(0, -10, 0);
	planeCollider = wall->AddComponent<PlaneCollider>();
	planeCollider->normal = point3d(0, 1, 0);

	////////////////////////////////

	wall = m_World->entityStorage->CreateEntity("Wall", room);
	transform = wall->AddComponent<Transform>();
	transform->scale = point3d(1, 10, 1);
	transform->position = point3d(10, 0, 0);
	transform->mRotation = GetMatrixFromDirection(point3d(0, 1, 0).normalized(), point3d(0, 0, 1));
	planeCollider = wall->AddComponent<PlaneCollider>();
	planeCollider->normal = point3d(-1, 0, 0);

	wallSprite = m_World->entityStorage->CreateEntity("WallSprite", wall);
	transform = wallSprite->AddComponent<Transform>();
	transform->position = point3d(-1, 0, 0);
	sprite = wallSprite->AddComponent<Sprite>();
	sprite->textureName = "comicsSpot";

	////////////////////////////////

	wall = m_World->entityStorage->CreateEntity("Wall", room);
	transform = wall->AddComponent<Transform>();
	transform->scale = point3d(1, 10, 1);
	transform->position = point3d(-10, 0, 0);
	transform->mRotation = GetMatrixFromDirection(point3d(0, 1, 0).normalized(), point3d(0, 0, 1));
	planeCollider = wall->AddComponent<PlaneCollider>();
	planeCollider->normal = point3d(1, 0, 0);

	wallSprite = m_World->entityStorage->CreateEntity("WallSprite", wall);
	transform = wallSprite->AddComponent<Transform>();
	transform->position = point3d(1, 0, 0);
	sprite = wallSprite->AddComponent<Sprite>();
	sprite->textureName = "comicsSpot";

	////////////////////////////////

	wall = m_World->entityStorage->CreateEntity("Wall", room);
	transform = wall->AddComponent<Transform>();
	transform->scale = point3d(10, 1, 1);
	transform->position = point3d(0, 0, 8);
	transform->mRotation = GetMatrixFromDirection(point3d(0, 1, 0).normalized(), point3d(0, 0, 1));
	planeCollider = wall->AddComponent<PlaneCollider>();
	planeCollider->normal = point3d(0, 0, -1);

	wallSprite = m_World->entityStorage->CreateEntity("WallSprite", wall);
	transform = wallSprite->AddComponent<Transform>();
	transform->position = point3d(0, 3, 0);
	sprite = wallSprite->AddComponent<Sprite>();
	sprite->textureName = "comicsSpot";

	////////////////////////////////

	wall = m_World->entityStorage->CreateEntity("Wall", room);
	transform = wall->AddComponent<Transform>();
	transform->scale = point3d(-10, 1, 1);
	transform->position = point3d(0, 0, -10);
	transform->mRotation = GetMatrixFromDirection(point3d(0, 1, 0).normalized(), point3d(0, 0, 1));
	planeCollider = wall->AddComponent<PlaneCollider>();
	planeCollider->normal = point3d(0, 0, 1);

	wallSprite = m_World->entityStorage->CreateEntity("WallSprite", wall);
	transform = wallSprite->AddComponent<Transform>();
	transform->position = point3d(0, -1, 0);
	sprite = wallSprite->AddComponent<Sprite>();
	sprite->textureName = "comicsSpot";

	CreateGardens(room);
	
}


void LevelManagerClass::CreateGardens(Entity* room)
{
	Entity* garden;
	Transform* transform;
	Sprite* sprite;
	SphereCollider* sphereCollider;

	for (int i = -9; i < 10; i += 2) {
		garden = m_World->entityStorage->CreateEntity("WallSprite", room);

		transform = garden->AddComponent<Transform>();
		transform->position = point3d(i, 0, 9);
		transform->mRotation = GetMatrixFromDirection(point3d(0, 1, 0).normalized(), point3d(0, 0, 1));

		sprite = garden->AddComponent<Sprite>();
		sprite->textureName = "garden";

		CreatePlant(garden);
		
		sphereCollider = garden->AddComponent<SphereCollider>();
		sphereCollider->radius = 1.25f;

	}
}




Entity* LevelManagerClass::CreateUIPlant(Entity* Plant)
{
	Entity* UIPlantBar;
	Transform* transformBar;
	Sprite* spriteLineBar;
	UIPlantBar = m_World->entityStorage->CreateEntity("UIBar", Plant);
	transformBar = UIPlantBar->AddComponent<Transform>();
	transformBar->position = point3d(0, 1, 0.2);
	transformBar->scale = point3d(1, 0.1, 1);
	spriteLineBar = UIPlantBar->AddComponent<Sprite>();
	spriteLineBar->textureName = "ScaleBar";
	
	
	Entity* UIPlant;
	Transform* transform;
	Sprite* spriteLine;
	UIPlant = m_World->entityStorage->CreateEntity("UILine", UIPlantBar);
	transform = UIPlant->AddComponent<Transform>();
	transform->position = point3d(0, 0, 0.3);
	spriteLine = UIPlant->AddComponent<Sprite>();
	spriteLine->textureName = "ScaleLineG";

	return UIPlant;
}

Entity* LevelManagerClass::CreateEmogy(Entity* Plant)
{
	Entity* PlantEmoji;
	Transform* transformEmoji;
	Sprite* spriteLineEmoji;

	PlantEmoji = m_World->entityStorage->CreateEntity("PlantEMOJI", Plant);

	transformEmoji = PlantEmoji->AddComponent<Transform>();
	transformEmoji->position = point3d(-0.7, 1.4, 0.4);
	transformEmoji->scale = point3d(0.4, 0.4, 0.1);
	spriteLineEmoji = PlantEmoji->AddComponent<Sprite>();
	spriteLineEmoji->textureName = "LOVE_EMOGY";

	return PlantEmoji;
}

void LevelManagerClass::CreatePlant(Entity* Garden)
{
	Entity* Plant;
	Transform* transform;
	Sprite* spritePlant;
	ComponentPlants* PropPlant;

	Plant = m_World->entityStorage->CreateEntity("Plant", Garden);

	transform = Plant->AddComponent<Transform>();
	transform->position = point3d(0, 0.1, 0.1);

	spritePlant = Plant->AddComponent<Sprite>();
	PropPlant = Plant->AddComponent<ComponentPlants>();

	spritePlant->textureName = "Plant1KILER";

	PropPlant->TexturePlant = spritePlant->textureName;
	PropPlant->Plant = Plant;


	PropPlant->UiLine = CreateUIPlant(Plant);
	PropPlant->Emoji = CreateEmogy(Plant);
	PropPlant->Garden = Garden;
	VPlants.push_back(Plant);
}

//Textures::LoadPNGTexture("Plant1KILER", L"..\\dx11minimal\\Resourses\\Textures\\G\\A.png");
//Textures::LoadPNGTexture("Plant1KIND", L"..\\dx11minimal\\Resourses\\Textures\\G\\B.png");
//Textures::LoadPNGTexture("Plant1SEED", L"..\\dx11minimal\\Resourses\\Textures\\G\\C.png");
//Textures::LoadPNGTexture("Plant1NORMAL", L"..\\dx11minimal\\Resourses\\Textures\\G\\D.png");
//Textures::LoadPNGTexture("Plant1EVIL", L"..\\dx11minimal\\Resourses\\Textures\\G\\E.png");
//
//Textures::LoadPNGTexture("ScaleBar", L"..\\dx11minimal\\Resourses\\Textures\\I\\A.png");
//Textures::LoadPNGTexture("ScaleLineG", L"..\\dx11minimal\\Resourses\\Textures\\I\\B.png");
//Textures::LoadPNGTexture("ScaleLineY", L"..\\dx11minimal\\Resourses\\Textures\\I\\C.png");
//Textures::LoadPNGTexture("ScaleLineR", L"..\\dx11minimal\\Resourses\\Textures\\I\\D.png");
//
//Textures::LoadPNGTexture("ANGRY_EMOGY", L"..\\dx11minimal\\Resourses\\Textures\\L\\A.png");
//Textures::LoadPNGTexture("NORMAL_EMOGY", L"..\\dx11minimal\\Resourses\\Textures\\L\\B.png");
//Textures::LoadPNGTexture("LOVE_EMOGY", L"..\\dx11minimal\\Resourses\\Textures\\L\\C.png");
//Textures::LoadPNGTexture("DEAD_EMOGY", L"..\\dx11minimal\\Resourses\\Textures\\L\\D.png");
#include "PlayerController.h"

#include "../UI/Text/TextLabel.h"



using namespace std;


BackPacks PlayerBackPack;

std::string itemTextures[16] = {
	"MENU_PlantBlue",
	"MENU_PlantYellow",
	"MENU_PlantRed",
	"MENU_PlantPurple",
	"MENU_PlantOrange",
	"MENU_PlantCyan",

	"Menu_Water",
	"Menu_Milk",
	"Menu_Tea",
	"Menu_Espresso",
	"Menu_Americano",
	"Menu_Raf",

	"MENU_UP1",
	"MENU_UP2",
	"MENU_UP3",
	"MENU_UP4",
};

PlayerController::PlayerController()
{
	playerEntity = 0;

	playerTransform = 0;
	playerPhysicBody = 0;
	playerPointCloud = 0;

	camera = 0;
	mouse = 0;
	window = 0;
}

PlayerController::PlayerController(const PlayerController& other)
{
}


PlayerController::~PlayerController()
{
}


void PlayerController::Initialize(Entity* Player)
{
	playerEntity = Player;

	playerTransform = Player->GetComponent<Transform>();
	playerPhysicBody = Player->GetComponent<PhysicBody>();
	playerPointCloud = Player->GetComponent<PointCloud>();

	World* world = Singleton::GetInstance<World>();

	ui = world->entityStorage->GetEntityByName("UI");

	inventoryButton = ui->GetChildByName("InventoryButton", true)->GetComponent<Button>();
	inventoryWindow = ui->GetChildByName("InventoryWindow");
	inventoryExit = inventoryWindow->GetChildByName("InventoryExit")->GetComponent<Button>();

	drinksButton = inventoryWindow->GetChildByName("DrinksButton");
	dressingButton = inventoryWindow->GetChildByName("DressingButton");
	plantsButton = inventoryWindow->GetChildByName("PlantsButton");

	drinksList = inventoryWindow->GetChildByName("DrinksList");
	dressingList = inventoryWindow->GetChildByName("DressingList");
	plantsList = inventoryWindow->GetChildByName("PlantsList");

	camera = world->m_Camera;
	mouse = Singleton::GetInstance<MouseClass>();
	window = Singleton::GetInstance<WindowClass>();
	entityStorage = Singleton::GetInstance<EntityStorage>();
	collisionManager = Singleton::GetInstance<CollisionManagerClass>();

	handSlot = entityStorage->GetEntityByName("HandSlot")->GetComponent<ImageLabel>();
	plantSlot = entityStorage->GetEntityByName("PlantSlot")->GetComponent<ImageLabel>();

	abilities = Singleton::GetInstance<PlayerAbilities>();
	abilities->Initialize(playerEntity, entityStorage);

	comboManager = Singleton::GetInstance<ComboManager>();
	comboManager->Initialize();

	cameraTarget = nullptr;
	lockMovementOnTarget = false;

	SetCursorPos(window->width / 2, window->height / 2);
}


void PlayerController::Shutdown()
{
	if (abilities) {
		abilities->Shutdown();
		delete abilities;
		abilities = 0;
	}

	if (playerEntity)
		playerEntity = 0;

	if (playerTransform)
		playerTransform = 0;

	if (playerPhysicBody)
		playerPhysicBody = 0;

	if (playerPointCloud)
		playerPointCloud = 0;

	if (ui)
		ui = 0;

	if (camera)
		camera = 0;

	if (mouse)
		mouse = 0;

	if (window)
		window = 0;
}


void PlayerController::ProcessInput()
{
	if (playerEntity == nullptr || !playerEntity->IsActive())
		return;

	if (movementLocked && playerPhysicBody->velocity.magnitude() < PLAYER_MOVE_SPEED) {
		movementLocked = false;
		playerPhysicBody->airFriction = 1;
	}

	if (!movementLocked)
	{
		point3d velocity = point3d();

		if (input::IsKeyDown('W')) {
			velocity += playerTransform->GetLookVector();
		}
		if (input::IsKeyDown('S')) {
			velocity += playerTransform->GetLookVector() * -1;
		}
		if (input::IsKeyDown('A')) {
			velocity += playerTransform->GetRightVector() * -1;
		}
		if (input::IsKeyDown('D')) {
			velocity += playerTransform->GetRightVector();
		}
		if (velocity.magnitude() > 0) {
			playerPhysicBody->acceleration += velocity.normalized() * PLAYER_MOVE_SPEED;
		}
	}

	if (input::IsKeyPressed('E')) {
		inventoryWindow->SetActive(!inventoryWindow->IsLocalActive());
	}


	//if (input::IsKeyPressed('1'))
	//{
	//	PlayerBackPack.ItemInHand = ItemsInBackPack::RED;
	//}
	//if (input::IsKeyPressed('2'))
	//{
	//	PlayerBackPack.ItemInHand = ItemsInBackPack::BLUE;
	//}
	//if (input::IsKeyPressed('3'))
	//{
	//	PlayerBackPack.ItemInHand = ItemsInBackPack::RED;
	//}
	//if (input::IsKeyPressed('4'))
	//{
	//	PlayerBackPack.ItemInHand = ItemsInBackPack::WATER;
	//}
	//if (input::IsKeyPressed('5'))
	//{
	//	PlayerBackPack.ItemInHand = ItemsInBackPack::WATER;
	//}
	//if (input::IsKeyPressed('6')) // reset item
	//{
	//	PlayerBackPack.ResetItem();
	//}
	if (input::IsKeyPressed('7')) // reset item
	{
		GameReset();
	}
}
		


void PlayerController::ProcessCamera()
{
	camera->position = point3d(0, camera->distance, 0).lerp(playerTransform->position + point3d(0, camera->distance, 0), 0.2f);

	XMMATRIX matrixRotation = GetMatrixFromDirection(point3d(0, -1, 0), playerTransform->GetLookVector());

	camera->SetMatrixRotation(LerpMatrix(camera->GetMatrixRotation(), matrixRotation, 0.15f));
}




void PlayerController::ProcessMouse()
{
	float x = mouse->absolutePos.x - window->width / 2;
	float y = mouse->absolutePos.y - window->height / 2;
	point3d mousePos = point3d(x / window->width / window->aspect, y / window->height, 0);

	switch (mouse->state)
	{
	case MouseState::Centered:
	{
		if (playerEntity != nullptr && playerEntity->IsActive()) {
			float length = mousePos.magnitude();

			if (length > CURSOR_IGNORE_ZONE) {
				if (length > MAX_CURSOR_DEVIATION) {
					mousePos = mousePos.normalized() * MAX_CURSOR_DEVIATION;

					mouse->absolutePos = point3d(mousePos.x * window->width * window->aspect + window->width / 2, mousePos.y * window->height + window->height / 2, 0);
					mouse->pos = point3d(mouse->absolutePos.x / window->width * 2 - 1, -(mouse->absolutePos.y / window->height * 2 - 1), 0.0f);

					SetCursorPos(mouse->absolutePos.x, mouse->absolutePos.y);
				}

				if (!lockMovementOnTarget || cameraTarget == nullptr) {
					float k = (length - CURSOR_IGNORE_ZONE) / MAX_CURSOR_DEVIATION;
					mousePos *= SENSIVITY * k;

					XMMATRIX additionalRotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(mousePos.y), XMConvertToRadians(mousePos.x), 0);

					playerPhysicBody->mAngVelocity = playerPhysicBody->mAngVelocity * additionalRotation;
				}
			}
		}
		break;
	}
	case MouseState::Free:
	{
		if (mouse->IsLButtonClicked() && mouse->visible)
		{
			for (int i = 0; i < 20; i++)
			{
				MouseParticle particle = MouseParticle();
				particle.pos = mouse->pos;
				particle.angle = (float)getRandom(0, 100) / 100.0f * PI * 2.0f;
				particle.lifetime = getRandom(500, 1500);
				particle.startTime = timer::currentTime;

				mouse->particles.push_back(particle);
			}

			ClickOnObjectL();
		}
		if (mouse->IsRButtonClicked() && mouse->visible)
		{
			for (int i = 0; i < 5; i++)
			{
				MouseParticle particle = MouseParticle();
				particle.pos = mouse->pos;
				particle.angle = (float)getRandom(0, 100) / 100.0f * PI * 2.0f;
				particle.lifetime = getRandom(500, 1500);
				particle.startTime = timer::currentTime;
				
				mouse->particles.push_back(particle);
			}

			ClickOnObjectR();
		}

		break;
	}
	}

	point3d mouseDirection = mouse->GetMouseDirection();
	
	/*Entity* e = entityStorage->CreateEntity("beam");
	e->AddComponent<Transform>();
	Beam* beam = e->AddComponent<Beam>();
	beam->point1 = camera->position;
	beam->point2 = camera->position + mouseDirection * RAY_DISTANCE;
	DelayedDestroy* d = e->AddComponent<DelayedDestroy>();
	d->lifeTime = 5;*/

	RayInfo rayInfo = RayInfo(camera->position, mouseDirection * RAY_DISTANCE, CollisionFilter::Group::PlayerRay, false);
	RaycastResult result = collisionManager->Raycast(rayInfo);

	target = result.hit ? result.entity : nullptr;
}

Entity* CreateUIPlant(Entity* Plant, EntityStorage* entityStorage)
{
	Entity* UIPlantBar;
	Transform* transformBar;
	Sprite* spriteLineBar;
	UIPlantBar = entityStorage->CreateEntity("UIBar", Plant);
	transformBar = UIPlantBar->AddComponent<Transform>();
	transformBar->position = point3d(0, 1, 0.2);
	transformBar->scale = point3d(1, 0.1, 1);
	spriteLineBar = UIPlantBar->AddComponent<Sprite>();
	spriteLineBar->textureName = "ScaleBar";


	Entity* UIPlant;
	Transform* transform;
	Sprite* spriteLine;
	UIPlant = entityStorage->CreateEntity("UILine", UIPlantBar);
	transform = UIPlant->AddComponent<Transform>();
	transform->position = point3d(0, 0, 0.3);
	spriteLine = UIPlant->AddComponent<Sprite>();
	spriteLine->textureName = "ScaleLineG";

	return UIPlant;
}

Entity* CreateEmogy(Entity* Plant, EntityStorage* entityStorage)
{
	Entity* PlantEmoji;
	Transform* transformEmoji;
	Sprite* spriteLineEmoji;

	PlantEmoji = entityStorage->CreateEntity("PlantEMOJI", Plant);

	transformEmoji = PlantEmoji->AddComponent<Transform>();
	transformEmoji->position = point3d(-0.7, 1.4, 0.4);
	transformEmoji->scale = point3d(0.4, 0.4, 0.1);
	spriteLineEmoji = PlantEmoji->AddComponent<Sprite>();
	spriteLineEmoji->textureName = "LOVE_EMOGY";

	return PlantEmoji;
}

Entity* CreateBuffInfo(Entity* Plant, EntityStorage* entityStorage)
{
	Entity* Plantbuff;
	Transform* transformbuff;
	Sprite* spriteLinebuff;

	Plantbuff = entityStorage->CreateEntity("PlantBuff", Plant);

	transformbuff = Plantbuff->AddComponent<Transform>();
	transformbuff->position = point3d(0.8, 1.3, 0.5);
	transformbuff->scale = point3d(0.2, 0.2, 0.1);
	spriteLinebuff = Plantbuff->AddComponent<Sprite>();
	spriteLinebuff->textureName = "BUFF_PLUS1";

	return Plantbuff;
}

void CreatePlant(Entity* Garden, EntityStorage* entityStorage , int Color)
{
	Entity* Plant;
	Transform* transform;
	Sprite* spritePlant;
	ComponentPlants* PropPlant;

	Plant = entityStorage->CreateEntity("Plant", Garden);

	transform = Plant->AddComponent<Transform>();
	transform->position = point3d(0, 0.65, 0.1);

	spritePlant = Plant->AddComponent<Sprite>();
	PropPlant = Plant->AddComponent<ComponentPlants>();

	spritePlant->textureName = "Plant1KILER";

	PropPlant->TexturePlant = spritePlant->textureName;
	PropPlant->Plant = Plant;

	PropPlant->TypeColorPlant = (TypePlant)Color;
	PropPlant->UiLine = CreateUIPlant(Plant, entityStorage);
	PropPlant->Emoji = CreateEmogy(Plant, entityStorage);
	PropPlant->BuffInfo = CreateBuffInfo(Plant, entityStorage);
	PropPlant->Garden = Garden;
	PlayerBackPack.VPlants.push_back(Plant);
}

void PlayerController::ProccessUI()
{
	if (inventoryWindow->IsActive()) {
		if (inventoryExit->isClicked) {
			inventoryExit->isClicked = false;
			inventoryWindow->SetActive(false);
		}

		if (drinksButton->GetComponent<Button>()->isClicked) {
			drinksButton->GetComponent<ImageLabel>()->opacity = 1.0f;
			dressingButton->GetComponent<ImageLabel>()->opacity = 0.65f;
			plantsButton->GetComponent<ImageLabel>()->opacity = 0.65f;

			drinksList->SetActive(true);
			dressingList->SetActive(false);
			plantsList->SetActive(false);
		}

		if (dressingButton->GetComponent<Button>()->isClicked) {
			drinksButton->GetComponent<ImageLabel>()->opacity = 0.65f;
			dressingButton->GetComponent<ImageLabel>()->opacity = 1.0f;
			plantsButton->GetComponent<ImageLabel>()->opacity = 0.65f;

			drinksList->SetActive(false);
			dressingList->SetActive(true);
			plantsList->SetActive(false);
		}

		if (plantsButton->GetComponent<Button>()->isClicked) {
			drinksButton->GetComponent<ImageLabel>()->opacity = 0.65f;
			dressingButton->GetComponent<ImageLabel>()->opacity = 0.65f;
			plantsButton->GetComponent<ImageLabel>()->opacity = 1.0f;

			drinksList->SetActive(false);
			dressingList->SetActive(false);
			plantsList->SetActive(true);
		}

		std::vector<Entity*> children = drinksList->GetChildren();
		for (int i = 0; i < children.size(); i++) {
			Entity* slot = children[i];
			Entity* count = slot->GetChildByName("Count");
			ItemsBackPack& item = PlayerBackPack.ListItems[i + 6];

			if (item.Count > 0) {
				count->SetActive(true);
				count->GetComponent<TextLabel>()->textW = std::to_wstring(item.Count);

				slot->GetComponent<ImageLabel>()->textureName = itemTextures[i + 6];

				if (slot->GetComponent<Button>()->isClicked) {
					PlayerBackPack.ItemInHand = item.Name;
					handSlot->textureName = itemTextures[item.Name];
				}
			}
			else {
				count->SetActive(false);
				slot->GetComponent<ImageLabel>()->textureName = "Menu_Slot";
			}
		}

		children = dressingList->GetChildren();
		for (int i = 0; i < children.size(); i++) {
			Entity* slot = children[i];
			Entity* count = slot->GetChildByName("Count");

			if (i + 12 >= std::size(PlayerBackPack.ListItems)) {
				count->SetActive(false);
				slot->GetComponent<ImageLabel>()->textureName = "Menu_Slot";
				continue;
			}

			ItemsBackPack& item = PlayerBackPack.ListItems[i + 12];

			if (item.Count > 0) {
				count->SetActive(true);
				count->GetComponent<TextLabel>()->textW = std::to_wstring(item.Count);

				slot->GetComponent<ImageLabel>()->textureName = itemTextures[i + 12];

				if (slot->GetComponent<Button>()->isClicked) {
					PlayerBackPack.ItemInHand = item.Name;
					handSlot->textureName = itemTextures[item.Name];
				}
			}
			else {
				count->SetActive(false);
				slot->GetComponent<ImageLabel>()->textureName = "Menu_Slot";
			}
		}

		children = plantsList->GetChildren();
		for (int i = 0; i < children.size(); i++) {
			Entity* slot = children[i];
			Entity* count = slot->GetChildByName("Count");
			ItemsBackPack& item = PlayerBackPack.ListItems[i];
			
			if (item.Count > 0) {
				count->SetActive(true);
				count->GetComponent<TextLabel>()->textW = std::to_wstring(item.Count);

				slot->GetComponent<ImageLabel>()->textureName = itemTextures[i];

				if (slot->GetComponent<Button>()->isClicked) {
					PlayerBackPack.ItemInHand = item.Name;
					handSlot->textureName = itemTextures[item.Name];
				}
			}
			else {
				count->SetActive(false);
				slot->GetComponent<ImageLabel>()->textureName = "Menu_Slot";
			}
		}
	}

	if (inventoryButton->isClicked) {
		inventoryWindow->SetActive(!inventoryWindow->IsLocalActive());
	}
}

void PlayerController::ClickOnObjectL()
{
	if (target != nullptr) {
		Transform targetTransform = GetWorldTransform(target);
		Transform palyerTransform = GetWorldTransform(playerEntity);

		if ((palyerTransform.position - targetTransform.position).magnitude() > INTERACT_DISTANCE)
			return;

		Sprite* sprite = target->GetComponent<Sprite>();

		if (sprite) {
			Entity* Plant;

			if (target->GetChildByName("Plant") == 0 && 
				PlayerBackPack.ChangeCountItem(PlayerBackPack.ListItems[PlayerBackPack.ItemInHand].Count) == true 
				&& PlayerBackPack.PlantInHand == false 
				)
			{
				if (PlayerBackPack.ItemInHand == ItemsInBackPack::BLUE ||
					PlayerBackPack.ItemInHand == ItemsInBackPack::RED ||
					PlayerBackPack.ItemInHand == ItemsInBackPack::YELLOW ||
					PlayerBackPack.ItemInHand == ItemsInBackPack::PURPLE ||
					PlayerBackPack.ItemInHand == ItemsInBackPack::ORANGE ||
					PlayerBackPack.ItemInHand == ItemsInBackPack::CYAN)
				{
					CreatePlant(target, entityStorage, PlayerBackPack.ItemInHand);
					Plant = target->GetChildByName("Plant");
					PlayerBackPack.UseItem(Plant);
				}
			}
			else if (target->GetChildByName("Plant") != 0 && 
				PlayerBackPack.ChangeCountItem(PlayerBackPack.ListItems[PlayerBackPack.ItemInHand].Count) == true &&
				PlayerBackPack.PlantInHand == false)
			{
				Plant = target->GetChildByName("Plant");
				PlayerBackPack.UseItem(Plant);
			}
			else if (PlayerBackPack.PlantInHand == true && target->GetChildByName("Plant") == 0)
			{
				PlayerBackPack.PlantInHand = false;
				target->AddChild(PlayerBackPack.Plant);
				PlayerBackPack.Plant->GetComponent<Sprite>()->active = true;
				plantSlot->textureName = "";

				for (int i = 0; i < PlayerBackPack.Plant->GetChildren().size(); i++)
				{PlayerBackPack.Plant->GetChildren()[i]->GetComponent<Sprite>()->active = true;}
				PlayerBackPack.Plant->GetChildByName("UIBar")->GetChildByName("UILine")->GetComponent<Sprite>()->active = true;
			}
		}
	}
}
void PlayerController::ClickOnObjectR()
{
	if (target != nullptr) {
		Transform targetTransform = GetWorldTransform(target);
		Transform palyerTransform = GetWorldTransform(playerEntity);

		if ((palyerTransform.position - targetTransform.position).magnitude() > INTERACT_DISTANCE)
			return;

		Sprite* sprite = target->GetComponent<Sprite>();

		if (sprite) {
			Entity* Plant = target->GetChildByName("Plant");
			if (Plant != NULL && PlayerBackPack.PlantInHand == false)
			{
				PlayerBackPack.PlantInHand = true;
				PlayerBackPack.Plant = Plant;
				target->RemoveChild(Plant);
				Sprite* plantSprite = PlayerBackPack.Plant->GetComponent<Sprite>();
				plantSprite->active = false;
				plantSlot->textureName = plantSprite->textureName;

				for (int i = 0; i < PlayerBackPack.Plant->GetChildren().size(); i++)
				{PlayerBackPack.Plant->GetChildren()[i]->GetComponent<Sprite>()->active = false;}
				PlayerBackPack.Plant->GetChildByName("UIBar")->GetChildByName("UILine")->GetComponent<Sprite>()->active = false;
			}
		}




	}
}

void PlayerController::GameReset()
{
	for (int i = 0; i < PlayerBackPack.VPlants.size(); i++)
	{
		ComponentPlants* com = PlayerBackPack.VPlants[i]->GetComponent<ComponentPlants>();
		com->CheckCreate = false;
	}
	
	PlayerBackPack.ListItems[0] = ItemsBackPack(ItemsInBackPack::BLUE, 1);
	PlayerBackPack.ListItems[1] = ItemsBackPack(ItemsInBackPack::YELLOW, 1);
	PlayerBackPack.ListItems[2] = ItemsBackPack(ItemsInBackPack::RED, 1);
	PlayerBackPack.ListItems[3] = ItemsBackPack(ItemsInBackPack::PURPLE, 1);
	PlayerBackPack.ListItems[4] = ItemsBackPack(ItemsInBackPack::ORANGE, 1);
	PlayerBackPack.ListItems[5] = ItemsBackPack(ItemsInBackPack::CYAN, 1);

	PlayerBackPack.ListItems[6] = ItemsBackPack(ItemsInBackPack::WATER, 1);
	PlayerBackPack.ListItems[7] = ItemsBackPack(ItemsInBackPack::MILK, 1);
	PlayerBackPack.ListItems[8] = ItemsBackPack(ItemsInBackPack::TEA, 1);
	PlayerBackPack.ListItems[9] = ItemsBackPack(ItemsInBackPack::ESPRESSO, 1);
	PlayerBackPack.ListItems[10] = ItemsBackPack(ItemsInBackPack::AMERICANO, 1);
	PlayerBackPack.ListItems[11] = ItemsBackPack(ItemsInBackPack::LAVANDER_RAF, 1);

	PlayerBackPack.ListItems[12] = ItemsBackPack(ItemsInBackPack::UP1, 1);
	PlayerBackPack.ListItems[13] = ItemsBackPack(ItemsInBackPack::UP2, 1);
	PlayerBackPack.ListItems[14] = ItemsBackPack(ItemsInBackPack::UP3, 1);
	PlayerBackPack.ListItems[15] = ItemsBackPack(ItemsInBackPack::UP4, 1);
	PlayerBackPack.ClearHandItem();
}
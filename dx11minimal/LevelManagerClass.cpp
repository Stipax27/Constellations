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
void PlantsNear()
{
	for (int i = 0; i < PlayerBackPack.VPlants.size(); i++)
	{
		for (int j = 0; j < PlayerBackPack.VPlants.size(); j++)
		{
			if (PlayerBackPack.VPlants[i]->GetId() != PlayerBackPack.VPlants[j]->GetId())
			{
				Transform SelfT = GetWorldTransform(PlayerBackPack.VPlants[i]);
				Transform NeverT = GetWorldTransform(PlayerBackPack.VPlants[j]);

				if ((SelfT.position - NeverT.position).magnitude() < 3)
				{
					
				ComponentPlants* comSelf = PlayerBackPack.VPlants[i]->GetComponent<ComponentPlants>();
				ComponentPlants* comNever = PlayerBackPack.VPlants[j]->GetComponent<ComponentPlants>();

				comSelf->StackBuFF_TOGEC = 0;

				switch (comSelf->TypeColorPlant)
				{
					case (TypePlant::BLUE_P):
					{
						if (comNever->TypeColorPlant == TypePlant::YELLOW_P &&
							comSelf->GainPlant == Mutation::KIND_PLANT &&
							comNever->GainPlant == Mutation::KIND_PLANT
							)
						{
							comSelf->TypeColorPlant = TypePlant::CYAN_P;
							comNever->LoyaltyScale = 0;
							break;
						}


						if (comNever->TypeColorPlant == TypePlant::BLUE_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;
						else if(comNever->TypeColorPlant == TypePlant::YELLOW_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::RED_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::PURPLE_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::ORANGE_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::CYAN_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;

						break;
					}
					case (TypePlant::YELLOW_P):
					{
						if (comNever->TypeColorPlant == TypePlant::RED_P &&
							comSelf->GainPlant == Mutation::KIND_PLANT &&
							comNever->GainPlant == Mutation::KIND_PLANT
							)
						{
							comSelf->TypeColorPlant = TypePlant::ORANGE_P;
							comNever->LoyaltyScale = 0;
							break;
						}



						if (comNever->TypeColorPlant == TypePlant::BLUE_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::YELLOW_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::RED_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::PURPLE_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::ORANGE_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::CYAN_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;

						break;
					}
					case (TypePlant::RED_P):
					{
						if (comNever->TypeColorPlant == TypePlant::BLUE_P &&
							comSelf->GainPlant == Mutation::KIND_PLANT && 
							comNever->GainPlant == Mutation::KIND_PLANT
							)
						{
							comSelf->TypeColorPlant = TypePlant::PURPLE_P;
							comNever->LoyaltyScale = 0;
							break;
						}

						if (comNever->TypeColorPlant == TypePlant::BLUE_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::YELLOW_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::RED_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::PURPLE_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::ORANGE_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::CYAN_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;

						break;
					}
					case (TypePlant::PURPLE_P):
					{
						if (comNever->TypeColorPlant == TypePlant::BLUE_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::YELLOW_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::RED_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::PURPLE_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::ORANGE_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::CYAN_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;

						break;
					}
					case (TypePlant::ORANGE_P):
					{
						if (comNever->TypeColorPlant == TypePlant::BLUE_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::YELLOW_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::RED_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::PURPLE_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::ORANGE_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::CYAN_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;

						break;
					}
					case (TypePlant::CYAN_P):
					{
						if (comNever->TypeColorPlant == TypePlant::BLUE_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::YELLOW_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::RED_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::PURPLE_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::ORANGE_P)
							comSelf->StackBuFF_TOGEC += comSelf->BUFF_TOGECHER;
						else if (comNever->TypeColorPlant == TypePlant::CYAN_P)
							comSelf->StackBuFF_TOGEC -= comSelf->BUFF_TOGECHER;

						break;
					}
				}

				}
			}
		}
	}
}
void GameChangeWin()
{
	int purple = 0;
	int orange = 0;
	int cyan = 0;

	for (int i = 0; i < PlayerBackPack.VPlants.size(); i++)
	{
		ComponentPlants* comSelf = PlayerBackPack.VPlants[i]->GetComponent<ComponentPlants>();
		if (comSelf->TypeColorPlant == TypePlant::PURPLE_P && comSelf->Status == StatusPlant::GOOD)
			purple++;
		else if (comSelf->TypeColorPlant == TypePlant::ORANGE_P && comSelf->Status == StatusPlant::GOOD)
			orange++;
		else if (comSelf->TypeColorPlant == TypePlant::CYAN_P && comSelf->Status == StatusPlant::GOOD)
			cyan++;
	}

	if (purple >= 2 && orange >= 2 && cyan >= 2)
	{
		///WIN
	}

}





void GameJamMetod(ComponentPlants& PropPlant)
{
	if(PropPlant.Plant->GetParent() != 0)
	{ 
	GameChangeWin();
	PlantsNear();
	const float summBuf = PropPlant.BAF_D + PropPlant.StackBuFF_TOGEC;


	float sec = 5 + summBuf;

	if (sec < 0.1)
		sec = 0.1;

	PropPlant.LoyaltyScale -= 1. / sec;
	float Scale = ((800 * PropPlant.LoyaltyScale) / 1000.);
	float scaleS = Scale / 800;
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
	

	if (summBuf >= 0 && summBuf < 3)
		PropPlant.BuffInfo->GetComponent<Sprite>()->textureName = "BUFF_PLUS1";
	else if(summBuf >= 3)
		PropPlant.BuffInfo->GetComponent<Sprite>()->textureName = "BUFF_PLUS2";
	else if(summBuf < 0 && summBuf >= -3)
		PropPlant.BuffInfo->GetComponent<Sprite>()->textureName = "BUFF_MINUS1";
	else if (summBuf < 3)
		PropPlant.BuffInfo->GetComponent<Sprite>()->textureName = "BUFF_MINUS2";


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
	else
	{}
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

	Textures::LoadPNGTexture("BUFF_MINUS1", L"..\\dx11minimal\\Resourses\\Textures\\BUFF_MINUS1.png");
	Textures::LoadPNGTexture("BUFF_MINUS2", L"..\\dx11minimal\\Resourses\\Textures\\BUFF_MINUS2.png");
	Textures::LoadPNGTexture("BUFF_PLUS1", L"..\\dx11minimal\\Resourses\\Textures\\BUFF_PLUS1.png");
	Textures::LoadPNGTexture("BUFF_PLUS2", L"..\\dx11minimal\\Resourses\\Textures\\BUFF_PLUS2.png");
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

	Textures::LoadPNGTexture("Location", L"..\\dx11minimal\\Resourses\\Textures\\Location2.png");
	Textures::LoadPNGTexture("MENU_RESET", L"..\\dx11minimal\\Resourses\\Textures\\MENU_RESET.png");

	Textures::LoadPNGTexture("CoffeeMachin", L"..\\dx11minimal\\Resourses\\Textures\\CoffeMachin.png");



	Textures::LoadPNGTexture("Front1", L"..\\dx11minimal\\Resourses\\Textures\\char1.png");
	Textures::LoadPNGTexture("Front2", L"..\\dx11minimal\\Resourses\\Textures\\char2.png");

	Textures::LoadPNGTexture("Stop", L"..\\dx11minimal\\Resourses\\Textures\\char9.png");

	Textures::LoadPNGTexture("Back1", L"..\\dx11minimal\\Resourses\\Textures\\char3.png");
	Textures::LoadPNGTexture("Back2", L"..\\dx11minimal\\Resourses\\Textures\\char4.png");
	Textures::LoadPNGTexture("Back3", L"..\\dx11minimal\\Resourses\\Textures\\char5.png");

	Textures::LoadPNGTexture("LMOVE1", L"..\\dx11minimal\\Resourses\\Textures\\char6.png");
	Textures::LoadPNGTexture("LMOVE2", L"..\\dx11minimal\\Resourses\\Textures\\char7.png");

	Textures::LoadPNGTexture("RMOVE1", L"..\\dx11minimal\\Resourses\\Textures\\char10.png");
	Textures::LoadPNGTexture("RMOVE2", L"..\\dx11minimal\\Resourses\\Textures\\char12.png");


	Textures::LoadPNGTexture("RLStop", L"..\\dx11minimal\\Resourses\\Textures\\char8.png");







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

	entity = m_World->entityStorage->CreateEntity("Background", worldFolder);
	transform = entity->AddComponent<Transform>();
	transform->scale = point3d(30, 18, 1);
	transform->mRotation = GetMatrixFromDirection(point3d(0, 1, 0).normalized(), point3d(0, 0, 1));
	transform->position = point3d(0, -2, 0);
	Sprite* sprite = entity->AddComponent<Sprite>();
	sprite->textureName = "Location";

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


	for (int i = 0; i < PlayerBackPack.VPlants.size(); i++)
	{
		ComponentPlants* com = PlayerBackPack.VPlants[i]->GetComponent<ComponentPlants>();
		if (com->CheckCreate == true)
		{
		GameJamMetod(*com);
		}
		else
		{
			com->Plant->Destroy();
			PlayerBackPack.VPlants.erase(PlayerBackPack.VPlants.begin() + i);
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
	sphereCollider->radius = 1.0f;
	sphereCollider->collisionGroup = CollisionFilter::Group::Player;

	/////////////////////////////////////////////////

	Entity* playerSprite = m_World->entityStorage->CreateEntity("PlayerSprite", player);

	transform = playerSprite->AddComponent<Transform>();
	transform->scale = point3d(2);
	transform->position = point3d(0, 0.2, 1);
	transform->mRotation = GetMatrixFromDirection(point3d(0, 1, 0).normalized(), point3d(0, 0, 1));

	Sprite* sprite = playerSprite->AddComponent<Sprite>();
	sprite->textureName = "Stop";

	return player;
}

//Textures::LoadPNGTexture("Front1", L"..\\dx11minimal\\Resourses\\Textures\\char1.png");
//Textures::LoadPNGTexture("Front2", L"..\\dx11minimal\\Resourses\\Textures\\char2.png");
//
//Textures::LoadPNGTexture("Stop", L"..\\dx11minimal\\Resourses\\Textures\\char9.png");
//
//Textures::LoadPNGTexture("Back1", L"..\\dx11minimal\\Resourses\\Textures\\char3.png");
//Textures::LoadPNGTexture("Back2", L"..\\dx11minimal\\Resourses\\Textures\\char4.png");
//Textures::LoadPNGTexture("Back3", L"..\\dx11minimal\\Resourses\\Textures\\char5.png");
//
//Textures::LoadPNGTexture("RLMOVE1", L"..\\dx11minimal\\Resourses\\Textures\\char6.png");
//Textures::LoadPNGTexture("RLMOVE2", L"..\\dx11minimal\\Resourses\\Textures\\char7.png");
//
//Textures::LoadPNGTexture("RLStop", L"..\\dx11minimal\\Resourses\\Textures\\char8.png");




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

	entity = m_World->entityStorage->CreateEntity("PlantSlotHolder", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->position = point3d(-0.75f, -0.85f, 0.0f);
	transform2D->ratio = ScreenAspectRatio::YY;
	transform2D->scale = point3d(0.1f, 0.1f, 0.0f);
	imageLabel = entity->AddComponent<ImageLabel>();
	imageLabel->textureName = "Menu_Slot";

	entity = m_World->entityStorage->CreateEntity("PlantSlot", entity);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->ratio = ScreenAspectRatio::YY;
	transform2D->scale = point3d(0.8f, 0.8f, 0.0f);
	imageLabel = entity->AddComponent<ImageLabel>();
	imageLabel->textureName = "";

	entity = m_World->entityStorage->CreateEntity("RestartButton", uiFolder);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->ratio = ScreenAspectRatio::YY;
	transform2D->position = point3d(-0.9f, 0.85f, 0.0f);
	transform2D->scale = point3d(0.1f, 0.1f, 0.0f);
	imageLabel = entity->AddComponent<ImageLabel>();
	imageLabel->textureName = "MENU_RESET";
	button = entity->AddComponent<Button>();
	button->opacity = 0;

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

	entity = m_World->entityStorage->CreateEntity("Count", entity);
	transform2D = entity->AddComponent<Transform2D>();
	transform2D->ratio = ScreenAspectRatio::YY;
	transform2D->position = point3d(-0.8, -0.7, 0);
	textLabel = entity->AddComponent<TextLabel>();
	textLabel->textW = L"E";
	textLabel->fontFamilyW = L"Impact";
	textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
	textLabel->fontWeight = 900;
	textLabel->fontSizePx = 44;
	textLabel->fontScale = 0.9f;
	textLabel->letterSpacingPx = 1.0f;

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

			entity = m_World->entityStorage->CreateEntity("ItemName", entity);
			transform2D = entity->AddComponent<Transform2D>();
			transform2D->ratio = ScreenAspectRatio::YY;
			transform2D->position = point3d(-0.85, 1.4, 0);
			textLabel = entity->AddComponent<TextLabel>();
			textLabel->textW = L"ITEM_NAME";
			textLabel->fontFamilyW = L"Impact";
			textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
			textLabel->fontWeight = 900;
			textLabel->fontSizePx = 44;
			textLabel->fontScale = 0.3f;
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

			entity = m_World->entityStorage->CreateEntity("ItemName", entity);
			transform2D = entity->AddComponent<Transform2D>();
			transform2D->ratio = ScreenAspectRatio::YY;
			transform2D->position = point3d(-0.85, 1.4, 0);
			textLabel = entity->AddComponent<TextLabel>();
			textLabel->textW = L"ITEM_NAME";
			textLabel->fontFamilyW = L"Impact";
			textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
			textLabel->fontWeight = 900;
			textLabel->fontSizePx = 44;
			textLabel->fontScale = 0.3f;
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

			entity = m_World->entityStorage->CreateEntity("ItemName", entity);
			transform2D = entity->AddComponent<Transform2D>();
			transform2D->ratio = ScreenAspectRatio::YY;
			transform2D->position = point3d(-0.85, 1.4, 0);
			textLabel = entity->AddComponent<TextLabel>();
			textLabel->textW = L"ITEM_NAME";
			textLabel->fontFamilyW = L"Impact";
			textLabel->fontFilePathW = L"..\\dx11minimal\\Resourses\\Fonts\\Impact.ttf";
			textLabel->fontWeight = 900;
			textLabel->fontSizePx = 44;
			textLabel->fontScale = 0.3f;
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
	transform->position = point3d(20, 0, 0);
	transform->mRotation = GetMatrixFromDirection(point3d(0, 1, 0).normalized(), point3d(0, 0, 1));
	planeCollider = wall->AddComponent<PlaneCollider>();
	planeCollider->normal = point3d(-1, 0, 0);

	////////////////////////////////

	wall = m_World->entityStorage->CreateEntity("Wall", room);
	transform = wall->AddComponent<Transform>();
	transform->scale = point3d(1, 10, 1);
	transform->position = point3d(-17, 0, 0);
	transform->mRotation = GetMatrixFromDirection(point3d(0, 1, 0).normalized(), point3d(0, 0, 1));
	planeCollider = wall->AddComponent<PlaneCollider>();
	planeCollider->normal = point3d(1, 0, 0);

	////////////////////////////////

	wall = m_World->entityStorage->CreateEntity("Wall", room);
	transform = wall->AddComponent<Transform>();
	transform->scale = point3d(10, 1, 1);
	transform->position = point3d(0, 0, 2);
	transform->mRotation = GetMatrixFromDirection(point3d(0, 1, 0).normalized(), point3d(0, 0, 1));
	planeCollider = wall->AddComponent<PlaneCollider>();
	planeCollider->normal = point3d(0, 0, -1);

	////////////////////////////////

	wall = m_World->entityStorage->CreateEntity("Wall", room);
	transform = wall->AddComponent<Transform>();
	transform->scale = point3d(-10, 1, 1);
	transform->position = point3d(0, 0, -9);
	transform->mRotation = GetMatrixFromDirection(point3d(0, 1, 0).normalized(), point3d(0, 0, 1));
	planeCollider = wall->AddComponent<PlaneCollider>();
	planeCollider->normal = point3d(0, 0, 1);

	CreateGardens(room);
	CreateCoffeMachine(room);
	
}

void LevelManagerClass::CreateGardens(Entity* room)
{
	Entity* garden;
	Transform* transform;
	Sprite* sprite;
	SphereCollider* sphereCollider;

	for (int i = -13; i < 16; i += 2) {
		garden = m_World->entityStorage->CreateEntity("WallSprite", room);

		transform = garden->AddComponent<Transform>();
		transform->position = point3d(i, 0, -10);
		transform->mRotation = GetMatrixFromDirection(point3d(0, 1, 0).normalized(), point3d(0, 0, 1));

		sprite = garden->AddComponent<Sprite>();
		sprite->textureName = "garden";

		sphereCollider = garden->AddComponent<SphereCollider>();
		sphereCollider->radius = 1.25f;
	}
}

Entity* LevelManagerClass::CreateCoffeMachine(Entity* room)
{
	Entity* CoffeMachin;
	Transform* transform;
	Sprite* sprite;
	SphereCollider* sphereCollider;

		CoffeMachin = m_World->entityStorage->CreateEntity("CoffeMachin", room);

		transform = CoffeMachin->AddComponent<Transform>();
		transform->position = point3d(0, 0, 3);
		transform->mRotation = GetMatrixFromDirection(point3d(0, 1, 0).normalized(), point3d(0, 0, 1));

		sprite = CoffeMachin->AddComponent<Sprite>();
		sprite->textureName = "CoffeeMachin";

		sphereCollider = CoffeMachin->AddComponent<SphereCollider>();
		sphereCollider->radius = 1.25f;

		return CoffeMachin;
}

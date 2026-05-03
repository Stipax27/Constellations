#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "../../ECS_Base/component.h"
#include "../../UI/ImageLabel.h"


struct Sprite : Component
{
	std::string textureName = "";

	float opacity = 1.0f;
	point3d color = point3d(1, 1, 1);

	int vShader = 32;
	int pShader = 28;
};


// __ COMPONENTS GAMEJAM __ //
enum StatusPlant {
	GOOD,
	NORMAL,
	BAD,
	DEAD,
};

enum ItemsInBackPack {
	BLUE,
	YELLOW,
	RED,
	PURPLE,
	ORANGE,
	CYAN,
	WATER,
	MILK,
	TEA,
	ESPRESSO,
	AMERICANO,
	LAVANDER_RAF,
	UP1,
	UP2,
	UP3,
	UP4,
	EMPTYItem,
};

enum Mutation
{
	SEED,
	PLANT,
	KIND_PLANT,
	EVIL_PLANT,
	KILLER_PLANT,
};

enum TypePlant
{
	BLUE_P,
	YELLOW_P,
	RED_P,
	PURPLE_P,
	ORANGE_P,
	CYAN_P,
};

struct ComponentPlants : Component
{
	//base property
	float x, y, Width, Height;
	const char* Name = "object";
	int Number = 0;
	bool CheckCreate = true;
	bool Click = false;
	bool Active = true;
	std::string TextureLine;
	std::string TexturePlant;
	std::string TextureEmogy;

	const float BUFF_DRING = 1;
	const float BUFF_TOGECHER = 0.5;
	float StackBuFF_TOGEC = 0.0;

	float BAF_D = 0.0;

	//Plant property
	const char* NameChar;
	float LoyaltyScale = 1000;
	float TimeGaine = 300;
	TypePlant TypeColorPlant = TypePlant::BLUE_P;
	StatusPlant Status = StatusPlant::GOOD;
	ItemsInBackPack LoveBeverage = ItemsInBackPack::WATER;
	ItemsInBackPack HateBeverage = ItemsInBackPack::MILK;
	Mutation GainPlant = Mutation::SEED;


	Entity* Plant;
	Entity* UiLine;
	Entity* Emoji;
	Entity* Garden;
	Entity* BuffInfo;
};
// -- COMPONENTS GAMEJAM -- //


struct ItemsBackPack
{
	ItemsBackPack() {};
	ItemsBackPack(ItemsInBackPack ItemName, int Number) : Name(ItemName), Count(Number) {}

	ItemsInBackPack Name;
	int Count;
};

extern std::string itemTextures[16];

struct BackPacks
{
	ItemsInBackPack ItemInHand = ItemsInBackPack::EMPTYItem;
	bool PlantInHand = false;

	ItemsBackPack ListItems[16] =
	{
		//СЕМЕНА
		ItemsBackPack(ItemsInBackPack::BLUE,   5), // 0
		ItemsBackPack(ItemsInBackPack::YELLOW, 5), // 1
		ItemsBackPack(ItemsInBackPack::RED,    5), // 2
		ItemsBackPack(ItemsInBackPack::PURPLE, 5), // 3
		ItemsBackPack(ItemsInBackPack::ORANGE, 5), // 4
		ItemsBackPack(ItemsInBackPack::CYAN,   5), // 5

		//НАПИТКИ
		ItemsBackPack(ItemsInBackPack::WATER,           4), // 6
		ItemsBackPack(ItemsInBackPack::MILK,            1), // 7
		ItemsBackPack(ItemsInBackPack::TEA,             1), // 8
		ItemsBackPack(ItemsInBackPack::ESPRESSO,        1), // 9
		ItemsBackPack(ItemsInBackPack::AMERICANO,       1), // 10
		ItemsBackPack(ItemsInBackPack::LAVANDER_RAF,    4), // 11

		//УДОБРЕНИЯ
		ItemsBackPack(ItemsInBackPack::UP1,             5), // 12
		ItemsBackPack(ItemsInBackPack::UP2,             5), // 13
		ItemsBackPack(ItemsInBackPack::UP3,             5), // 14
		ItemsBackPack(ItemsInBackPack::UP4,             5), // 15
	};

	ItemsBackPack ItemPick;
	Entity* Plant;
	Entity* Garden;

	void ClearHandItem()
	{
		ItemInHand = ItemsInBackPack::EMPTYItem;
		ItemPick.Count = 0;
		ItemPick.Name = ItemsInBackPack::EMPTYItem;
		Plant = 0;
		Garden = 0;
		PlantInHand = false;
		EntityStorage* entityStorage = Singleton::GetInstance<EntityStorage>();
		Entity* handSlot = entityStorage->GetEntityByName("HandSlot");
		handSlot->GetComponent<ImageLabel>()->textureName = "Menu_Slot";
	}
	void AddItem(ItemsInBackPack NameItem, int PlusAddItems)
	{
		ListItems[NameItem].Count += PlusAddItems;
	}
	bool ChangeCountItem(int CountItems)
	{
		if (CountItems >= 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}



	void ProcessSound(const wchar_t* name)  // меняем тип параметра
	{
		PlaySoundW(name, NULL, SND_FILENAME | SND_ASYNC);
	}

	void MusicPlayer(const wchar_t* NameMusic)  // меняем тип параметра
	{
		std::wstring command = L"open \"";
		command += NameMusic;
		command += L"\" alias MyMusic";

		mciSendStringW(command.c_str(), NULL, 0, NULL);
		mciSendStringW(L"play MyMusic", NULL, 0, NULL);
	}

	void ResetItem()
	{
		ClearHandItem();
	}
	void UseItem(Entity* Plants)
	{
		if (ItemInHand != ItemsInBackPack::EMPTYItem)
		{
			ItemPick = ListItems[ItemInHand];
			Garden = Plants->GetParent();
			Plant = Plants;
			ComponentPlants* com = Plant->GetComponent<ComponentPlants>();
			ListItems[ItemInHand].Count--;
			switch (ItemPick.Name)
			{
			// полив ростений
			case (ItemsInBackPack::WATER):
			{
				if (com->TypeColorPlant == TypePlant::RED_P)
					com->BAF_D -= com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::YELLOW_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::BLUE_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::PURPLE_P)
					com->BAF_D -= com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::ORANGE_P)
					com->BAF_D -= com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::CYAN_P)
					com->BAF_D += com->BUFF_DRING;
				break;
			}
			case (ItemsInBackPack::MILK):
			{
				if (com->TypeColorPlant == TypePlant::RED_P)
					com->BAF_D -= com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::YELLOW_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::BLUE_P)
					com->BAF_D -= com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::PURPLE_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::ORANGE_P)
					com->BAF_D -= com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::CYAN_P)
					com->BAF_D += com->BUFF_DRING;
				break;
			}
			case (ItemsInBackPack::TEA):
			{
				if (com->TypeColorPlant == TypePlant::RED_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::YELLOW_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::BLUE_P)
					com->BAF_D -= com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::PURPLE_P)
					com->BAF_D -= com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::ORANGE_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::CYAN_P)
					com->BAF_D -= com->BUFF_DRING;
				break;
			}
			case (ItemsInBackPack::ESPRESSO):
			{
				if (com->TypeColorPlant == TypePlant::RED_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::YELLOW_P)
					com->BAF_D -= com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::BLUE_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::PURPLE_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::ORANGE_P)
					com->BAF_D -= com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::CYAN_P)
					com->BAF_D -= com->BUFF_DRING;
				break;
			}
			case (ItemsInBackPack::AMERICANO):
			{
				if (com->TypeColorPlant == TypePlant::RED_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::YELLOW_P)
					com->BAF_D -= com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::BLUE_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::PURPLE_P)
					com->BAF_D -= com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::ORANGE_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::CYAN_P)
					com->BAF_D += com->BUFF_DRING;
				break;
			}
			case (ItemsInBackPack::LAVANDER_RAF):
			{
				if (com->TypeColorPlant == TypePlant::RED_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::YELLOW_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::BLUE_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::PURPLE_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::ORANGE_P)
					com->BAF_D += com->BUFF_DRING;
				else if (com->TypeColorPlant == TypePlant::CYAN_P)
					com->BAF_D += com->BUFF_DRING;
				break;
			}
			// полив ростений

			// использование удобрения
			case (ItemsInBackPack::UP1):
			{
				com->LoyaltyScale += 100;
				if (com->LoyaltyScale > 1000)
					com->LoyaltyScale = 1000;
				break;
			}
			case (ItemsInBackPack::UP2):
			{
				com->LoyaltyScale += 200;
				if (com->LoyaltyScale > 1000)
					com->LoyaltyScale = 1000;
				break;
			}
			case (ItemsInBackPack::UP3):
			{
				com->LoyaltyScale += 300;
				if (com->LoyaltyScale > 1000)
					com->LoyaltyScale = 1000;
				break;
			}
			case (ItemsInBackPack::UP4):
			{
				com->LoyaltyScale += 500;
				if (com->LoyaltyScale > 1000)
					com->LoyaltyScale = 1000;
				break;
			}
			// использование удобрения
			}
		}
		ClearHandItem();
	}


	std::vector<Entity*> VPlants;
};


extern BackPacks PlayerBackPack;

#endif
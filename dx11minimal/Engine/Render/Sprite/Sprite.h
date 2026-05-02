#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "../../ECS_Base/component.h"


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
};
// -- COMPONENTS GAMEJAM -- //


struct ItemsBackPack
{
	ItemsBackPack() {};
	ItemsBackPack(ItemsInBackPack ItemName, int Number) : Name(ItemName), Count(Number) {}

	ItemsInBackPack Name;
	int Count;
};

struct BackPacks
{
	ItemsInBackPack ItemInHand = ItemsInBackPack::EMPTYItem;
	bool PlantInHand = false;

	ItemsBackPack ListItems[16] =
	{
		//СЕМЕНА
		ItemsBackPack(ItemsInBackPack::BLUE,   1), // 0
		ItemsBackPack(ItemsInBackPack::YELLOW, 1), // 1
		ItemsBackPack(ItemsInBackPack::RED,    1), // 2
		ItemsBackPack(ItemsInBackPack::PURPLE, 1), // 3
		ItemsBackPack(ItemsInBackPack::ORANGE, 1), // 4
		ItemsBackPack(ItemsInBackPack::CYAN,   1), // 5

		//НАПИТКИ
		ItemsBackPack(ItemsInBackPack::WATER,           1), // 6
		ItemsBackPack(ItemsInBackPack::MILK,            1), // 7
		ItemsBackPack(ItemsInBackPack::TEA,             1), // 8
		ItemsBackPack(ItemsInBackPack::ESPRESSO,        1), // 9
		ItemsBackPack(ItemsInBackPack::AMERICANO,       1), // 10
		ItemsBackPack(ItemsInBackPack::LAVANDER_RAF,    1), // 11

		//УДОБРЕНИЯ
		ItemsBackPack(ItemsInBackPack::UP1,             1), // 12
		ItemsBackPack(ItemsInBackPack::UP2,             1), // 13
		ItemsBackPack(ItemsInBackPack::UP3,             1), // 14
		ItemsBackPack(ItemsInBackPack::UP4,             1), // 15
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
				// посадка ростений
			case (ItemsInBackPack::BLUE):
			{

				break;
			}
			case (ItemsInBackPack::YELLOW):
			{

				break;
			}
			case (ItemsInBackPack::RED):
			{

				break;
			}
			case (ItemsInBackPack::PURPLE):
			{

				break;
			}
			case (ItemsInBackPack::ORANGE):
			{

				break;
			}
			case (ItemsInBackPack::CYAN):
			{

				break;
			}
			// посадка ростений

			// полив ростений
			case (ItemsInBackPack::WATER):
			{

				break;
			}
			case (ItemsInBackPack::MILK):
			{

				break;
			}
			case (ItemsInBackPack::TEA):
			{

				break;
			}
			case (ItemsInBackPack::ESPRESSO):
			{

				break;
			}
			case (ItemsInBackPack::AMERICANO):
			{


				break;
			}
			case (ItemsInBackPack::LAVANDER_RAF):
			{


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
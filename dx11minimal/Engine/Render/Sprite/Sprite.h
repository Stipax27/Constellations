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

enum Beverage {
	WATER,
	MILK,
	TEA,
	ESPRESSO,
	AMERICANO,
	LAVANDER_RAF,
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
	BLUE,
	YELLOW,
	RED,
	PURPLE,
	ORANGE,
	CYAN,
};

struct ComponentPlants : Component
{
	//base property
	float x, y, Width, Height;
	const char* Name = "object";
	int Number = 0;
	bool CheckCreate = true;
	bool Click = false;
	bool Active = false;
	std::string TextureLine;
	std::string TexturePlant;
	std::string TextureEmogy;

	//Plant property
	const char* NameChar;
	float LoyaltyScale = 1000;
	float TimeGaine = 300;
	TypePlant TypeColorPlant = TypePlant::BLUE;
	StatusPlant Status = StatusPlant::GOOD;
	Beverage LoveBeverage = Beverage::WATER;
	Beverage HateBeverage = Beverage::MILK;
	Mutation GainPlant = Mutation::SEED;

	Entity* Plant;
	Entity* UiLine;
	Entity* Emoji;
	Entity* Garden;
};
// -- COMPONENTS GAMEJAM -- //


#endif
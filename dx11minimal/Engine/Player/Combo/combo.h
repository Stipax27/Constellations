#ifndef _COMBO_STRUCT_H_
#define _COMBO_STRUCT_H_

#include "../../ECS_Base/entityStorage.h"
#include "../../ECS_Base/entity.h"

#include <vector>


#define COMBO_INPUT_WEAPON_INDEX 6


using ComboRun = void(*)(EntityStorage*, Entity*, const point3d&);

enum class ComboInputType
{
	Light,
	Heavy,
	LightHeld,
	HeavyHeld,
	Pause,
	Dash,
	TakeFists,
	TakeSword,
	TakeBow
};

struct Combo
{
	std::vector<ComboInputType> steps;
	ComboRun run;

	bool expandable;

	Combo(std::vector<ComboInputType> Steps, ComboRun Run, bool Expandable);
};

#endif
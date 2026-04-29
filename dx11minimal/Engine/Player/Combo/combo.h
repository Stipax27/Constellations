#ifndef _COMBO_STRUCT_H_
#define _COMBO_STRUCT_H_

#include "../../ECS_Base/entityStorage.h"
#include "../../ECS_Base/entity.h"

#include <vector>

using ComboRun = void(*)(EntityStorage*, Entity*, const point3d&);

enum class ComboInputType
{
	Light,
	Heavy,
	LightHeld,
	HeavyHeld,
	Pause,
	Dash,
	SwitchToFists,
	SwitchToSword,
	SwitchToBow
};

struct Combo
{
	std::vector<ComboInputType> steps;
	ComboRun run;

	bool expandable;

	Combo(std::vector<ComboInputType> Steps, ComboRun Run, bool Expandable);
};

#endif
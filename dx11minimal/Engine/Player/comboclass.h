#include <vector>

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

	Combo(std::vector<ComboInputType> Steps)
		: steps(Steps)
	{}
};
#include "combo.h"


Combo::Combo(std::vector<ComboInputType> Steps, ComboRun Run, bool Expandable)
	: steps(Steps), run(Run), expandable(Expandable)
{}
#pragma once

#include <vector>
#include "../Engine/ECS_Base/entity.h"

namespace MazeLink
{
	extern std::vector<std::pair<Entity*, Entity*>> starPairs;

	int FindStarPair(Entity* star1, Entity* star2);
}
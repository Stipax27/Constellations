#include "MazeLinks.h"

std::vector<std::pair<Entity*, Entity*>> MazeLink::starPairs;

int MazeLink::FindStarPair(Entity* star1, Entity* star2)
{
	for (int i = 0; i < MazeLink::starPairs.size(); i++) {
		if (MazeLink::starPairs[i].first == star1 && MazeLink::starPairs[i].second == star2) {
			return i;
		}
	}

	return -1;
}
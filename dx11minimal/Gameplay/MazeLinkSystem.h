#ifndef _MAZE_LINK_SYSTEM_H_
#define _MAZE_LINK_SYSTEM_H_

//////////////
// INCLUDES //
//////////////
#include "../Engine/ECS_Base/system.h"


class MazeLinkSystem : public System
{
public:
	MazeLinkSystem();
	void Initialize() override;
	void Shutdown() override;

	void Update(EntityStorage&, float) override;

private:
	std::vector<std::pair<Entity*, Entity*>> starPairs;
	std::vector<Entity*> mazeLinks;

private:
	int FindStarPair(Entity* star1, Entity* star2);
};

#endif
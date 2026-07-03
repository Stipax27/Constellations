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
	std::vector<Entity*> mazeLinks;
};

#endif
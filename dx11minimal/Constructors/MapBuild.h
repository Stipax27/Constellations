#pragma once

#include "../Engine/ECS_Base/entity.h"
#include "../Engine/BasicComponents/Transform.h"

namespace MapBuild
{
	void BuildMaze();
	void CreateRotatingStar(const Transform& onTransform, Entity* parent);
}
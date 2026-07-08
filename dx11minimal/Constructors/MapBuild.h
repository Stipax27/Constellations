#pragma once

#include "../Engine/ECS_Base/entity.h"
#include "../Engine/BasicComponents/Transform.h"

namespace MapBuild
{
	void BuildMaze();
	void CreateRotatingStar(const Transform& onTransform, float rotateSpeed, const point3d& axis = point3d(0, 1, 0), Entity* parent = nullptr);
}
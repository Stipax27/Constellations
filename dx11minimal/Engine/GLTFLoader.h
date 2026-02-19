#pragma once
#include <string>

#include "Mesh/Animation.h"
#include "Mesh/Skeleton.h"
#include "Mesh/SkinnedMesh.h"

class GLTFLoader
{
public:
	bool Load(
		const std::string& path,
		SkinnedMesh& mesh,
		Skeleton& skeleton,
		AnimationClip& animation
	);

	bool Load(
		const std::string& path,
		SkinnedMesh& mesh,
		Skeleton& skeleton,
		std::vector<AnimationClip>& animations
	);
};

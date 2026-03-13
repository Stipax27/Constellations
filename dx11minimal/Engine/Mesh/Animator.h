#pragma once
#include "Skeleton.h"
#include "Animation.h"

class Animator
{
public:
	void Update(
		Skeleton& skeleton,
		const AnimationClip& clip,
		float time
	);
};
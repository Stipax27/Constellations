#ifndef _ANIMATOR_H_
#define _ANIMATOR_H_

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

#endif
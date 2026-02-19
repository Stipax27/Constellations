#include "SkeletalAnimationComponent.h"

bool SkeletalAnimationComponent::SetAnimationByIndex(size_t index, bool restart)
{
	if (!animationClips || index >= animationClips->size())
	{
		return false;
	}

	animationClip = &(*animationClips)[index];
	if (restart)
	{
		currentTime = 0.0f;
	}

	return true;
}

bool SkeletalAnimationComponent::SetAnimationByName(const std::string& clipName, bool restart)
{
	if (!animationClips)
	{
		return false;
	}

	for (size_t i = 0; i < animationClips->size(); ++i)
	{
		if ((*animationClips)[i].name == clipName)
		{
			return SetAnimationByIndex(i, restart);
		}
	}

	return false;
}


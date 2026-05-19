#include "SkeletalAnimationComponent.h"

void SkeletalAnimationComponent::CaptureBindPose()
{
	bindLocalPose.clear();
	if (!skeleton)
	{
		return;
	}

	bindLocalPose.reserve(skeleton->joints.size());
	for (const Joint& joint : skeleton->joints)
	{
		bindLocalPose.push_back(joint.local);
	}
}

void SkeletalAnimationComponent::ResetToBindPose()
{
	if (!skeleton || bindLocalPose.size() != skeleton->joints.size())
	{
		return;
	}

	for (size_t i = 0; i < skeleton->joints.size(); ++i)
	{
		skeleton->joints[i].local = bindLocalPose[i];
	}

	skeleton->UpdateGlobalPose();
}

bool SkeletalAnimationComponent::SetAnimationByIndex(size_t index, bool restart)
{
	if (!animationClips || index >= animationClips->size())
	{
		return false;
	}

	animationClip = &(*animationClips)[index];
	if (restart)
	{
		ResetToBindPose();
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


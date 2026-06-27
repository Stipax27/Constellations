#pragma once

#include <string>
#include <vector>

#include "../../ECS_Base/component.h"
#include "../../Mesh/Animation.h"
#include "../../Mesh/Animator.h"
#include "../../Mesh/Skeleton.h"

enum class RootMotionMode
{
	InPlace,
	Accumulate
};

struct SkeletalAnimationComponent : Component
{
	Skeleton* skeleton = nullptr;
	AnimationClip* animationClip = nullptr;
	std::vector<AnimationClip>* animationClips = nullptr;
	Animator animator;
	std::vector<DirectX::XMMATRIX> bonePalette;
	std::vector<DirectX::XMFLOAT4X4> bindLocalPose;

	float currentTime = 0.0f;
	bool isPlaying = false;
	bool isLooping = true;
	RootMotionMode rootMotionMode = RootMotionMode::InPlace;

	void CaptureBindPose();
	void ResetToBindPose();
	bool SetAnimationByIndex(size_t index, bool restart = true);
	bool SetAnimationByName(const std::string& clipName, bool restart = true);
};


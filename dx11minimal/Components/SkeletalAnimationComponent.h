#pragma once

#include <string>
#include <vector>

#include "../component.h"
#include "../Engine/Mesh/Animation.h"
#include "../Engine/Mesh/Animator.h"
#include "../Engine/Mesh/Skeleton.h"

struct SkeletalAnimationComponent : Component
{
	Skeleton* skeleton = nullptr;
	AnimationClip* animationClip = nullptr;
	std::vector<AnimationClip>* animationClips = nullptr;
	Animator animator;
	std::vector<DirectX::XMMATRIX> bonePalette;

	float currentTime = 0.0f;
	bool isPlaying = false;
	bool isLooping = true;

	bool SetAnimationByIndex(size_t index, bool restart = true);
	bool SetAnimationByName(const std::string& clipName, bool restart = true);
};


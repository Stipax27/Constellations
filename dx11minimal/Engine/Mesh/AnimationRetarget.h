#pragma once

#include <vector>

#include "Animation.h"
#include "Skeleton.h"

namespace AnimationRetarget
{
	// Strict check: same joint count, same names, same hierarchy and same indices.
	bool CanUseAnimationDirectly(
		const Skeleton& sourceSkeleton,
		const Skeleton& targetSkeleton
	);

	// Build sourceJointIndex -> targetJointIndex map by joint names.
	// Returns false when skeletons are incompatible by names/hierarchy.
	bool BuildSourceToTargetJointMap(
		const Skeleton& sourceSkeleton,
		const Skeleton& targetSkeleton,
		std::vector<int>& outSourceToTargetJointMap
	);

	// Remap one clip's channel joint indices to target skeleton.
	bool RemapAnimationToSkeleton(
		const AnimationClip& sourceClip,
		const std::vector<int>& sourceToTargetJointMap,
		AnimationClip& outTargetClip
	);

	// Remap multiple clips.
	bool RemapAnimationsToSkeleton(
		const std::vector<AnimationClip>& sourceClips,
		const std::vector<int>& sourceToTargetJointMap,
		std::vector<AnimationClip>& outTargetClips
	);
}


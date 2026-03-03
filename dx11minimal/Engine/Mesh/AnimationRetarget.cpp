#include "AnimationRetarget.h"

#include <unordered_map>
#include <cctype>

namespace
{
	std::string CanonicalizeJointName(const std::string& name)
	{
		if (name.size() > 4)
		{
			const size_t dotPos = name.size() - 4;
			if (name[dotPos] == '.' &&
				std::isdigit(static_cast<unsigned char>(name[dotPos + 1])) &&
				std::isdigit(static_cast<unsigned char>(name[dotPos + 2])) &&
				std::isdigit(static_cast<unsigned char>(name[dotPos + 3])))
			{
				return name.substr(0, dotPos);
			}
		}

		return name;
	}

	bool BuildExactJointNameToIndexMap(
		const Skeleton& skeleton,
		std::unordered_map<std::string, int>& outMap)
	{
		outMap.clear();
		outMap.reserve(skeleton.joints.size());

		for (size_t i = 0; i < skeleton.joints.size(); ++i)
		{
			const std::string& jointName = skeleton.joints[i].name;
			if (jointName.empty())
			{
				return false;
			}

			const auto inserted = outMap.emplace(jointName, static_cast<int>(i));
			if (!inserted.second)
			{
				return false;
			}
		}

		return true;
	}

	bool BuildCanonicalJointNameToIndexMap(
		const Skeleton& skeleton,
		std::unordered_map<std::string, int>& outMap)
	{
		outMap.clear();
		outMap.reserve(skeleton.joints.size());

		for (size_t i = 0; i < skeleton.joints.size(); ++i)
		{
			const std::string& jointName = skeleton.joints[i].name;
			if (jointName.empty())
			{
				return false;
			}

			const std::string canonicalName = CanonicalizeJointName(jointName);
			const auto found = outMap.find(canonicalName);
			if (found == outMap.end())
			{
				outMap.emplace(canonicalName, static_cast<int>(i));
			}
			else
			{
				// Ambiguous fallback name. Mark as unusable for canonical lookup.
				found->second = -1;
			}
		}

		return true;
	}
}

bool AnimationRetarget::CanUseAnimationDirectly(
	const Skeleton& sourceSkeleton,
	const Skeleton& targetSkeleton)
{
	if (sourceSkeleton.joints.size() != targetSkeleton.joints.size())
	{
		return false;
	}

	for (size_t i = 0; i < sourceSkeleton.joints.size(); ++i)
	{
		const Joint& sourceJoint = sourceSkeleton.joints[i];
		const Joint& targetJoint = targetSkeleton.joints[i];
		if (CanonicalizeJointName(sourceJoint.name) != CanonicalizeJointName(targetJoint.name))
		{
			return false;
		}

		if (sourceJoint.parent != targetJoint.parent)
		{
			return false;
		}
	}

	return true;
}

bool AnimationRetarget::BuildSourceToTargetJointMap(
	const Skeleton& sourceSkeleton,
	const Skeleton& targetSkeleton,
	std::vector<int>& outSourceToTargetJointMap)
{
	outSourceToTargetJointMap.clear();
	outSourceToTargetJointMap.resize(sourceSkeleton.joints.size(), -1);

	if (sourceSkeleton.joints.empty() || targetSkeleton.joints.empty())
	{
		return false;
	}

	std::unordered_map<std::string, int> targetExactJointNameToIndex;
	if (!BuildExactJointNameToIndexMap(targetSkeleton, targetExactJointNameToIndex))
	{
		return false;
	}

	std::unordered_map<std::string, int> targetCanonicalJointNameToIndex;
	if (!BuildCanonicalJointNameToIndexMap(targetSkeleton, targetCanonicalJointNameToIndex))
	{
		return false;
	}

	for (size_t sourceIdx = 0; sourceIdx < sourceSkeleton.joints.size(); ++sourceIdx)
	{
		const std::string& jointName = sourceSkeleton.joints[sourceIdx].name;
		if (jointName.empty())
		{
			return false;
		}

		const auto exactFound = targetExactJointNameToIndex.find(jointName);
		if (exactFound != targetExactJointNameToIndex.end())
		{
			outSourceToTargetJointMap[sourceIdx] = exactFound->second;
			continue;
		}

		const std::string canonicalName = CanonicalizeJointName(jointName);
		const auto canonicalFound = targetCanonicalJointNameToIndex.find(canonicalName);
		if (canonicalFound == targetCanonicalJointNameToIndex.end() ||
			canonicalFound->second < 0)
		{
			return false;
		}

		outSourceToTargetJointMap[sourceIdx] = canonicalFound->second;
	}

	// Hierarchy compatibility check via mapped parents.
	for (size_t sourceIdx = 0; sourceIdx < sourceSkeleton.joints.size(); ++sourceIdx)
	{
		const int mappedTargetJoint = outSourceToTargetJointMap[sourceIdx];
		if (mappedTargetJoint < 0 ||
			static_cast<size_t>(mappedTargetJoint) >= targetSkeleton.joints.size())
		{
			return false;
		}

		const int sourceParent = sourceSkeleton.joints[sourceIdx].parent;
		const int targetParent = targetSkeleton.joints[mappedTargetJoint].parent;
		if (sourceParent < 0)
		{
			if (targetParent >= 0)
			{
				return false;
			}
			continue;
		}

		if (static_cast<size_t>(sourceParent) >= outSourceToTargetJointMap.size())
		{
			return false;
		}

		if (outSourceToTargetJointMap[sourceParent] != targetParent)
		{
			return false;
		}
	}

	return true;
}

bool AnimationRetarget::RemapAnimationToSkeleton(
	const AnimationClip& sourceClip,
	const std::vector<int>& sourceToTargetJointMap,
	AnimationClip& outTargetClip)
{
	outTargetClip = AnimationClip{};
	outTargetClip.name = sourceClip.name;
	outTargetClip.duration = sourceClip.duration;

	for (const AnimationChannel& sourceChannel : sourceClip.channels)
	{
		if (sourceChannel.joint < 0 ||
			static_cast<size_t>(sourceChannel.joint) >= sourceToTargetJointMap.size())
		{
			continue;
		}

		const int targetJointIndex = sourceToTargetJointMap[sourceChannel.joint];
		if (targetJointIndex < 0)
		{
			continue;
		}

		AnimationChannel targetChannel = sourceChannel;
		targetChannel.joint = targetJointIndex;
		outTargetClip.channels.push_back(std::move(targetChannel));
	}

	return !outTargetClip.channels.empty();
}

bool AnimationRetarget::RemapAnimationsToSkeleton(
	const std::vector<AnimationClip>& sourceClips,
	const std::vector<int>& sourceToTargetJointMap,
	std::vector<AnimationClip>& outTargetClips)
{
	outTargetClips.clear();
	outTargetClips.reserve(sourceClips.size());

	for (const AnimationClip& sourceClip : sourceClips)
	{
		AnimationClip targetClip;
		if (!RemapAnimationToSkeleton(sourceClip, sourceToTargetJointMap, targetClip))
		{
			continue;
		}

		outTargetClips.push_back(std::move(targetClip));
	}

	return !outTargetClips.empty();
}


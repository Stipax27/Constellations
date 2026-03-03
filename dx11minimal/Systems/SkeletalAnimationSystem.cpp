#include "SkeletalAnimationSystem.h"
#include "../Components/SkeletalAnimationComponent.h"
#include "../Entity.h"
#include "../Transform.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <algorithm>
#include <cmath>
#include <limits>
#include <unordered_set>
#include <utility>

using namespace DirectX;

namespace
{
	point3d ToPoint3d(FXMVECTOR v)
	{
		return point3d(
			XMVectorGetX(v),
			XMVectorGetY(v),
			XMVectorGetZ(v));
	}

	point3d ExtractTranslation(const DirectX::XMFLOAT4X4& matrix)
	{
		XMVECTOR scale;
		XMVECTOR rotation;
		XMVECTOR translation;
		if (XMMatrixDecompose(&scale, &rotation, &translation, XMLoadFloat4x4(&matrix)))
		{
			return ToPoint3d(translation);
		}

		return point3d();
	}

	void SetTranslation(DirectX::XMFLOAT4X4& matrix, const point3d& translationValue)
	{
		XMVECTOR scale = XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
		XMVECTOR rotation = XMQuaternionIdentity();
		XMVECTOR translation = XMVectorSet(
			translationValue.x,
			translationValue.y,
			translationValue.z,
			0.0f);

		XMMATRIX localMatrix = XMLoadFloat4x4(&matrix);
		XMVECTOR decomposedScale;
		XMVECTOR decomposedRotation;
		XMVECTOR decomposedTranslation;
		if (XMMatrixDecompose(&decomposedScale, &decomposedRotation, &decomposedTranslation, localMatrix))
		{
			scale = decomposedScale;
			rotation = decomposedRotation;
		}

		XMStoreFloat4x4(
			&matrix,
			XMMatrixScalingFromVector(scale) *
			XMMatrixRotationQuaternion(rotation) *
			XMMatrixTranslationFromVector(translation));
	}

	const AnimationChannel* FindTranslationChannel(const AnimationClip& clip, int jointIndex)
	{
		for (const AnimationChannel& channel : clip.channels)
		{
			if (channel.joint == jointIndex && channel.path == "translation")
			{
				return &channel;
			}
		}

		return nullptr;
	}

	int GetJointDepth(const Skeleton& skeleton, int jointIndex)
	{
		int depth = 0;
		int current = jointIndex;
		while (current >= 0 &&
			static_cast<size_t>(current) < skeleton.joints.size())
		{
			current = skeleton.joints[current].parent;
			depth++;
		}

		return depth;
	}

	int FindRootMotionJointIndex(const Skeleton& skeleton, const AnimationClip& clip)
	{
		std::unordered_set<int> translationJoints;
		for (const AnimationChannel& channel : clip.channels)
		{
			if (channel.path == "translation" && channel.joint >= 0)
			{
				translationJoints.insert(channel.joint);
			}
		}

		int bestJoint = -1;
		int bestDepth = (std::numeric_limits<int>::max)();
		for (int jointIndex : translationJoints)
		{
			if (static_cast<size_t>(jointIndex) >= skeleton.joints.size())
			{
				continue;
			}

			const int parentIndex = skeleton.joints[jointIndex].parent;
			if (parentIndex >= 0 && translationJoints.find(parentIndex) != translationJoints.end())
			{
				continue;
			}

			const int depth = GetJointDepth(skeleton, jointIndex);
			if (depth < bestDepth)
			{
				bestDepth = depth;
				bestJoint = jointIndex;
			}
		}

		if (bestJoint >= 0)
		{
			return bestJoint;
		}

		for (int jointIndex : translationJoints)
		{
			if (static_cast<size_t>(jointIndex) >= skeleton.joints.size())
			{
				continue;
			}

			const int depth = GetJointDepth(skeleton, jointIndex);
			if (depth < bestDepth)
			{
				bestDepth = depth;
				bestJoint = jointIndex;
			}
		}

		return bestJoint;
	}

	point3d SampleTranslationAtTime(const AnimationChannel& channel, float time)
	{
		if (channel.times.empty() || channel.values.empty())
		{
			return point3d();
		}

		if (time <= channel.times.front())
		{
			const DirectX::XMFLOAT4& value = channel.values.front();
			return point3d(value.x, value.y, value.z);
		}

		if (time >= channel.times.back())
		{
			const DirectX::XMFLOAT4& value = channel.values.back();
			return point3d(value.x, value.y, value.z);
		}

		size_t keyframeIdx = 0;
		for (size_t i = 0; i + 1 < channel.times.size(); ++i)
		{
			if (time < channel.times[i + 1])
			{
				keyframeIdx = i;
				break;
			}
		}

		const float t0 = channel.times[keyframeIdx];
		const float t1 = channel.times[keyframeIdx + 1];
		const DirectX::XMFLOAT4& a = channel.values[keyframeIdx];
		const DirectX::XMFLOAT4& b = channel.values[keyframeIdx + 1];
		float alpha = 0.0f;
		if (fabsf(t1 - t0) > 1e-6f)
		{
			alpha = (std::max)(0.0f, (std::min)(1.0f, (time - t0) / (t1 - t0)));
		}

		return point3d(
			a.x * (1.0f - alpha) + b.x * alpha,
			a.y * (1.0f - alpha) + b.y * alpha,
			a.z * (1.0f - alpha) + b.z * alpha);
	}

	void ApplyLocalDeltaToTransform(Transform& transform, const point3d& localDelta)
	{
		transform.position +=
			(transform.GetRightVector() * localDelta.x +
			 transform.GetUpVector() * localDelta.y +
			 transform.GetLookVector() * localDelta.z) * transform.scale;
	}
}

SkeletalAnimationSystem::SkeletalAnimationSystem(ID3D11DeviceContext* ctx, ID3D11Buffer* boneBuf)
	: context(ctx), boneBuffer(boneBuf)
{
}

void SkeletalAnimationSystem::Initialize()
{
}

void SkeletalAnimationSystem::Shutdown()
{
}

void SkeletalAnimationSystem::Update(std::vector<Entity*>& entities, float deltaTime)
{
	for (Entity* entity : entities)
	{
		if (!entity)
		{
			continue;
		}

		SkeletalAnimationComponent* animComp = entity->GetComponent<SkeletalAnimationComponent>();
		if (!animComp || !animComp->skeleton)
		{
			continue;
		}

		// Update skeleton animation if playing
		if (animComp->isPlaying && animComp->animationClip && animComp->animationClip->duration > 0.0f)
		{
			const float previousTime = animComp->currentTime;
			bool didLoop = false;

			animComp->currentTime += deltaTime;
			if (animComp->isLooping)
			{
				didLoop = animComp->currentTime >= animComp->animationClip->duration;
				animComp->currentTime = fmod(animComp->currentTime, animComp->animationClip->duration);
				if (animComp->currentTime < 0.0f)
				{
					animComp->currentTime += animComp->animationClip->duration;
				}
			}
			else if (animComp->currentTime >= animComp->animationClip->duration)
			{
				animComp->currentTime = animComp->animationClip->duration;
				animComp->isPlaying = false;
			}

			// Update skeleton with animation
			animComp->animator.Update(*animComp->skeleton, *animComp->animationClip, animComp->currentTime);

			const int rootMotionJoint = FindRootMotionJointIndex(*animComp->skeleton, *animComp->animationClip);
			const bool hasBindPose = rootMotionJoint >= 0 &&
				static_cast<size_t>(rootMotionJoint) < animComp->bindLocalPose.size() &&
				static_cast<size_t>(rootMotionJoint) < animComp->skeleton->joints.size();
			const AnimationChannel* rootTranslationChannel =
				rootMotionJoint >= 0 ? FindTranslationChannel(*animComp->animationClip, rootMotionJoint) : nullptr;

			if (hasBindPose && rootTranslationChannel)
			{
				if (animComp->rootMotionMode == RootMotionMode::Accumulate)
				{
					Transform* transform = entity->GetComponent<Transform>();
					if (transform)
					{
						point3d rootDelta = SampleTranslationAtTime(*rootTranslationChannel, animComp->currentTime)
							- SampleTranslationAtTime(*rootTranslationChannel, previousTime);

						if (didLoop && animComp->isLooping)
						{
							const point3d startTranslation = SampleTranslationAtTime(*rootTranslationChannel, 0.0f);
							const point3d endTranslation =
								SampleTranslationAtTime(*rootTranslationChannel, animComp->animationClip->duration);
							rootDelta =
								(endTranslation - SampleTranslationAtTime(*rootTranslationChannel, previousTime)) +
								(SampleTranslationAtTime(*rootTranslationChannel, animComp->currentTime) - startTranslation);
						}

						ApplyLocalDeltaToTransform(*transform, rootDelta);
					}
				}

				SetTranslation(
					animComp->skeleton->joints[rootMotionJoint].local,
					ExtractTranslation(animComp->bindLocalPose[rootMotionJoint]));
				animComp->skeleton->UpdateGlobalPose();
			}
		}

		// Build per-entity palette. Rendering uploads this palette before each draw.
		BuildBonePalette(*animComp);
	}
}


void SkeletalAnimationSystem::BuildBonePalette(SkeletalAnimationComponent& animComp)
{
	if (animComp.bonePalette.size() != 128)
	{
		animComp.bonePalette.assign(128, DirectX::XMMatrixIdentity());
	}
	else
	{
		std::fill(animComp.bonePalette.begin(), animComp.bonePalette.end(), DirectX::XMMatrixIdentity());
	}

	Skeleton& skeleton = *animComp.skeleton;

	for (size_t i = 0; i < skeleton.joints.size() && i < 128; i++)
	{
		DirectX::XMMATRIX globalPose =
			DirectX::XMLoadFloat4x4(&skeleton.joints[i].global);

		DirectX::XMMATRIX invBind =
			DirectX::XMLoadFloat4x4(&skeleton.joints[i].inverseBind);

		// Row-vector convention (mul(pos, M)):
		// Final = InverseBind * Global
		DirectX::XMMATRIX final = invBind * globalPose;

		// DirectX -> HLSL
		animComp.bonePalette[i] = DirectX::XMMatrixTranspose(final);

	}
}

#include "SkeletalAnimationSystem.h"
#include "../Components/SkeletalAnimationComponent.h"
#include "../Entity.h"
#include "../Transform.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <cmath>
#include <utility>

using namespace DirectX;

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
			animComp->currentTime += deltaTime;
			if (animComp->isLooping)
			{
				animComp->currentTime = fmod(animComp->currentTime, animComp->animationClip->duration);
			}
			else if (animComp->currentTime >= animComp->animationClip->duration)
			{
				animComp->currentTime = animComp->animationClip->duration;
				animComp->isPlaying = false;
			}

			// Update skeleton with animation
			animComp->animator.Update(*animComp->skeleton, *animComp->animationClip, animComp->currentTime);
		}

		// Build per-entity palette. Rendering uploads this palette before each draw.
		BuildBonePalette(*animComp);
	}
}


void SkeletalAnimationSystem::BuildBonePalette(SkeletalAnimationComponent& animComp)
{
	std::vector<DirectX::XMMATRIX> bones(128, DirectX::XMMatrixIdentity());
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
		bones[i] = DirectX::XMMatrixTranspose(final);

	}

	animComp.bonePalette = std::move(bones);
}

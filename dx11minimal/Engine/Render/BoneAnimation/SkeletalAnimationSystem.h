#pragma once
#include <vector>
#include "../../ECS_Base/system.h"
#include <d3d11.h>

class Entity;
struct SkeletalAnimationComponent;

class SkeletalAnimationSystem : public System
{
public:
    SkeletalAnimationSystem(ID3D11DeviceContext* ctx, ID3D11Buffer* boneBuf);
    virtual ~SkeletalAnimationSystem() = default;

    void Initialize() override;
    void Shutdown() override;
    void Update(EntityStorage& entityStorage, float deltaTime) override;

private:
	ID3D11DeviceContext* context = nullptr;
	ID3D11Buffer* boneBuffer = nullptr;

	void BuildBonePalette(SkeletalAnimationComponent& animComp);
};

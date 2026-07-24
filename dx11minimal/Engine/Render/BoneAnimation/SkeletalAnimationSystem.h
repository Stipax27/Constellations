#pragma once
#include <vector>
#include "../../ECS_Base/system.h"

class Entity;
struct SkeletalAnimationComponent;

class SkeletalAnimationSystem : public System
{
public:
	virtual ~SkeletalAnimationSystem() = default;

    void Initialize() override;
    void Shutdown() override;
    void Update(EntityStorage& entityStorage, float deltaTime) override;

private:
	void BuildBonePalette(SkeletalAnimationComponent& animComp);
};

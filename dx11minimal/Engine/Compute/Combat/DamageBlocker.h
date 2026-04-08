#ifndef _DAMAGE_BLOCKER_H_
#define _DAMAGE_BLOCKER_H_

#include <array>
#include "../../ECS_Base/component.h"
#include "damageUnit.h"

struct DamageBlocker : Component
{
    std::array<float, static_cast<size_t>(DamageType::Count)> blockFactors = { 0.5f, 0.5f, 0.0f };

    void SetBlockFactor(DamageType type, float coeff);
    float GetBlockFactor(DamageType type) const;
    float CalcDamage(const DamageUnit& damageUnit) const;
};

#endif
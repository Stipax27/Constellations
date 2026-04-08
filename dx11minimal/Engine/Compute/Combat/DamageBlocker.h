#ifndef _DAMAGE_BLOCKER_H_
#define _DAMAGE_BLOCKER_H_

#include <array>
//#include <iostream>
#include "../../ECS_Base/component.h"
#include "damageType.h"

struct DamageBlocker : Component
{
    std::array<float, static_cast<size_t>(DamageType::Count)> blockFactors = { 1.0f, 1.0f, 0.0f };

    void setBlockCoeff(DamageType type, float coeff) {
        blockFactors[static_cast<size_t>(type)] = coeff;
    }

    // Получение коэффициента
    float getBlockCoeff(DamageType type) const {
        return blockFactors[static_cast<size_t>(type)];
    }

    // Применение блокирования к урону
    float apply(float damage, DamageType type) const {
        return damage * getBlockCoeff(type);
    }
};

#endif
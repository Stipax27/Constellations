#include "DamageBlocker.h"


void DamageBlocker::SetBlockFactor(DamageType type, float coeff) {
    blockFactors[static_cast<size_t>(type)] = coeff;
}

float DamageBlocker::GetBlockFactor(DamageType type) const {
    return blockFactors[static_cast<size_t>(type)];
}

float DamageBlocker::CalcDamage(const DamageUnit& damageUnit) const {
    return damageUnit.damage * GetBlockFactor(damageUnit.damageType);
}
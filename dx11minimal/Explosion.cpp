#ifndef _EXPLOSION_C_
#define _EXPLOSION_C_

#include "component.h"

struct Explosion : Component {
    enum class Type {
        FLASH,
        CORONAL_EJECTION,
        SUN_WIND,
        DEFAULT
    };

    Type explosionType = Type::DEFAULT;
    float max_radius = 50.f;
    float radius = 0.f;
    float speed = 5.f;
    float duration = 1.0f; // Время жизни взрыва
    float startTime = 0.f; // Время создания
    int particleCount = 10; // Количество частиц
    float intensity = 1.0f; // Интенсивность эффекта
};

#endif
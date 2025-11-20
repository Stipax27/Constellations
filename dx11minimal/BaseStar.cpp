#include "BaseStar.h"

void BaseStar::Init(World* World, Entity* Entity) {
    m_World = World;
    m_entity = Entity;
    LastTime = timer::currentTime;
}
void BaseStar::LifeTimeParticl() {

    DWORD currentTimeEff = timer::currentTime;

    if (currentTimeEff - LastTime > 2000) {
        for (int i = 0; i < effect.size(); i++)
        {
            m_World->RemoveEntityByObject(effect[i]);
        }
        LastTime = currentTimeEff;
    }

}

void BaseStar::FartingEffect() {

    point3d starPos = m_entity->GetComponent<Transform>()->position;

      
    
    for (int i = 0; i < 6; i++) {
        Entity* sparkEntity = m_World->CreateEntity();
        effect.push_back(sparkEntity);
        Transform* sparkTransform = sparkEntity->AddComponent<Transform>();

        float angle = (i * 60.0f) * 3.14159f / 180.0f;
        float distance = 1.2f;

        sparkTransform->position = point3d(
            starPos.x + cos(angle) * distance,
            starPos.y + sin(angle) * distance,
            starPos.z
        );

        Explosion* sparkExplosion = sparkEntity->AddComponent<Explosion>();
        sparkExplosion->explosionType = Explosion::Type::EFFECT;
        sparkExplosion->max_radius = 1.0f;
        sparkExplosion->speed = 40.0f;
        sparkExplosion->duration = 0.15f;
        sparkExplosion->intensity = 3.5f;
        sparkExplosion->lifeTime = 1500;
    }

    
}
void BaseStar::Flash()
{
    point3d starPos = m_entity->GetComponent<Transform>()->position;

    // Основная мощная вспышка
    Entity* flashEntity = m_World->CreateEntity();
    Transform* flashTransform = flashEntity->AddComponent<Transform>();
    flashTransform->position = starPos;

    Explosion* explosion = flashEntity->AddComponent<Explosion>();
    explosion->explosionType = Explosion::Type::FLASH;
    explosion->max_radius = 8.0f; // Большой радиус
    explosion->speed = .5f;     // Очень быстрый рост
    explosion->duration = 0.3f;   // Короткая длительность
    explosion->intensity = 2.0f;  // Высокая интенсивность

    // Дополнительные быстрые вспышки
    for (int i = 0; i < 6; i++) {
        Entity* sparkEntity = m_World->CreateEntity();
        Transform* sparkTransform = sparkEntity->AddComponent<Transform>();

        float angle = (i * 60.0f) * 3.14159f / 180.0f;
        float distance = 1.2f;

        sparkTransform->position = point3d(
            starPos.x + cos(angle) * distance,
            starPos.y + sin(angle) * distance,
            starPos.z
        );

        Explosion* sparkExplosion = sparkEntity->AddComponent<Explosion>();
        sparkExplosion->explosionType = Explosion::Type::FLASH;
        sparkExplosion->max_radius = 2.0f;
        sparkExplosion->speed = 40.0f;
        sparkExplosion->duration = 0.15f;
        sparkExplosion->intensity = 1.5f;
    }
}

void BaseStar::CoronalEjection()
{
    point3d starPos = m_entity->GetComponent<Transform>()->position;

    // Фаза зарядки - интенсивные маленькие взрывы
    for (int i = 0; i < 8; i++) {
        Entity* chargeEntity = m_World->CreateEntity();
        Transform* chargeTransform = chargeEntity->AddComponent<Transform>();

        float angle = (rand() % 360) * 3.14159f / 180.0f;
        float distance = 0.8f;

        chargeTransform->position = point3d(
            starPos.x + cos(angle) * distance,
            starPos.y + sin(angle) * distance,
            starPos.z
        );

        Explosion* chargeExplosion = chargeEntity->AddComponent<Explosion>();
        chargeExplosion->explosionType = Explosion::Type::CORONAL_EJECTION;
        chargeExplosion->max_radius = 1.0f;
        chargeExplosion->speed = 0.5f;
        chargeExplosion->duration = 0.5f;
        chargeExplosion->intensity = 1.2f;
    }

    // Основной выброс плазмы - медленные, большие взрывы
    for (int wave = 0; wave < 2; wave++) {
        for (int i = 0; i < 15; i++) {
            Entity* ejectionEntity = m_World->CreateEntity();
            Transform* ejectionTransform = ejectionEntity->AddComponent<Transform>();

            float angle = (-40.0f + i * 5.0f) * 3.14159f / 180.0f;
            float distance = 1.5f + wave * 2.0f;

            ejectionTransform->position = point3d(
                starPos.x + sin(angle) * 1.2f,
                starPos.y + cos(angle) * 1.2f,
                starPos.z + distance
            );

            Explosion* ejectionExplosion = ejectionEntity->AddComponent<Explosion>();
            ejectionExplosion->explosionType = Explosion::Type::CORONAL_EJECTION;
            ejectionExplosion->max_radius = 3.0f - wave * 0.5f;
            ejectionExplosion->speed = 4.0f + wave * 1.0f;
            ejectionExplosion->duration = 1.2f;
            ejectionExplosion->intensity = 1.0f;
        }
    }
}

void BaseStar::SunWind()
{
    point3d starPos = m_entity->GetComponent<Transform>()->position;

    // Параметры торнадо
    int spirals = 6;           // Количество спиралей
    int particlesPerSpiral = 20; // Частиц на одну спираль
    float maxDistance = 20.0f; // Максимальная дистанция
    float startRadius = 0.5f;  // Начальный радиус
    float endRadius = 4.0f;    // Конечный радиус (расширяется!)

    // Создаем несколько спиралей
    for (int spiral = 0; spiral < spirals; spiral++) {
        // Смещение фазы для каждой спирали
        float spiralPhase = (float)spiral / (float)spirals * 2.0f * 3.14159f;

        for (int i = 0; i < particlesPerSpiral; i++) {
            Entity* windEntity = m_World->CreateEntity();
            Transform* windTransform = windEntity->AddComponent<Transform>();

            // Прогресс вдоль спирали (0..1)
            float progress = (float)i / (float)particlesPerSpiral;

            // Расстояние от звезды - увеличивается с прогрессом
            float distance = progress * maxDistance;

            // Радиус спирали - увеличивается с прогрессом (расширяется к концу)
            float spiralRadius = startRadius + (endRadius - startRadius) * progress;

            // Угол для спирали - несколько оборотов + фаза спирали
            float angle = progress * 8.0f * 3.14159f + spiralPhase; // 4 оборота

            // Позиция в спирали
            float xOffset = cos(angle) * spiralRadius;
            float yOffset = sin(angle) * spiralRadius;

            // Небольшое вертикальное смещение для объемности
            float verticalOffset = sin(progress * 4.0f * 3.14159f) * 0.5f;

            windTransform->position = point3d(
                starPos.x + xOffset,
                starPos.y + yOffset * 0.3f + verticalOffset, // Сжатие по Y для горизонтальности
                starPos.z + distance
            );

            Explosion* windExplosion = windEntity->AddComponent<Explosion>();
            windExplosion->explosionType = Explosion::Type::SUN_WIND;
            windExplosion->max_radius = 0.3f + progress * 0.2f; // Частицы увеличиваются к концу
            windExplosion->speed = 0.2f;
            windExplosion->duration = 1.5f - progress * 0.5f; // Более долгая жизнь у дальних частиц
            windExplosion->intensity = 0.8f;
        }
    }

    // Добавляем центральный поток - более плотные частицы по оси
    for (int i = 0; i < 40; i++) {
        Entity* coreEntity = m_World->CreateEntity();
        Transform* coreTransform = coreEntity->AddComponent<Transform>();

        float progress = (float)i / 40.0f;
        float distance = progress * maxDistance;

        // Небольшое случайное смещение от центра
        float xOffset = (rand() % 100 - 50) * 0.01f;
        float yOffset = (rand() % 100 - 50) * 0.01f;

        coreTransform->position = point3d(
            starPos.x + xOffset,
            starPos.y + yOffset,
            starPos.z + distance
        );

        Explosion* coreExplosion = coreEntity->AddComponent<Explosion>();
        coreExplosion->explosionType = Explosion::Type::SUN_WIND;
        coreExplosion->max_radius = 0.2f;
        coreExplosion->speed = 0.3f;
        coreExplosion->duration = 1.0f;
        coreExplosion->intensity = 1.0f;
    }

    // Вихрь у основания с более мелкими частицами
    for (int i = 0; i < 25; i++) {
        Entity* vortexEntity = m_World->CreateEntity();
        Transform* vortexTransform = vortexEntity->AddComponent<Transform>();

        float angle = (rand() % 360) * 3.14159f / 180.0f;
        float radius = (rand() % 100) * 0.015f;
        float distance = (rand() % 100) * 0.1f;

        vortexTransform->position = point3d(
            starPos.x + cos(angle) * radius,
            starPos.y + sin(angle) * radius,
            starPos.z + distance
        );

        Explosion* vortexExplosion = vortexEntity->AddComponent<Explosion>();
        vortexExplosion->explosionType = Explosion::Type::SUN_WIND;
        vortexExplosion->max_radius = 0.15f;
        vortexExplosion->speed = 0.4f;
        vortexExplosion->duration = 0.4f;
        vortexExplosion->intensity = 0.7f;
    }
}
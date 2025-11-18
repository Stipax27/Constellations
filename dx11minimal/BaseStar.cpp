#include "BaseStar.h"

void BaseStar::Init(World* World, Entity* Entity) {
    m_World = World;
    m_entity = Entity;
    LastTime = timer::currentTime;
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

    // Множество маленьких, быстрых взрывов для эффекта потока
    for (int i = 0; i < 60; i++) {
        Entity* windEntity = m_World->CreateEntity();
        Transform* windTransform = windEntity->AddComponent<Transform>();

        // Конусообразное распределение
        float angle = (rand() % 80 - 40) * 3.14159f / 180.0f; // ±40 градусов
        float distance = 0.5f + (rand() % 100) * 0.15f; // 0.5-15 единиц
        float spread = distance * 0.4f;

        windTransform->position = point3d(
            starPos.x + sin(angle) * spread,
            starPos.y + (rand() % 100 - 50) * 0.03f,
            starPos.z + distance
        );

        Explosion* windExplosion = windEntity->AddComponent<Explosion>();
        windExplosion->explosionType = Explosion::Type::SUN_WIND;
        windExplosion->max_radius = 0.8f;
        windExplosion->speed = 0.5f;
        windExplosion->duration = 0.4f;
        windExplosion->intensity = 0.7f;
    }

    // Более плотные частицы у основания
    for (int i = 0; i < 25; i++) {
        Entity* baseEntity = m_World->CreateEntity();
        Transform* baseTransform = baseEntity->AddComponent<Transform>();

        float angle = (rand() % 80 - 40) * 3.14159f / 180.0f;
        float spread = 0.7f;

        baseTransform->position = point3d(
            starPos.x + sin(angle) * spread,
            starPos.y + (rand() % 100 - 50) * 0.02f,
            starPos.z + 0.3f
        );

        Explosion* baseExplosion = baseEntity->AddComponent<Explosion>();
        baseExplosion->explosionType = Explosion::Type::SUN_WIND;
        baseExplosion->max_radius = 0.5f;
        baseExplosion->speed = 12.0f;
        baseExplosion->duration = 0.3f;
        baseExplosion->intensity = 0.9f;
    }
}
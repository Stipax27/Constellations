#ifndef _AI_SYSTEM_H_
#define _AI_SYSTEM_H_

#include<cmath>               // Подключаем математические функции (sqrt, sin, cos и т.д.)
#include "../../ECS_Base/system.h"           // Подключаем базовый абстрактный класс System
#include "../../BasicComponents/Transform.h"        // Подключаем компонент Transform (позиция, поворот, масштаб)
#include "../../Physic/Movement/PhysicBody.h"       // Подключаем компонент PhysicBody (скорость, физика)
#include "AIComponent.h"     // Подключаем компонент AIComponent (данные ИИ)

#include "../../Render/Star.h"             // Подключаем компонент Star (визуальное представление звезды)
#include "../../Compute/Combat/Health.h"           // Подключаем компонент Health (здоровье сущности)


// Класс AISystem, наследующий от System, реализует логику ИИ для врагов
class AISystem : public System
{
public:
    AISystem();

    void Initialize() override;
    void Shutdown() override;

    void Update(EntityStorage& entityStorage, float deltaTime) override;

private:
    void ProcessAIBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Патрулирование ----------
    void UpdatePatrolBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Преследование ----------
    void UpdateChaseBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Поиск (после потери игрока) ----------
    void UpdateSearchBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Атака ----------
    void UpdateAttackBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Бегство ----------
    void UpdateFleeBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Бездействие ----------
    void UpdateIdleBehavior(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);


    Entity* DetectTarget(EntityStorage& entityStorage, Entity* entity, Transform* transform, AIComponent* ai);

    // ---------- Вспомогательная функция: переход к патрулю вокруг последней позиции ----------
    void GoToPatrolAroundLastPosition(AIComponent* ai);
};

#endif
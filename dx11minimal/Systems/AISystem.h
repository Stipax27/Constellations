#ifndef _AI_SYSTEM_H_
#define _AI_SYSTEM_H_

#include<cmath>               // Подключаем математические функции (sqrt, sin, cos и т.д.)
#include "../system.h"           // Подключаем базовый абстрактный класс System
#include "../Transform.h"        // Подключаем компонент Transform (позиция, поворот, масштаб)
#include "../Components/AIComponent.h"     // Подключаем компонент AIComponent (данные ИИ)
#include "../PhysicBody.h"       // Подключаем компонент PhysicBody (скорость, физика)
#include "../Star.h"             // Подключаем компонент Star (визуальное представление звезды)
#include "../Health.h"           // Подключаем компонент Health (здоровье сущности)

#define AI_UPDATE_INTERVAL 0.01f  // Интервал обновления ИИ (0.01 сек = 100 раз в секунду)

// Класс AISystem, наследующий от System, реализует логику ИИ для врагов
class AISystem : public System
{
public:
    AISystem();

    void Initialize() override;
    void Shutdown() override;

    void SetPlayerEntity(Entity* player);

    void Update(vector<Entity*>& entities, float deltaTime) override;

private:
    Entity* playerEntity;
    float lastUpdateTime;

private:
    void ProcessAIBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Патрулирование ----------
    void UpdatePatrolBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Преследование ----------
    void UpdateChaseBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Поиск (после потери игрока) ----------
    void UpdateSearchBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Атака ----------
    void UpdateAttackBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Бегство ----------
    void UpdateFleeBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Бездействие ----------
    void UpdateIdleBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime);

    // ---------- Обнаружение игрока ----------
    bool DetectPlayer(Transform* transform, AIComponent* ai);

    // ---------- Вспомогательная функция: переход к патрулю вокруг последней позиции ----------
    void GoToPatrolAroundLastPosition(AIComponent* ai);
};

#endif
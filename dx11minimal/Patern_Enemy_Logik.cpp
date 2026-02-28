#ifndef _AI_SYSTEM_        // Если макрос _AI_SYSTEM_ не определён (защита от повторного включения)
#define _AI_SYSTEM_        // Определяем макрос _AI_SYSTEM_

//////////////
// INCLUDES //
//////////////
#include<cmath>            // Подключаем математические функции (например, sqrt, sin)
#include "system.h"        // Подключаем базовый класс System
#include "Transform.h"     // Подключаем компонент Transform (позиция, поворот, масштаб)
#include "Enemy_Patern.h"  // Подключаем компонент AIComponent (данные ИИ)
#include "PhysicBody.h"    // Подключаем компонент PhysicBody (скорость, физика)

/////////////
// GLOBALS //
/////////////
const float AI_UPDATE_INTERVAL = 0.01f; // Интервал обновления ИИ (0.01 сек = 100 раз в секунду)

class AISystem : public System          // Объявляем класс AISystem, наследующий от System
{
public:                                 // Открытые члены класса
    AISystem()                          // Конструктор по умолчанию
    {
        playerEntity = nullptr;          // Инициализируем указатель на игрока нулём
        lastUpdateTime = 0.0f;           // Инициализируем время последнего обновления нулём
    }

    void Initialize()                    // Метод инициализации (из базового класса)
    {
        //enemyPhysicBody;                // Закомментированная строка (вероятно, задел)
    }

    void Shutdown()                      // Метод завершения работы (из базового класса)
    {
        playerEntity = nullptr;           // Обнуляем указатель на игрока (без удаления)
    }

    // Установка цели (игрока) для врагов
    void SetPlayerEntity(Entity* player)  // Метод для передачи указателя на сущность игрока
    {
        playerEntity = player;             // Сохраняем указатель в поле класса
    }

    void Update(vector<Entity*>& entities, float deltaTime)  // Основной метод обновления (вызывается каждый кадр)
    {
        lastUpdateTime += deltaTime;      // Накопляем время с последнего вызова

        // Обновляем ИИ с определенным интервалом для оптимизации
        if (lastUpdateTime < AI_UPDATE_INTERVAL) // Если накопленное время меньше интервала
            return;                       // Пропускаем обновление (выходим)

        size_t size = entities.size();    // Получаем количество сущностей в списке
        for (int i = 0; i < size; i++)    // Цикл по всем сущностям
        {
            Entity* entity = entities[i];  // Берём текущую сущность
            if (entity->IsActive())        // Проверяем, активна ли сущность
            {
                Transform* transform = entity->GetComponent<Transform>();    // Получаем компонент Transform
                AIComponent* aiComponent = entity->GetComponent<AIComponent>(); // Получаем компонент AIComponent
                PhysicBody* physicBody = entity->GetComponent<PhysicBody>(); // Получаем компонент PhysicBody

                if (transform != nullptr && aiComponent != nullptr && physicBody != nullptr && aiComponent->enabled)
                    // Если все компоненты существуют и ИИ включён
                {
                    ProcessAIBehavior(entity, transform, aiComponent, physicBody, deltaTime);
                    // Вызываем метод обработки поведения
                }
            }
        }

        lastUpdateTime = 0.0f;            // Сбрасываем накопленное время (после обработки)
    }

private:                                 // Приватные члены класса
    Entity* playerEntity;                  // Указатель на сущность игрока
    float lastUpdateTime;                  // Накопленное время с последнего обновления

    void ProcessAIBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime) // Метод, распределяющий поведение по типам
    {
        switch (ai->behaviorType)          // Проверяем текущее состояние (тип поведения)
        {
        case AIBehaviorType::PATROL:       // Если патрулирование
            UpdatePatrolBehavior(entity, transform, ai, physicBody, deltaTime); // Вызываем метод патрулирования
            break;

        case AIBehaviorType::CHASE:        // Если преследование
            UpdateChaseBehavior(entity, transform, ai, physicBody, deltaTime); // Вызываем метод преследования
            break;

        case AIBehaviorType::ATTACK:       // Если атака
            UpdateAttackBehavior(entity, transform, ai, physicBody, deltaTime); // Вызываем метод атаки
            break;

        case AIBehaviorType::FLEE:         // Если бегство
            UpdateFleeBehavior(entity, transform, ai, physicBody, deltaTime); // Вызываем метод бегства
            break;

        case AIBehaviorType::IDLE:         // Если бездействие
            UpdateIdleBehavior(entity, transform, ai, physicBody, deltaTime); // Вызываем метод бездействия
            break;
        }

        // Обновление таймеров состояний
        ai->stateTimer += deltaTime;       // Увеличиваем таймер состояния на прошедшее время
    }

    void UpdatePatrolBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime) // Метод патрулирования
    {
        if (ai->patrolPoints.empty()) return; // Если список точек патруля пуст, ничего не делаем

        // Получаем текущую точку патрулирования
        point3d& targetPoint = ai->patrolPoints[ai->currentPatrolIndex]; // Ссылка на целевую точку

        // Движемся к точке
        point3d direction = (targetPoint - transform->position); // Вектор от текущей позиции к целевой
        float distance = direction.magnitude(); // Расстояние до цели

        //XMMATRIX additionalRotation = ... (закомментировано)
        //enemyPhysicBody->mAngVelocity = ... (закомментировано)

        if (distance < ai->arrivalDistance)   // Если расстояние меньше порога прибытия
        {
            // Достигли точки, переходим к следующей
            ai->currentPatrolIndex = (ai->currentPatrolIndex + 1) % ai->patrolPoints.size(); // Инкремент индекса (зацикленно)
            ai->stateTimer = 0.0f;             // Сбрасываем таймер состояния
        }
        else                                   // Если ещё не дошли
        {
            // Движение к точке
            direction = direction.normalized(); // Нормализуем направление
            physicBody->velocity = direction * ai->movementSpeed; // Устанавливаем скорость движения
        }
        // Проверка на обнаружение игрока
        if (playerEntity && DetectPlayer(transform, ai)) // Если игрок существует и обнаружен
        {
            ai->behaviorType = AIBehaviorType::CHASE;   // Переключаемся на преследование
            ai->stateTimer = 0.0f;                       // Сбрасываем таймер
        }
    }

    void UpdateChaseBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime) // Метод преследования
    {
        if (!playerEntity)                        // Если игрока нет
        {
            ai->behaviorType = AIBehaviorType::PATROL; // Возвращаемся к патрулированию
            return;
        }

        Transform* playerTransform = playerEntity->GetComponent<Transform>(); // Получаем компонент Transform игрока
        if (!playerTransform) return;              // Если у игрока нет трансформа, выходим

        point3d direction = playerTransform->position - transform->position; // Вектор к игроку
        float distance = direction.magnitude();    // Расстояние до игрока

        if (distance > ai->chaseRange)             // Если игрок вышел за радиус преследования
        {
            // Игрок слишком далеко, возвращаемся к патрулированию
            ai->behaviorType = AIBehaviorType::PATROL;
            ai->stateTimer = 0.0f;
        }
        else if (distance <= ai->attackRange)      // Если игрок в радиусе атаки
        {
            // Достаточно близко для атаки
            ai->behaviorType = AIBehaviorType::ATTACK;
            ai->stateTimer = 0.0f;
        }
        else                                        // Иначе продолжаем преследование
        {
            direction = direction.normalized();     // Нормализуем направление
            physicBody->velocity = direction * ai->movementSpeed; // Двигаемся к игроку
        }
    }

    void UpdateAttackBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime) // Метод атаки
    {
        if (!playerEntity)                        // Если игрока нет
        {
            ai->behaviorType = AIBehaviorType::PATROL; // Возвращаемся к патрулированию
            return;
        }

        Transform* playerTransform = playerEntity->GetComponent<Transform>(); // Трансформ игрока
        if (!playerTransform) return;

        point3d direction = playerTransform->position - transform->position; // Вектор к игроку
        float distance = direction.magnitude();    // Расстояние до игрока

        if (distance > ai->attackRange)            // Если игрок вышел из радиуса атаки
        {
            // Игрок ушел из радиуса атаки
            ai->behaviorType = AIBehaviorType::CHASE; // Возвращаемся к преследованию
            ai->stateTimer = 0.0f;
        }
        else
        {
            // Атакуем игрока
            physicBody->velocity = point3d();      // Останавливаемся для атаки (обнуляем скорость)

            // Реализация логики атаки
            if (ai->stateTimer >= ai->attackCooldown) // Если прошло достаточно времени с последней атаки
            {
                //PerformAttack(entity, transform, ai);   // Здесь должна быть реальная атака (закомментировано)
                ai->stateTimer = 0.0f;                 // Сбрасываем таймер атаки
            }
        }
    }

    void UpdateFleeBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime) // Метод бегства
    {
        if (!playerEntity)                        // Если игрока нет
        {
            ai->behaviorType = AIBehaviorType::PATROL; // Возвращаемся к патрулированию
            return;
        }

        Transform* playerTransform = playerEntity->GetComponent<Transform>(); // Трансформ игрока
        if (!playerTransform) return;

        point3d direction = transform->position - playerTransform->position; // Вектор от игрока (противоположный)
        direction = direction.normalized();      // Нормализуем

        physicBody->velocity = direction * ai->movementSpeed; // Двигаемся прочь от игрока

        // Проверяем, можно ли прекратить бегство
        if (ai->stateTimer >= ai->fleeDuration)  // Если время бегства истекло
        {
            ai->behaviorType = AIBehaviorType::PATROL; // Возвращаемся к патрулированию
            ai->stateTimer = 0.0f;
        }
    }

    void UpdateIdleBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime) // Метод бездействия
    {
        physicBody->velocity = point3d();        // Стоим на месте (обнуляем скорость)

        // Проверка на обнаружение игрока
        if (playerEntity && DetectPlayer(transform, ai)) // Если игрок обнаружен
        {
            ai->behaviorType = AIBehaviorType::CHASE;    // Переходим к преследованию
            ai->stateTimer = 0.0f;
        }

        // Через некоторое время возвращаемся к патрулированию
        if (ai->stateTimer >= ai->idleDuration)  // Если время бездействия истекло
        {
            ai->behaviorType = AIBehaviorType::PATROL; // Возвращаемся к патрулированию
            ai->stateTimer = 0.0f;
        }
    }

    bool DetectPlayer(Transform* transform, AIComponent* ai) // Метод обнаружения игрока
    {
        if (!playerEntity) return false;          // Если игрока нет, возвращаем false

        Transform* playerTransform = playerEntity->GetComponent<Transform>(); // Трансформ игрока
        if (!playerTransform) return false;       // Если у игрока нет трансформа, false

        point3d direction = playerTransform->position - transform->position; // Вектор к игроку
        float distance = direction.magnitude();   // Расстояние

        return distance <= ai->detectionRange;    // Возвращаем true, если дистанция <= радиуса обнаружения
    }
};

#endif // _AI_SYSTEM_
#ifndef _AI_SYSTEM_          // Если макрос _AI_SYSTEM_ ещё не определён (защита от повторного включения)
#define _AI_SYSTEM_          // Определяем макрос _AI_SYSTEM_

#include<cmath>               // Подключаем математические функции (sqrt, sin, cos и т.д.)
#include "system.h"           // Подключаем базовый абстрактный класс System
#include "Transform.h"        // Подключаем компонент Transform (позиция, поворот, масштаб)
#include "Enemy_Patern.h"     // Подключаем компонент AIComponent (данные ИИ)
#include "PhysicBody.h"       // Подключаем компонент PhysicBody (скорость, физика)
#include "Star.h"             // Подключаем компонент Star (визуальное представление звезды)
#include "Health.h"           // Подключаем компонент Health (здоровье сущности)

const float AI_UPDATE_INTERVAL = 0.01f;  // Интервал обновления ИИ (0.01 сек = 100 раз в секунду)

// Класс AISystem, наследующий от System, реализует логику ИИ для врагов
class AISystem : public System
{
public:
    // Конструктор: инициализирует указатель на игрока и время последнего обновления
    AISystem()
    {
        playerEntity = nullptr;   // Пока игрок не задан
        lastUpdateTime = 0.0f;    // Время с последнего обновления = 0
    }

    // Метод инициализации (пустой, но требуется базовым классом)
    void Initialize() override {}

    // Метод завершения работы: обнуляем указатель на игрока
    void Shutdown() override { playerEntity = nullptr; }

    // Устанавливает сущность игрока, за которым будут следить враги
    void SetPlayerEntity(Entity* player) { playerEntity = player; }

    // Основной метод обновления, вызываемый каждый кадр
    void Update(vector<Entity*>& entities, float deltaTime) override
    {
        lastUpdateTime += deltaTime;               // Накопляем прошедшее время
        if (lastUpdateTime < AI_UPDATE_INTERVAL)   // Если не прошло достаточно времени
            return;                                 // Пропускаем обновление (экономия ресурсов)

        // Проходим по всем сущностям
        for (Entity* entity : entities)
        {
            if (!entity->IsActive()) continue;      // Пропускаем неактивные сущности

            // Получаем необходимые компоненты
            Transform* transform = entity->GetComponent<Transform>();
            AIComponent* ai = entity->GetComponent<AIComponent>();
            PhysicBody* physicBody = entity->GetComponent<PhysicBody>();

            // Если все компоненты есть и ИИ включён – обрабатываем поведение
            if (transform && ai && physicBody && ai->enabled)
            {
                ProcessAIBehavior(entity, transform, ai, physicBody, deltaTime);
            }
        }
        lastUpdateTime = 0.0f;   // Сбрасываем накопленное время (ждём следующий интервал)
    }

private:
    Entity* playerEntity;        // Указатель на сущность игрока
    float lastUpdateTime;        // Время, прошедшее с последнего вызова обновления

    // Метод, который в зависимости от текущего состояния ИИ вызывает соответствующую функцию
    void ProcessAIBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        // Для отладки меняем цвет звезды в зависимости от состояния (необязательно)
        Star* star = entity->GetComponent<Star>();
        if (star)
        {
            switch (ai->behaviorType)
            {
            case AIBehaviorType::PATROL: star->color1 = point3d(0.2f, 0.8f, 0.2f); break; // зелёный
            case AIBehaviorType::CHASE:  star->color1 = point3d(1.0f, 0.5f, 0.0f); break; // оранжевый
            case AIBehaviorType::ATTACK: star->color1 = point3d(1.0f, 0.0f, 0.0f); break; // красный
            case AIBehaviorType::FLEE:   star->color1 = point3d(0.0f, 0.0f, 1.0f); break; // синий
            case AIBehaviorType::IDLE:   star->color1 = point3d(0.5f, 0.5f, 0.5f); break; // серый
            case AIBehaviorType::SEARCH: star->color1 = point3d(1.0f, 1.0f, 0.0f); break; // жёлтый
            }
        }

        // Выбор подходящего обработчика в зависимости от текущего состояния
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

        case AIBehaviorType::SEARCH:       // Если поиск (после потери игрока)
            UpdateSearchBehavior(entity, transform, ai, physicBody, deltaTime); // Вызываем метод поиска
            break;
        }

        ai->stateTimer += deltaTime;   // Увеличиваем таймер текущего состояния
    }

    // ---------- Патрулирование ----------
    void UpdatePatrolBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        if (ai->patrolPoints.empty()) return;   // Если нет точек патруля – ничего не делаем

        point3d& targetPoint = ai->patrolPoints[ai->currentPatrolIndex]; // Текущая целевая точка
        point3d direction = targetPoint - transform->position;            // Вектор к цели
        float distance = direction.magnitude();                           // Расстояние до цели

        if (distance < ai->arrivalDistance)   // Если почти дошли
        {
            // Переходим к следующей точке (зацикленно)
            ai->currentPatrolIndex = (ai->currentPatrolIndex + 1) % ai->patrolPoints.size();
            ai->stateTimer = 0.0f;            // Сбрасываем таймер
        }
        else
        {
            direction = direction.normalized();        // Нормализуем направлениеs
            //physicBody->velocity = direction * ai->movementSpeed; // Двигаемся к цели

            point3d targetVelocity = direction * ai->movementSpeed;
            //point3d desiredAccel = (targetVelocity - physicBody->velocity) * ai->accelerationStrength;
            point3d desiredAccel = targetVelocity * ai->accelerationStrength;
            float accelMag = desiredAccel.magnitude();
            if (accelMag > ai->maxAcceleration)
                desiredAccel = desiredAccel.normalized() * ai->maxAcceleration;
            physicBody->acceleration = desiredAccel;
        }

        // Если заметили игрока во время патруля – переключаемся на преследование
        if (playerEntity && DetectPlayer(transform, ai))
        {
            Transform* playerTransform = playerEntity->GetComponent<Transform>();
            if (playerTransform)
                ai->lastKnownPlayerPosition = playerTransform->position; // Запоминаем его позицию
            ai->hasLastKnownPosition = true;        // Отмечаем, что позиция запомнена
            ai->behaviorType = AIBehaviorType::CHASE;
            ai->stateTimer = 0.0f;
        }
    }

    // ---------- Преследование ----------
    void UpdateChaseBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        if (!playerEntity)                         // Если игрока нет – возвращаемся к патрулю
        {
            ai->behaviorType = AIBehaviorType::PATROL;
            return;
        }

        Transform* playerTransform = playerEntity->GetComponent<Transform>();
        if (!playerTransform) return;               // Если у игрока нет трансформа – выходим

        point3d direction = playerTransform->position - transform->position;
        float distance = direction.magnitude();     // Расстояние до игрока

        if (distance > ai->chaseRange)               // Если игрок слишком далеко
        {
            // Потеряли игрока – запоминаем его последнюю позицию и переходим в поиск
            ai->lastKnownPlayerPosition = playerTransform->position;
            ai->hasLastKnownPosition = true;
            ai->behaviorType = AIBehaviorType::SEARCH;
            ai->stateTimer = 0.0f;
        }
        else if (distance <= ai->attackRange)        // Если подошли на расстояние атаки
        {
            ai->behaviorType = AIBehaviorType::ATTACK;
            ai->stateTimer = 0.0f;
        }
        else                                           // Иначе продолжаем двигаться к игроку
        {
            direction = direction.normalized();
            /*physicBody->velocity = direction * ai->movementSpeed;*/
            point3d targetVelocity = direction * ai->movementSpeed;
            //point3d desiredAccel = (targetVelocity - physicBody->velocity) * ai->accelerationStrength;
            point3d desiredAccel = targetVelocity * ai->accelerationStrength;
            float accelMag = desiredAccel.magnitude();
            if (accelMag > ai->maxAcceleration)
                desiredAccel = desiredAccel.normalized() * ai->maxAcceleration;
            physicBody->acceleration = desiredAccel;
        }
    }

    // ---------- Поиск (после потери игрока) ----------
    void UpdateSearchBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        // Если нет сохранённой позиции или истекло время поиска – переходим к патрулю вокруг последней позиции
        if (!ai->hasLastKnownPosition || ai->stateTimer >= ai->searchDuration)
        {
            GoToPatrolAroundLastPosition(ai);
            return;
        }

        // Движемся к последней известной позиции игрока
        point3d direction = ai->lastKnownPlayerPosition - transform->position;
        float distance = direction.magnitude();

        if (distance < ai->arrivalDistance)          // Если достигли точки
        {
            GoToPatrolAroundLastPosition(ai);        // Начинаем патрулировать вокруг неё
            return;
        }

        direction = direction.normalized();
        //physicBody->velocity = direction * ai->movementSpeed * 2; // Двигаемся чуть быстрее (коэф. 2)
        point3d targetVelocity = direction * ai->movementSpeed;
        //point3d desiredAccel = (targetVelocity - physicBody->velocity) * ai->accelerationStrength;
        point3d desiredAccel = targetVelocity * ai->accelerationStrength;
        float accelMag = desiredAccel.magnitude();
        if (accelMag > ai->maxAcceleration)
            desiredAccel = desiredAccel.normalized() * ai->maxAcceleration;
        physicBody->acceleration = desiredAccel;

        // Во время движения проверяем, не появился ли игрок снова
        if (playerEntity && DetectPlayer(transform, ai))
        {
            Transform* playerTransform = playerEntity->GetComponent<Transform>();
            if (playerTransform)
                ai->lastKnownPlayerPosition = playerTransform->position;
            ai->behaviorType = AIBehaviorType::CHASE; // Снова преследуем
            ai->stateTimer = 0.0f;
        }
    }

    // ---------- Атака ----------
    void UpdateAttackBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        if (!playerEntity)                           // Если игрока нет – возврат к патрулю
        {
            ai->behaviorType = AIBehaviorType::PATROL;
            return;
        }

        Transform* playerTransform = playerEntity->GetComponent<Transform>();
        if (!playerTransform) return;

        float distance = (playerTransform->position - transform->position).magnitude();

        if (distance > ai->attackRange)               // Если игрок вышел из радиуса атаки
        {
            ai->behaviorType = AIBehaviorType::CHASE; // Возвращаемся к преследованию
            ai->stateTimer = 0.0f;
        }
        else
        {
            // Останавливаемся для атаки
            physicBody->velocity = point3d();

            // Проверяем, прошло ли достаточно времени для следующей атаки
            //if (ai->stateTimer >= ai->attackCooldown)
            //{
                // Получаем компонент здоровья игрока
                Health* playerHealth = playerEntity->GetComponent<Health>();
                if (playerHealth)
                {
                    playerHealth->hp -= ai->attackDamage; // Наносим урон
                    if (playerHealth->hp < 0) playerHealth->hp = 0; // Не даём уйти в минус
                }
                ai->stateTimer = 0.0f; // Сбрасываем таймер атаки
            //}
        }
    }

    // ---------- Бегство ----------
    void UpdateFleeBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        if (!playerEntity)                           // Если игрока нет – возврат к патрулю
        {
            ai->behaviorType = AIBehaviorType::PATROL;
            return;
        }

        Transform* playerTransform = playerEntity->GetComponent<Transform>();
        if (!playerTransform) return;

        point3d direction = transform->position - playerTransform->position; // Вектор от игрока
        direction = direction.normalized();
        //physicBody->velocity = direction * ai->movementSpeed; // Убегаем
        point3d targetVelocity = direction * ai->movementSpeed;
        //point3d desiredAccel = (targetVelocity - physicBody->velocity) * ai->accelerationStrength;
        point3d desiredAccel = targetVelocity * ai->accelerationStrength;
        float accelMag = desiredAccel.magnitude();
        if (accelMag > ai->maxAcceleration)
            desiredAccel = desiredAccel.normalized() * ai->maxAcceleration;
        physicBody->acceleration = desiredAccel;

        if (ai->stateTimer >= ai->fleeDuration)      // Если время бегства истекло
        {
            ai->behaviorType = AIBehaviorType::PATROL; // Возвращаемся к патрулю
            ai->stateTimer = 0.0f;
        }
    }

    // ---------- Бездействие ----------
    void UpdateIdleBehavior(Entity* entity, Transform* transform, AIComponent* ai,
        PhysicBody* physicBody, float deltaTime)
    {
        physicBody->velocity = point3d();            // Стоим на месте

        if (playerEntity && DetectPlayer(transform, ai)) // Если заметили игрока
        {
            ai->behaviorType = AIBehaviorType::CHASE;    // Начинаем преследование
            ai->stateTimer = 0.0f;
        }

        if (ai->stateTimer >= ai->idleDuration)      // Если время бездействия вышло
        {
            ai->behaviorType = AIBehaviorType::PATROL; // Переходим к патрулю
            ai->stateTimer = 0.0f;
        }
    }

    // ---------- Обнаружение игрока ----------
    bool DetectPlayer(Transform* transform, AIComponent* ai)
    {
        if (!playerEntity) return false;              // Нет игрока – не обнаружен
        Transform* playerTransform = playerEntity->GetComponent<Transform>();
        if (!playerTransform) return false;           // У игрока нет трансформа – не обнаружен
        float distance = (playerTransform->position - transform->position).magnitude();
        return distance <= ai->detectionRange;        // Обнаружен, если дистанция меньше радиуса обнаружения
    }

    // ---------- Вспомогательная функция: переход к патрулю вокруг последней позиции ----------
    void GoToPatrolAroundLastPosition(AIComponent* ai)
    {
        if (ai->hasLastKnownPosition)                  // Если есть сохранённая позиция
        {
            // Генерируем четыре точки патруля вокруг последней позиции игрока
            point3d center = ai->lastKnownPlayerPosition;
            float r = ai->searchPatrolRadius;
            ai->patrolPoints = {
                center + point3d(r, 0,  0),
                center + point3d(0, 0,  r),
                center + point3d(-r, 0,  0),
                center + point3d(0, 0, -r)
            };
            ai->currentPatrolIndex = 0;                 // Начинаем с первой точки
        }
        ai->behaviorType = AIBehaviorType::PATROL;      // Переводим в режим патруля
        ai->stateTimer = 0.0f;                          // Сбрасываем таймер
        ai->hasLastKnownPosition = false;                // Забываем сохранённую позицию
    }
};

#endif // _AI_SYSTEM_
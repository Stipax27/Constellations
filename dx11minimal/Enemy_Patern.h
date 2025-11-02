#pragma once

#include <string>

// Состояния вражеского ИИ
enum Enemy_State {
    PATROL,
    SEQUENCE,
    CHASE,
    ATTACK
};

class EnemyAI {
private:
    Enemy_State currentState;
    std::string enemyName;
    float patrolTimer;
    float attackCooldown;
    bool playerDetected;
    float playerDistance;

    // Внутренние методы
    void ExecutePatrol();
    void ExecuteSequence();
    void ExecuteChase();
    void ExecuteAttack();
    void UpdateSensors();

public:
    EnemyAI(const std::string& name);

    // Основные методы
    void Update(float deltaTime);
    void ChangeState(Enemy_State newState);
    void DetectPlayer(bool detected, float distance);

    // Геттеры
    Enemy_State GetCurrentState() const;
    std::string GetStateName() const;
    std::string GetEnemyName() const;

    // Вспомогательные методы
    void Reset();
};
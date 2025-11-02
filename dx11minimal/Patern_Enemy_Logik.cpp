#include "Enemy_Patern.h"
#include <iostream>
#include <cmath>

EnemyAI::EnemyAI(const std::string& name)
    : enemyName(name),
    currentState(PATROL),
    patrolTimer(0.0f),
    attackCooldown(0.0f),
    playerDetected(false),
    playerDistance(0.0f) {
    std::cout << "Создан вражеский ИИ: " << enemyName << std::endl;
}

void EnemyAI::Update(float deltaTime) {
    // Обновляем сенсоры и таймеры
    UpdateSensors();

    if (attackCooldown > 0.0f) {
        attackCooldown -= deltaTime;
    }

    // Основная логика поведения через switch
    switch (currentState) {
    case PATROL:
        ExecutePatrol();
        break;
    case SEQUENCE:
        ExecuteSequence();
        break;
    case CHASE:
        ExecuteChase();
        break;
    case ATTACK:
        ExecuteAttack();
        break;
    }
}

void EnemyAI::ExecutePatrol() {
    std::cout << enemyName << ": Патрулирую сектор..." << std::endl;
    patrolTimer += 0.1f;

    // Логика перехода между состояниями
    if (playerDetected) {
        if (playerDistance < 50.0f) {
            ChangeState(ATTACK);
        }
        else {
            ChangeState(CHASE);
        }
    }
    else if (patrolTimer > 5.0f) {
        ChangeState(SEQUENCE);
        patrolTimer = 0.0f;
    }
}

void EnemyAI::ExecuteSequence() {
    static int sequenceStep = 0;

    switch (sequenceStep) {
    case 0:
        std::cout << enemyName << ": Сканирую пространство на аномалии..." << std::endl;
        sequenceStep++;
        break;
    case 1:
        std::cout << enemyName << ": Проверяю системы навигации..." << std::endl;
        sequenceStep++;
        break;
    case 2:
        std::cout << enemyName << ": Калибрую орудия..." << std::endl;
        sequenceStep = 0; // Сброс последовательности
        ChangeState(PATROL);
        break;
    }

    // Приоритет: игрок
    if (playerDetected) {
        sequenceStep = 0;
        if (playerDistance < 50.0f) {
            ChangeState(ATTACK);
        }
        else {
            ChangeState(CHASE);
        }
    }
}

void EnemyAI::ExecuteChase() {
    std::cout << enemyName << ": Преследую цель! Дистанция: " << playerDistance << std::endl;

    // Логика преследования
    if (!playerDetected) {
        std::cout << enemyName << ": Потерял цель! Возвращаюсь к патрулированию." << std::endl;
        ChangeState(PATROL);
    }
    else if (playerDistance < 30.0f) {
        ChangeState(ATTACK);
    }
    else if (playerDistance > 100.0f) {
        std::cout << enemyName << ": Цель слишком далеко. Отступаю." << std::endl;
        ChangeState(PATROL);
    }
}

void EnemyAI::ExecuteAttack() {
    if (attackCooldown <= 0.0f) {
        std::cout << enemyName << ": АТАКА! Лазерный залп по цели!" << std::endl;
        attackCooldown = 2.0f; // 2 секунды перезарядки
    }
    else {
        std::cout << enemyName << ": Прицеливаюсь... " << attackCooldown << "с до выстрела" << std::endl;
    }

    // Логика атаки
    if (!playerDetected) {
        std::cout << enemyName << ": Цель исчезла! Поиск..." << std::endl;
        ChangeState(SEQUENCE);
    }
    else if (playerDistance > 60.0f) {
        std::cout << enemyName << ": Цель уходит! Продолжаю преследование." << std::endl;
        ChangeState(CHASE);
    }
}

void EnemyAI::UpdateSensors() {
    // Здесь будет логика обнаружения игрока
    // Пока просто эмулируем работу сенсоров
    if (playerDetected) {
        playerDistance -= 1.0f; // Эмуляция движения
        if (playerDistance < 0.0f) playerDistance = 0.0f;
    }
}

void EnemyAI::ChangeState(Enemy_State newState) {
    if (currentState != newState) {
        std::cout << enemyName << ": Смена состояния ["
            << GetStateName() << " -> "
            << GetStateName(newState) << "]" << std::endl;
        currentState = newState;
    }
}

void EnemyAI::DetectPlayer(bool detected, float distance) {
    playerDetected = detected;
    playerDistance = distance;

    if (detected) {
        std::cout << enemyName << ": Обнаружена цель! Дистанция: " << distance << std::endl;
    }
}

// Геттеры
Enemy_State EnemyAI::GetCurrentState() const {
    return currentState;
}

std::string EnemyAI::GetStateName() const {
    return GetStateName(currentState);
}

std::string EnemyAI::GetStateName(Enemy_State state) const {
    switch (state) {
    case PATROL: return "ПАТРУЛИРОВАНИЕ";
    case SEQUENCE: return "ПОСЛЕДОВАТЕЛЬНОСТЬ";
    case CHASE: return "ПРЕСЛЕДОВАНИЕ";
    case ATTACK: return "АТАКА";
    default: return "НЕИЗВЕСТНО";
    }
}

std::string EnemyAI::GetEnemyName() const {
    return enemyName;
}

void EnemyAI::Reset() {
    currentState = PATROL;
    patrolTimer = 0.0f;
    attackCooldown = 0.0f;
    playerDetected = false;
    playerDistance = 0.0f;
    std::cout << enemyName << ": Сброс состояния" << std::endl;
}
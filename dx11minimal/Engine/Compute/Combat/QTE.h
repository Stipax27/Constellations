#ifndef _QTE_H_
#define _QTE_H_

#include "../../ECS_Base/component.h"
#include "Health.h"
#include "vector"
#include "functional"

// Типы QTE
enum class QTEType {
    BUTTON_MASHING,     // Быстрое нажатие кнопки
    TIMED_PRESS,        // Нажать в нужный момент
    SEQUENCE,           // Последовательность кнопок
    HOLD,               // Удержание кнопки
    RHYTHM,             // Ритмичное нажатие
    JOYSTICK_DIRECTION  // Движение стиком
};

// Результат одной кнопки/действия
struct QTEStepResult {
    bool success;
    float reactionTime;    // За сколько миллисекунд отреагировал игрок
    int inputValue;        // Какая кнопка/значение было нажато
};

struct QTE : Component {
    // === Состояние (уже есть) ===
    bool isStartQTE;
    bool isEndQTE;
    bool isSuccess;              // Успешно ли завершилось QTE
    bool isFailed;               // Провалено ли QTE

    // === Временные параметры ===
    float durationQTE;           // Общая длительность 
    float currentTime;           // Текущее время выполнения
    float timePerStep;           // Время на один шаг (для последовательностей)
    float reactionWindowStart;   // Окно реакции: начало (в секундах от старта)
    float reactionWindowEnd;     // Окно реакции: конец
    float forgivenessTime;       // Лёгкий режим: насколько рано/поздно можно нажать (сек)
    float currentStep;           // Текущий шаг QTE 

    // === Игровые параметры ===
    QTEType type;                // Тип QTE
    std::vector<int> requiredSequence;  // Последовательность кнопок (коды клавиш)
    int requiredButton;          // Одна кнопка для простых QTE
    int requiredPressCount;      // Сколько раз нужно нажать (для машинга)
    int currentPressCount;       // Текущее количество нажатий

    // === Визуальные и аудио параметры ===
    int uiPromptId;              // ID спрайта/UI элемента с подсказкой
    int successAnimationId;      // ID анимации при успехе
    int failAnimationId;         // ID анимации при провале
    std::string successSound;    // Звук успеха
    std::string failSound;       // Звук провала

    // === Награды и наказания ===
    int rewardDamage;            // Урон врагу при успехе
    int penaltyDamage;           // Урон герою при провале
    float rewardTimeSlow;        // Замедление времени при успехе (0.5 = половинная скорость)
    int rewardMana;              // Восстановление маны/энергии
    Health* targetHealth;        // Чьё здоровье меняем (уже есть Health.h)

    // === Обработчики (callbacks) ===
    std::function<void(QTEStepResult)> onStepComplete;   // Шаг завершён
    std::function<void(bool)> onComplete;                // QTE завершено (успех/провал)
    std::function<void()> onPerfect;                     // Идеальное выполнение (бонус)

    // === Прогрессия сложности ===
    float difficultyMultiplier;  // Множитель сложности (1.0 = нормально)
    int comboCounter;            // Счётчик успешных QTE подряд

    // === Дополнительные флаги ===
    bool isPaused;               // Поставлено на паузу
    bool showDebugInfo;          // Показывать отладочную информацию
    bool allowRetry;             // Можно ли перепройти при провале
    bool consumeInput;           // Поглощать ли ввод во время QTE (блокировать движение героя)
};

#endif
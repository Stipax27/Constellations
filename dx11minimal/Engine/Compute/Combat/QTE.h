#ifndef _QTE_H_
#define _QTE_H_

enum class QTEType {
    BUTTON_MASH,      // Быстро нажимать кнопку
    TIMED_PRESS,      // Нажать в нужный момент
    SEQUENCE,         // Последовательность кнопок
    HOLD             // Удерживать кнопку
};

struct QTESimple {
    QTEType type = QTEType::TIMED_PRESS;

    // Настройки
    int requiredKey = 0;                    // Клавиша для нажатия (VK_SPACE и т.д.)
    std::vector<int> keySequence;           // Для SEQUENCE
    int requiredPresses = 0;               // Для BUTTON_MASH
    float duration = 3.0f;                  // Время на выполнение
    float perfectWindowStart = 0.3f;        // Окно идеального нажатия (0.0-1.0)
    float perfectWindowEnd = 0.7f;

    // Состояние
    bool isActive = false;
    bool isCompleted = false;
    bool isSuccess = false;
    bool isPerfect = false;
    float currentTime = 0.0f;
    int currentPresses = 0;
    int currentSequenceStep = 0;

    // Коллбэки
    std::function<void(bool, bool)> onComplete;  // (success, isPerfect)
};
#endif 
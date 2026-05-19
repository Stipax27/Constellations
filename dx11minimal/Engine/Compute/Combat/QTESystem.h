#ifndef _QTE_SYSTEM_H_
#define _QTE_SYSTEM_H_

#include <vector>
#include <functional>
#include "../../ECS_Base/system.h"
#include "../../UI/uiSystem.h"
#include "../../Compute/Combat/QTE.h"


class QTESystem : public System {
public:
    QTESystem();
    ~QTESystem();

    void Initialize();
    void Shutdown();
    void Update(EntityStorage& entityStorage, float deltaTime);

    // API для запуска QTE
    void StartQTE(Entity* entity);
    void StopQTE(Entity* entity);
    bool IsQTEActive() const { return currentQTE != nullptr; }

    // Обработка ввода (вызывается из InputSystem)
    void OnKeyPressed(int keyCode);

private:
    // Внутренние методы
    void UpdateTiming(float deltaTime);
    void CompleteQTE(bool success);
    void ProcessButtonMash(int keyCode);
    void ProcessTimedPress(int keyCode);
    void ProcessSequence(int keyCode);
    void ProcessHold(int keyCode);
    void ProcessRhythm(int keyCode);
    void ProcessJoystickDirection(int keyCode);
    bool IsInPerfectWindow() const;
    void ApplyRewardsAndPenalties(bool success);

    // Текущий активный QTE
    Entity* currentQTE = nullptr;
    QTE* currentQTEComp = nullptr;

    // Runtime состояние
    float currentTime = 0.0f;
    float buttonMashResetTimer = 0.0f;

    // Константы
    static constexpr float BUTTON_MASH_RESET_TIME = 0.5f;
};

#endif
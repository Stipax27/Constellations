#ifndef _QTE_SYSTEM_H_
#define _QTE_SYSTEM_H_

#include <vector>
#include <functional>
#include "QTE.h"

// Простой UI интерфейс (без привязки к конкретной системе)
class IQTEUI {
public:
    virtual ~IQTEUI() = default;
    virtual void ShowMashUI(int required, float duration) = 0;
    virtual void ShowTimedUI(float duration, float perfectStart, float perfectEnd) = 0;
    virtual void ShowSequenceUI(const std::vector<int>& keys, float duration) = 0;
    virtual void ShowHoldUI(float duration) = 0;
    virtual void UpdateProgress(float progress, bool isPerfect) = 0;
    virtual void UpdateMashCount(int current, int required) = 0;
    virtual void UpdateSequenceStep(int current, int total) = 0;
    virtual void HideUI() = 0;
    virtual void SetPromptKey(int keyCode) = 0;
};

class QTESimpleSystem {
public:
    QTESimpleSystem();
    ~QTESimpleSystem();

    // Установка UI интерфейса
    void SetUI(IQTEUI* uiInterface) { ui = uiInterface; }

    // Запуск QTE
    void StartQTE(QTESimple& qte);
    void StopQTE();
    bool IsActive() const { return activeQTE != nullptr; }

    // Обновление каждый кадр
    void Update(float deltaTime);

    // Обработка ввода
    void OnKeyPressed(int keyCode);
    void OnKeyReleased(int keyCode);  // Для HOLD

private:
    void Complete(bool success, bool isPerfect = false);
    void ProcessButtonMash(int keyCode);
    void ProcessTimedPress(int keyCode);
    void ProcessSequence(int keyCode);
    void ProcessHoldPress(int keyCode);
    void ProcessHoldRelease(int keyCode);
    bool IsInPerfectWindow() const;

    IQTEUI* ui = nullptr;
    QTESimple* activeQTE = nullptr;
    bool isKeyHeld = false;  // Для HOLD режима
};

#endif 
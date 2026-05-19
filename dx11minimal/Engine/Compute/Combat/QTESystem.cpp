#include "QTESystem.h"
#include "../../Compute/Entity.h"

QTESystem::QTESystem() {}

QTESystem::~QTESystem() {}

void QTESystem::Initialize() {
    currentQTE = nullptr;
    currentQTEComp = nullptr;
    currentTime = 0.0f;
    buttonMashResetTimer = 0.0f;
}

void QTESystem::Shutdown() {
    if (currentQTE) {
        StopQTE(currentQTE);
    }
}

void QTESystem::Update(EntityStorage& entityStorage, float deltaTime) {
    (void)entityStorage; // Не используется, но оставлено для совместимости с System

    if (!currentQTE || !currentQTEComp) {
        return;
    }

    if (!currentQTEComp->isStartQTE || currentQTEComp->isEndQTE) {
        return;
    }

    UpdateTiming(deltaTime);

    // Обновляем UI прогресс
    float progress = currentTime / currentQTEComp->durationQTE;
    bool isPerfect = IsInPerfectWindow();

    switch (currentQTEComp->type) {
    case QTEType::BUTTON_MASHING: {
        float mashProgress = (float)currentQTEComp->currentPressCount / currentQTEComp->requiredPressCount;
        UISystem::GetInstance().UpdateQTEMash(mashProgress);
        break;
    }
    case QTEType::TIMED_PRESS:
        UISystem::GetInstance().UpdateQTETimer(progress, isPerfect);
        break;
    case QTEType::SEQUENCE: {
        float seqProgress = (float)currentQTEComp->currentStep / currentQTEComp->requiredSequence.size();
        UISystem::GetInstance().UpdateQTESequence(seqProgress, currentQTEComp->currentStep);
        break;
    }
    case QTEType::HOLD:
        UISystem::GetInstance().UpdateQTEHold(progress);
        break;
    case QTEType::RHYTHM:
        UISystem::GetInstance().UpdateQTERhythm(currentTime, currentQTEComp->durationQTE);
        break;
    case QTEType::JOYSTICK_DIRECTION:
        UISystem::GetInstance().UpdateQTEJoystick(currentTime, currentQTEComp->durationQTE);
        break;
    }

    // Для машинга - автоматический провал если долго не нажимал
    if (currentQTEComp->type == QTEType::BUTTON_MASHING) {
        if (buttonMashResetTimer > 0.0f) {
            buttonMashResetTimer -= deltaTime;
            if (buttonMashResetTimer <= 0.0f && currentQTEComp->currentPressCount < currentQTEComp->requiredPressCount) {
                CompleteQTE(false);
            }
        }
    }
}

void QTESystem::UpdateTiming(float deltaTime) {
    currentTime += deltaTime;
    currentQTEComp->currentTime = currentTime;

    if (currentTime >= currentQTEComp->durationQTE) {
        CompleteQTE(false);
    }
}

void QTESystem::StartQTE(Entity* entity) {
    if (!entity) return;

    if (currentQTE) {
        StopQTE(currentQTE);
    }

    currentQTE = entity;
    currentQTEComp = entity->GetComponent<QTE>();

    if (!currentQTEComp) {
        currentQTE = nullptr;
        return;
    }

    // Сброс состояния
    currentQTEComp->isStartQTE = true;
    currentQTEComp->isEndQTE = false;
    currentQTEComp->isSuccess = false;
    currentQTEComp->isFailed = false;
    currentQTEComp->currentStep = 0;
    currentQTEComp->currentPressCount = 0;
    currentQTEComp->currentTime = 0.0f;

    currentTime = 0.0f;
    buttonMashResetTimer = BUTTON_MASH_RESET_TIME;

    // Показываем UI в зависимости от типа QTE
    switch (currentQTEComp->type) {
    case QTEType::BUTTON_MASHING:
        UISystem::GetInstance().ShowQTEMash(currentQTEComp->requiredPressCount, currentQTEComp->durationQTE);
        if (currentQTEComp->requiredButton != 0) {
            UISystem::GetInstance().SetQTEPromptKey(currentQTEComp->requiredButton);
        }
        break;
    case QTEType::TIMED_PRESS:
        UISystem::GetInstance().ShowQTETimed(currentQTEComp->durationQTE,
            currentQTEComp->reactionWindowStart, currentQTEComp->reactionWindowEnd);
        if (currentQTEComp->requiredButton != 0) {
            UISystem::GetInstance().SetQTEPromptKey(currentQTEComp->requiredButton);
        }
        break;
    case QTEType::SEQUENCE:
        UISystem::GetInstance().ShowQTESequence(currentQTEComp->requiredSequence, currentQTEComp->durationQTE);
        break;
    case QTEType::HOLD:
        UISystem::GetInstance().ShowQTEHold(currentQTEComp->durationQTE);
        if (currentQTEComp->requiredButton != 0) {
            UISystem::GetInstance().SetQTEPromptKey(currentQTEComp->requiredButton);
        }
        break;
    case QTEType::RHYTHM:
        UISystem::GetInstance().ShowQTERhythm(currentQTEComp->durationQTE);
        break;
    case QTEType::JOYSTICK_DIRECTION:
        UISystem::GetInstance().ShowQTEJoystick(currentQTEComp->durationQTE);
        break;
    }
}

void QTESystem::StopQTE(Entity* entity) {
    if (currentQTE == entity) {
        UISystem::GetInstance().HideQTE();

        if (currentQTEComp) {
            currentQTEComp->isStartQTE = false;
            currentQTEComp->isEndQTE = true;
        }

        currentQTE = nullptr;
        currentQTEComp = nullptr;
    }
}

void QTESystem::OnKeyPressed(int keyCode) {
    if (!currentQTE || !currentQTEComp || !currentQTEComp->isStartQTE) {
        return;
    }

    if (currentQTEComp->isEndQTE) {
        return;
    }

    switch (currentQTEComp->type) {
    case QTEType::BUTTON_MASHING:
        ProcessButtonMash(keyCode);
        break;
    case QTEType::TIMED_PRESS:
        ProcessTimedPress(keyCode);
        break;
    case QTEType::SEQUENCE:
        ProcessSequence(keyCode);
        break;
    case QTEType::HOLD:
        ProcessHold(keyCode);
        break;
    case QTEType::RHYTHM:
        ProcessRhythm(keyCode);
        break;
    case QTEType::JOYSTICK_DIRECTION:
        ProcessJoystickDirection(keyCode);
        break;
    }
}

void QTESystem::ProcessButtonMash(int keyCode) {
    bool isValidKey = false;

    if (currentQTEComp->requiredButton != 0) {
        isValidKey = (keyCode == currentQTEComp->requiredButton);
    }
    else if (currentQTEComp->requiredSequence.empty()) {
        isValidKey = (keyCode == VK_SPACE) || (keyCode >= 'A' && keyCode <= 'Z');
    }
    else {
        for (int expected : currentQTEComp->requiredSequence) {
            if (keyCode == expected) {
                isValidKey = true;
                break;
            }
        }
    }

    if (isValidKey) {
        currentQTEComp->currentPressCount++;
        buttonMashResetTimer = BUTTON_MASH_RESET_TIME;

        float mashProgress = (float)currentQTEComp->currentPressCount / currentQTEComp->requiredPressCount;
        UISystem::GetInstance().UpdateQTEMash(mashProgress);

        if (currentQTEComp->currentPressCount >= currentQTEComp->requiredPressCount) {
            CompleteQTE(true);
        }
    }
    else {
        CompleteQTE(false);
    }
}

void QTESystem::ProcessTimedPress(int keyCode) {
    int expectedKey = (currentQTEComp->requiredButton != 0) ?
        currentQTEComp->requiredButton :
        (currentQTEComp->requiredSequence.empty() ? VK_SPACE : currentQTEComp->requiredSequence[0]);

    if (keyCode == expectedKey) {
        bool isPerfect = IsInPerfectWindow();
        currentQTEComp->isPerfect = isPerfect;

        if (isPerfect && currentQTEComp->onPerfect) {
            currentQTEComp->onPerfect();
        }

        CompleteQTE(true);
    }
    else {
        CompleteQTE(false);
    }
}

void QTESystem::ProcessSequence(int keyCode) {
    if (currentQTEComp->currentStep >= (int)currentQTEComp->requiredSequence.size()) {
        return;
    }

    int expectedKey = currentQTEComp->requiredSequence[currentQTEComp->currentStep];

    if (keyCode == expectedKey) {
        QTEStepResult stepResult;
        stepResult.success = true;
        stepResult.reactionTime = currentTime;
        stepResult.inputValue = keyCode;

        if (currentQTEComp->onStepComplete) {
            currentQTEComp->onStepComplete(stepResult);
        }

        currentQTEComp->currentStep++;

        if (currentQTEComp->currentStep < (int)currentQTEComp->requiredSequence.size()) {
            float progress = (float)currentQTEComp->currentStep / currentQTEComp->requiredSequence.size();
            UISystem::GetInstance().UpdateQTESequence(progress, currentQTEComp->currentStep);
        }

        if (currentQTEComp->currentStep >= (int)currentQTEComp->requiredSequence.size()) {
            CompleteQTE(true);
        }
    }
    else {
        QTEStepResult stepResult;
        stepResult.success = false;
        stepResult.reactionTime = currentTime;
        stepResult.inputValue = keyCode;

        if (currentQTEComp->onStepComplete) {
            currentQTEComp->onStepComplete(stepResult);
        }

        CompleteQTE(false);
    }
}

void QTESystem::ProcessHold(int keyCode) {
    int expectedKey = (currentQTEComp->requiredButton != 0) ?
        currentQTEComp->requiredButton :
        (currentQTEComp->requiredSequence.empty() ? VK_SPACE : currentQTEComp->requiredSequence[0]);

    if (keyCode == expectedKey) {
        // Удержание - успех если додержали до конца таймера
        // Проверка будет в Update по таймеру
    }
    else if (keyCode != expectedKey && currentTime < currentQTEComp->durationQTE) {
        CompleteQTE(false);
    }
}

void QTESystem::ProcessRhythm(int keyCode) {
    int expectedKey = (currentQTEComp->requiredButton != 0) ?
        currentQTEComp->requiredButton :
        (currentQTEComp->requiredSequence.empty() ? VK_SPACE : currentQTEComp->requiredSequence[0]);

    if (keyCode == expectedKey) {
        CompleteQTE(true);
    }
    else {
        CompleteQTE(false);
    }
}

void QTESystem::ProcessJoystickDirection(int keyCode) {
    for (int expected : currentQTEComp->requiredSequence) {
        if (keyCode == expected) {
            CompleteQTE(true);
            return;
        }
    }
    CompleteQTE(false);
}

bool QTESystem::IsInPerfectWindow() const {
    float normalizedTime = currentTime / currentQTEComp->durationQTE;
    return (normalizedTime >= currentQTEComp->reactionWindowStart &&
        normalizedTime <= currentQTEComp->reactionWindowEnd);
}

void QTESystem::ApplyRewardsAndPenalties(bool success) {
    if (!currentQTEComp) return;

    if (success) {
        if (currentQTEComp->rewardDamage > 0 && currentQTEComp->targetHealth) {
            currentQTEComp->targetHealth->hp -= currentQTEComp->rewardDamage;
        }

        if (currentQTEComp->onComplete) {
            currentQTEComp->onComplete(true);
        }
    }
    else {
        if (currentQTEComp->penaltyDamage > 0) {
            // Наносим урон игроку - нужно получить компонент Health игрока
            // Для простоты пока оставим, потом добавите
        }

        if (currentQTEComp->onComplete) {
            currentQTEComp->onComplete(false);
        }
    }
}

void QTESystem::CompleteQTE(bool success) {
    if (!currentQTE || !currentQTEComp || !currentQTEComp->isStartQTE) {
        return;
    }

    currentQTEComp->isStartQTE = false;
    currentQTEComp->isEndQTE = true;

    if (success) {
        currentQTEComp->isSuccess = true;
        currentQTEComp->isFailed = false;
    }
    else {
        currentQTEComp->isSuccess = false;
        currentQTEComp->isFailed = true;
    }

    ApplyRewardsAndPenalties(success);
    UISystem::GetInstance().HideQTE();

    currentQTE = nullptr;
    currentQTEComp = nullptr;
}
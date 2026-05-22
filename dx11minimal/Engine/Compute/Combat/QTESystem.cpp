#include <algorithm>
#include "QTESystem.h"

QTESimpleSystem::QTESimpleSystem() = default;
QTESimpleSystem::~QTESimpleSystem() = default;

void QTESimpleSystem::StartQTE(QTESimple& qte) {
    // Останавливаем текущий QTE если есть
    if (activeQTE) {
        StopQTE();
    }

    activeQTE = &qte;

    // Сбрасываем состояние
    qte.isActive = true;
    qte.isCompleted = false;
    qte.isSuccess = false;
    qte.isPerfect = false;
    qte.currentTime = 0.0f;
    qte.currentPresses = 0;
    qte.currentSequenceStep = 0;
    isKeyHeld = false;

    // Показываем UI в зависимости от типа
    if (ui) {
        switch (qte.type) {
        case QTEType::BUTTON_MASH:
            ui->ShowMashUI(qte.requiredPresses, qte.duration);
            break;
        case QTEType::TIMED_PRESS:
            ui->ShowTimedUI(qte.duration, qte.perfectWindowStart, qte.perfectWindowEnd);
            break;
        case QTEType::SEQUENCE:
            ui->ShowSequenceUI(qte.keySequence, qte.duration);
            break;
        case QTEType::HOLD:
            ui->ShowHoldUI(qte.duration);
            break;
        }

        if (qte.requiredKey != 0) {
            ui->SetPromptKey(qte.requiredKey);
        }
    }
}

void QTESimpleSystem::StopQTE() {
    if (activeQTE && ui) {
        ui->HideUI();

        activeQTE->isActive = false;
        activeQTE->isCompleted = true;
        activeQTE->isSuccess = false;
        activeQTE = nullptr;
    }
}

void QTESimpleSystem::Update(float deltaTime) {
    if (!activeQTE || !activeQTE->isActive) return;

    activeQTE->currentTime += deltaTime;
    float progress = activeQTE->currentTime / activeQTE->duration;

    // Проверка таймаута
    if (activeQTE->currentTime >= activeQTE->duration) {
        // Для HOLD - успех если додержали
        if (activeQTE->type == QTEType::HOLD && isKeyHeld) {
            Complete(true);
        }
        else {
            Complete(false);
        }
        return;
    }

    // Обновление UI
    if (ui) {
        bool isPerfect = IsInPerfectWindow();

        switch (activeQTE->type) {
        case QTEType::BUTTON_MASH:
            ui->UpdateMashCount(activeQTE->currentPresses, activeQTE->requiredPresses);
            break;
        case QTEType::TIMED_PRESS:
            ui->UpdateProgress(progress, isPerfect);
            break;
        case QTEType::SEQUENCE:
            ui->UpdateSequenceStep(activeQTE->currentSequenceStep,
                activeQTE->keySequence.size());
            break;
        case QTEType::HOLD:
            ui->UpdateProgress(progress, false);
            break;
        }
    }
}

void QTESimpleSystem::OnKeyPressed(int keyCode) {
    if (!activeQTE || !activeQTE->isActive) return;

    switch (activeQTE->type) {
    case QTEType::BUTTON_MASH:
        ProcessButtonMash(keyCode);
        break;
    case QTEType::TIMED_PRESS:
        ProcessTimedPress(keyCode);
        break;
    case QTEType::SEQUENCE:
        ProcessSequence(keyCode);
        break;
    case QTEType::HOLD:
        ProcessHoldPress(keyCode);
        break;
    }
}

void QTESimpleSystem::OnKeyReleased(int keyCode) {
    if (!activeQTE || !activeQTE->isActive) return;

    if (activeQTE->type == QTEType::HOLD) {
        ProcessHoldRelease(keyCode);
    }
}

void QTESimpleSystem::ProcessButtonMash(int keyCode) {
    // Если задана конкретная клавиша
    if (activeQTE->requiredKey != 0 && keyCode != activeQTE->requiredKey) {
        return;  // Игнорируем неверные клавиши
    }

    activeQTE->currentPresses++;

    if (ui) {
        ui->UpdateMashCount(activeQTE->currentPresses, activeQTE->requiredPresses);
    }

    if (activeQTE->currentPresses >= activeQTE->requiredPresses) {
        Complete(true);
    }
}

void QTESimpleSystem::ProcessTimedPress(int keyCode) {
    int expectedKey = activeQTE->requiredKey != 0 ? activeQTE->requiredKey : 0x20;// SPACE

    if (keyCode == expectedKey) {
        bool isPerfect = IsInPerfectWindow();
        Complete(true, isPerfect);
    }
}

void QTESimpleSystem::ProcessSequence(int keyCode) {
    if (activeQTE->currentSequenceStep >= activeQTE->keySequence.size()) {
        return;
    }

    int expectedKey = activeQTE->keySequence[activeQTE->currentSequenceStep];

    if (keyCode == expectedKey) {
        activeQTE->currentSequenceStep++;

        if (ui) {
            ui->UpdateSequenceStep(activeQTE->currentSequenceStep,
                activeQTE->keySequence.size());
        }

        if (activeQTE->currentSequenceStep >= activeQTE->keySequence.size()) {
            Complete(true);
        }
    }
    else {
        Complete(false);
    }
}

void QTESimpleSystem::ProcessHoldPress(int keyCode) {
    int expectedKey = activeQTE->requiredKey != 0 ? activeQTE->requiredKey : 0x20;// SPACE

    if (keyCode == expectedKey) {
        isKeyHeld = true;
    }
}

void QTESimpleSystem::ProcessHoldRelease(int keyCode) {
    int expectedKey = activeQTE->requiredKey != 0 ? activeQTE->requiredKey : 0x20;// SPACE

    if (keyCode == expectedKey && isKeyHeld) {
        isKeyHeld = false;
        Complete(false);  // Отпустили раньше времени
    }
}

bool QTESimpleSystem::IsInPerfectWindow() const {
    if (!activeQTE) return false;

    float normalizedTime = activeQTE->currentTime / activeQTE->duration;
    return normalizedTime >= activeQTE->perfectWindowStart &&
        normalizedTime <= activeQTE->perfectWindowEnd;
}

void QTESimpleSystem::Complete(bool success, bool isPerfect) {
    if (!activeQTE) return;

    activeQTE->isActive = false;
    activeQTE->isCompleted = true;
    activeQTE->isSuccess = success;
    activeQTE->isPerfect = isPerfect;

    if (ui) {
        ui->HideUI();
    }

    // Вызываем коллбэк
    if (activeQTE->onComplete) {
        activeQTE->onComplete(success, isPerfect);
    }

    activeQTE = nullptr;
}
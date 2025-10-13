// ¬ файл TransitionState.h
#pragma once

enum class TransitionState {
    NONE,
    FADE_OUT,
    FADE_IN,
    COMPLETE
};

struct GameTransition {
    TransitionState state = TransitionState::NONE;
    float duration = 1000.0f;
    float progress = 0.0f;
    DWORD startTime = 0;
    gameState_ fromState;
    gameState_ toState;

    // Ёффекты перехода
    float fadeAlpha = 0.0f;
};

extern GameTransition currentTransition;

void StartTransition(gameState_ toState, float transitionDuration = 1000.0f);
void UpdateTransition(float deltaTime);
void DrawTransitionOverlay();
void InitializeState(gameState_ newState);
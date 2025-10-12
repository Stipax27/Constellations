#pragma once
#include "DirectXMath.h"

using namespace DirectX;

namespace CameraTargeting
{
    struct TargetingData
    {
        bool isTargeting = false;
        int targetEnemyID = -1;
        XMVECTOR targetPosition = XMVectorSet(0, 0, 0, 0);
        float targetSmoothness = 0.1f;
        float targetingDistance = 3000.0f;
        float normalDistance = 1000.0f;
        float targetingVerticalOffset = 100.0f;
        float targetingHorizontalOffset = 0.0f;

        // Для плавного перехода
        float currentTransition = 0.0f;
        float transitionSpeed = 3.0f;
         bool wasTogglePressed = false;
         DWORD lastToggleTime = 0;
         const DWORD TOGGLE_COOLDOWN = 500;
    };

    extern TargetingData data;

    // Объявления функций (реализация в .cpp)
    void StartTargeting(int enemyID);
    void StopTargeting();
    void ToggleTargeting(int enemyID);
    void UpdateTargeting(float deltaTime);
    bool IsTargeting();
    int GetTargetEnemyID();
    XMVECTOR GetTargetPosition();
}

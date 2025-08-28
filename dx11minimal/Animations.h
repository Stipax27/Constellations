#include "Quaternions.h"

namespace Hero {
    struct State; 
}

namespace HeroAnimations {

    struct QuaternionState {
        DXMathExtensions::Quaternion currentRotation = DXMathExtensions::Quaternion::Identity();
        DXMathExtensions::Quaternion rotationBeforeAttack = DXMathExtensions::Quaternion::Identity();
        DXMathExtensions::Quaternion targetRotation = DXMathExtensions::Quaternion::Identity();
        DXMathExtensions::Quaternion animationStartRotation = DXMathExtensions::Quaternion::Identity();
        DXMathExtensions::Quaternion animationTargetRotation = DXMathExtensions::Quaternion::Identity();

        // Методы для синхронизации с основной структурой Hero::State
        void syncFromXMVectors(const DirectX::XMVECTOR& current,
            const DirectX::XMVECTOR& beforeAttack,
            const DirectX::XMVECTOR& target);

        void syncToXMVectors(DirectX::XMVECTOR& current,
            DirectX::XMVECTOR& beforeAttack,
            DirectX::XMVECTOR& target) const;

        // Конвертация отдельных кватернионов
        static DXMathExtensions::Quaternion fromXMVECTOR(const DirectX::XMVECTOR& xmQuat);
        static DirectX::XMVECTOR toXMVECTOR(const DXMathExtensions::Quaternion& quat);
    };

    // Вспомогательные функции для анимаций
    namespace AnimationHelpers {
        void StartRotationAnimation(QuaternionState& state, float yawDegrees, float pitchDegrees, float rollDegrees);
        void UpdateRotationAnimation(QuaternionState& state, Hero::State& heroState, float deltaTime);
        void RotateTowardsTarget(QuaternionState& state, Hero::State& heroState,
            const DirectX::XMVECTOR& targetDirection, float deltaTime);
    }

}

namespace HeroAnimations {

    void QuaternionState::syncFromXMVectors(const DirectX::XMVECTOR& current,
        const DirectX::XMVECTOR& beforeAttack,
        const DirectX::XMVECTOR& target) {
        currentRotation = fromXMVECTOR(current);
        rotationBeforeAttack = fromXMVECTOR(beforeAttack);
        targetRotation = fromXMVECTOR(target);
    }

    void QuaternionState::syncToXMVectors(DirectX::XMVECTOR& current,
        DirectX::XMVECTOR& beforeAttack,
        DirectX::XMVECTOR& target) const {
        current = toXMVECTOR(currentRotation);
        beforeAttack = toXMVECTOR(rotationBeforeAttack);
        target = toXMVECTOR(targetRotation);
    }

    DXMathExtensions::Quaternion QuaternionState::fromXMVECTOR(const DirectX::XMVECTOR& xmQuat) {
        return DXMathExtensions::Quaternion(xmQuat);
    }

    DirectX::XMVECTOR QuaternionState::toXMVECTOR(const DXMathExtensions::Quaternion& quat) {
        return quat.toXMVECTOR();
    }

    namespace AnimationHelpers {

        void StartRotationAnimation(QuaternionState& state, float yawDegrees, float pitchDegrees, float rollDegrees) {
            state.animationStartRotation = state.currentRotation;

            // Создаем целевое вращение через наш класс
            DXMathExtensions::Quaternion deltaRot = DXMathExtensions::Quaternion::FromEulerAngles(
                XMConvertToRadians(rollDegrees),
                XMConvertToRadians(pitchDegrees),
                XMConvertToRadians(yawDegrees)
            );

            state.animationTargetRotation = state.currentRotation * deltaRot;
        }

        void UpdateRotationAnimation(QuaternionState& state, Hero::State& heroState, float deltaTime) 
        {
            if (!heroState.isAnimating) return;

            float progress = min(1.0f, (GetTickCount() - heroState.animationStartTime) /
                (heroState.animationDuration * 1000.0f));

            float easedProgress = 1.0f - powf(1.0f - progress, 3.0f);

            // Используем наш Slerp
            state.currentRotation = DXMathExtensions::Quaternion::Slerp(
                state.animationStartRotation,
                state.animationTargetRotation,
                easedProgress
            );

            // Синхронизируем обратно в Hero::State
            state.syncToXMVectors(heroState.currentRotation,
                heroState.rotationBeforeAttack,
                heroState.targetRotation);

            // Обновляем матрицы и направления
            heroState.constellationSubOffset = XMMatrixRotationQuaternion(heroState.currentRotation);
            heroState.worldMatrix = heroState.constellationSubOffset;

            heroState.Up = XMVector3Rotate(heroState.defaultUp, heroState.currentRotation);
            heroState.Forwardbuf = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), heroState.currentRotation);
            heroState.Right = XMVector3Rotate(XMVectorSet(-1, 0, 0, 0), heroState.currentRotation);

            if (progress >= 1.0f) {
                heroState.isAnimating = false;
                state.currentRotation = state.animationTargetRotation;
                state.syncToXMVectors(heroState.currentRotation,
                    heroState.rotationBeforeAttack,
                    heroState.targetRotation);
            }
        }

        void RotateTowardsTarget(QuaternionState& state, Hero::State& heroState,
            const DirectX::XMVECTOR& targetDirection, float deltaTime) {

            DXMathExtensions::Quaternion targetRot = DXMathExtensions::Quaternion::FromDirection(
                targetDirection, heroState.defaultUp);

            // Плавное вращение к цели
            float rotationSpeed = heroState.rotationSpeed * deltaTime;
            state.currentRotation = state.currentRotation.rotateTowards(targetRot, rotationSpeed);

            // Синхронизируем обратно
            state.syncToXMVectors(heroState.currentRotation,
                heroState.rotationBeforeAttack,
                heroState.targetRotation);

            // Обновляем матрицы и направления
            heroState.constellationSubOffset = XMMatrixRotationQuaternion(heroState.currentRotation);
            heroState.worldMatrix = heroState.constellationSubOffset;

            heroState.Up = XMVector3Rotate(heroState.defaultUp, heroState.currentRotation);
            heroState.Forwardbuf = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), heroState.currentRotation);
            heroState.Right = XMVector3Rotate(XMVectorSet(-1, 0, 0, 0), heroState.currentRotation);
        }

    } 
} 


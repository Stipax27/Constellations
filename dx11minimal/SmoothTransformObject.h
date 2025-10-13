// В файл SmoothTransformObject.h
#pragma once


class SmoothTransform {
private:
    Constellation* targetConstellation; // Указатель на конкретное созвездие

public:
    XMMATRIX startTransform;
    XMMATRIX targetTransform;
    DWORD startTime;
    float duration;
    bool inProgress = false;

    void Start(Constellation* constellation, XMMATRIX target, float transitionDuration = 1000.0f) {
        targetConstellation = constellation;
        startTransform = constellation->Transform;
        targetTransform = target;
        startTime = currentTime;
        duration = transitionDuration;
        inProgress = true;
    }

    void Update() {
        if (!inProgress || !targetConstellation) return;

        float progress = min((float)(currentTime - startTime) / duration, 1.0f);
        float smoothProgress = SmoothStep(progress);

        // Интерполяция матриц
        targetConstellation->Transform = InterpolateMatrices(startTransform, targetTransform, smoothProgress);

        if (progress >= 1.0f) {
            inProgress = false;
            targetConstellation->Transform = targetTransform;
        }
    }

private:
    float SmoothStep(float x) {
        return x * x * (3 - 2 * x);
    }

    XMMATRIX InterpolateMatrices(const XMMATRIX& m1, const XMMATRIX& m2, float t) {
        // Декомпозиция матриц и интерполяция компонентов
        // Упрощенная версия - интерполяция позиции и масштаба

        XMVECTOR pos1 = m1.r[3];
        XMVECTOR pos2 = m2.r[3];
        XMVECTOR currentPos = XMVectorLerp(pos1, pos2, t);

        XMVECTOR scale1 = XMVectorSet(
            XMVectorGetX(XMVector3Length(m1.r[0])),
            XMVectorGetY(XMVector3Length(m1.r[1])),
            XMVectorGetZ(XMVector3Length(m1.r[2])),
            0
        );
        XMVECTOR scale2 = XMVectorSet(
            XMVectorGetX(XMVector3Length(m2.r[0])),
            XMVectorGetY(XMVector3Length(m2.r[1])),
            XMVectorGetZ(XMVector3Length(m2.r[2])),
            0
        );
        XMVECTOR currentScale = XMVectorLerp(scale1, scale2, t);

        return XMMatrixScalingFromVector(currentScale) * XMMatrixTranslationFromVector(currentPos);
    }
};
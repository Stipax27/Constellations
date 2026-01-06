#include "utils.h"

float clamp(float x, float a, float b) {
    return fmax(fmin(x, b), a);
}

float smoothstep(float edge0, float edge1, float x) {
    // Scale, bias and saturate x to 0..1 range
    clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Evaluate polynomial
    return x * x * (3 - 2 * x);
}

float fract(float a) {
    return a - floor(a);
}

float lerp(float x1, float x2, float a) {
    return x1 * (1 - a) + x2 * a;
}

float degreesToRadians(float degrees) {
    return degrees * PI / 180.0f;
}

point3d quaternionToEuler(float x, float y, float z, float w) {
    float magintude = sqrt(x * x + y * y + z * z);
    x /= magintude;
    y /= magintude;
    z /= magintude;

    float pitch = atan2(2 * (w * x + y * z), 1 - 2 * (pow(x, 2) + pow(y, 2)));
    float yaw = asin(2 * (w * y - z * x));
    float roll = atan2(2 * (w * z + x * y), 1 - 2 * (pow(y, 2) + pow(z, 2)));

    return point3d(pitch, yaw, roll);
}

DirectX::XMVECTOR eulerToQuanternion(float x, float y, float z) {
    DirectX::XMVECTOR qRotation = DirectX::XMQuaternionRotationRollPitchYaw(x, y, z);
    qRotation = DirectX::XMQuaternionNormalize(qRotation);

    return qRotation;
}

int getRandom(int min, int max) {
    if (max > min)
    {
        return rand() % (max - min) + min;
    }
    else
    {
        return rand() % min;
    }
}

void SetLookVector(Transform* transform, point3d direction){
    point3d currentLookVector = transform->GetLookVector();
    point3d rotationAxis = currentLookVector.cross(direction).normalized();
    DirectX::XMVECTOR rotationAxisVector = DirectX::XMVectorSet(rotationAxis.x, rotationAxis.y, rotationAxis.z, 0.0f);
    float angleBetweenVectors = acosf(currentLookVector.dot(direction));
    DirectX::XMVECTOR quaternionRotation = DirectX::XMQuaternionRotationAxis(rotationAxisVector, angleBetweenVectors);
    DirectX::XMMATRIX matrixRotation = DirectX::XMMatrixRotationQuaternion(quaternionRotation);

    transform->mRotation = matrixRotation * transform->mRotation;
}

vector<point3d> smoothCornersPath(const vector<point3d>& pointsBefore, int numberIterations) {

    if (pointsBefore.size() <= 1) return pointsBefore;

    vector<point3d> pointsAfter;

    for (int i = 0; i < pointsBefore.size(); i++) {

        int iPrevious;
        int iNext;

        if (i == 0) {
            iPrevious = pointsBefore.size() - 1;
            iNext = i + 1;
        }
        else if (i == pointsBefore.size() - 1) {
            iPrevious = i - 1;
            iNext = 0;
        }
        else {
            iPrevious = i - 1;
            iNext = i + 1;
        }

        point3d previousPoint = pointsBefore[iPrevious];
        point3d currentPoint = pointsBefore[i];
        point3d nextPoint = pointsBefore[iNext];

        point3d localStartPoint = previousPoint.lerp(currentPoint, 0.6666f);
        point3d localEndPoint = currentPoint.lerp(nextPoint, 0.3333f);
        point3d supportivePoint = localStartPoint.lerp(localEndPoint, 0.5f);
        point3d localMiddlePoint = supportivePoint.lerp(currentPoint, 0.5f);

        pointsAfter.push_back(localStartPoint);
        pointsAfter.push_back(localMiddlePoint);
        pointsAfter.push_back(localEndPoint);

    }

    if (numberIterations <= 1) return pointsAfter;
    else return smoothCornersPath(pointsAfter, numberIterations-1);
}
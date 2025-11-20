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

Transform GetWorldTransform(Entity* entity) {
    Transform worldTransform = Transform();

    Transform* originTransform = entity->GetComponent<Transform>();
    if (originTransform != nullptr) {

        vector<Transform*> transforms = { originTransform };

        Entity* ancestor = entity->parent;
        while (ancestor != nullptr) {

            Transform* parentTransform = ancestor->GetComponent<Transform>();
            if (parentTransform != nullptr) {
                transforms.push_back(parentTransform);
                ancestor = ancestor->parent;
            }
            else {
                break;
            }
        }

        for (int i = transforms.size() - 1; i >= 0; i--) {
            worldTransform += *transforms[i];
        }
    }

    return worldTransform;
}
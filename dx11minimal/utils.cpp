#include "utils.h"

float clamp(float x, float a, float b)
{
    return fmax(fmin(x, b), a);
}

float smoothstep(float edge0, float edge1, float x) {
    // Scale, bias and saturate x to 0..1 range
    clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Evaluate polynomial
    return x * x * (3 - 2 * x);
}

float fract(float a)
{
    return a - floor(a);
}

float lerp(float x1, float x2, float a)
{
    return x1 * (1 - a) + x2 * a;
}

float degreesToRadians(float degrees)
{
    return degrees * PI / 180.0f;
}

point3d quaternionToEuler(float x, float y, float z, float w)
{
    float magintude = sqrt(x * x + y * y + z * z);
    x /= magintude;
    y /= magintude;
    z /= magintude;

    float pitch = atan2(2 * (w * x + y * z), 1 - 2 * (pow(x, 2) + pow(y, 2)));
    float yaw = asin(2 * (w * y - z * x));
    float roll = atan2(2 * (w * z + x * y), 1 - 2 * (pow(y, 2) + pow(z, 2)));

    return point3d(pitch, yaw, roll);
}

XMVECTOR eulerToQuanternion(float x, float y, float z)
{
    XMVECTOR qRotation = XMQuaternionRotationRollPitchYaw(x, y, z);
    qRotation = XMQuaternionNormalize(qRotation);

    return qRotation;
}
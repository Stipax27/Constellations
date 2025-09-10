#pragma once

#define PI 3.1415926535897932384626433832795f

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
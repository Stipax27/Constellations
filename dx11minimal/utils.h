float saturate(float x)
{
    return max(min(x, 1.), 0.);// размеры экрана.
}

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

float hashf(float n)
{
    return fract(sin(n) * 43758.5453f);
}

float noise(point3d point)
{
    point3d p = point.floor3();
    point3d f = point.fract();

    f *= f;

    f = f * f * (3.0f - 2.0f * f.x);
    float n = p.x + p.y * 57.0f + 113.0f * p.z;

    float a = lerp(lerp(lerp(hashf(n + 0.0f), hashf(n + 1.0f), f.x),
        lerp(hashf(n + 57.0f), hashf(n + 58.0f), f.x), f.y),
        lerp(lerp(hashf(n + 113.0f), hashf(n + 114.0f), f.x),
            lerp(hashf(n + 170.0f), hashf(n + 171.0f), f.x), f.y), f.z);

    return a - 0.5f;
}
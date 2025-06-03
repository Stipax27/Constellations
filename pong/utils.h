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

void move(point3d& p, float x, float y, float z)
{
    p.x += x;
    p.y += y;
    p.z += z;
}

void move(point3d& p, point3d& ofs)
{
    p.x += ofs.x;
    p.y += ofs.y;
    p.z += ofs.z;
}

void rotateX(point3d& p, float angle)// поворот по Оси X.
{
    float a = angle * PI / 180.;

    float x1 = p.x;
    float y1 = p.y * cos(a) - p.z * sin(a);
    float z1 = p.y * sin(a) + p.z * cos(a);

    p.x = x1;
    p.y = y1;
    p.z = z1;
}

void rotateY(point3d& p, float angle)// поворот по Оси Y.
{
    float a = angle * PI / 180.;

    float x1 = p.x * cos(a) - p.z * sin(a);
    float y1 = p.y;
    float z1 = p.x * sin(a) + p.z * cos(a);

    p.x = x1;
    p.y = y1;
    p.z = z1;
}

void rotateZ(point3d& p, float angle)// поворот по Оси Z.
{
    float a = angle * PI / 180.;

    float x1 = p.x * cos(a) - p.y * sin(a);
    float y1 = p.x * sin(a) + p.y * cos(a);
    float z1 = p.z;

    p.x = x1;
    p.y = y1;
    p.z = z1;
}
float lerp(float x1, float x2, float a) 
{
    return x1 * (1 - a) + x2 * a;
}
point3d lerp(point3d p1, point3d p2, float a)
{
    return { lerp(p1.x, p2.x, a), lerp(p1.y, p2.y, a), lerp(p1.z, p2.z, a) };
}
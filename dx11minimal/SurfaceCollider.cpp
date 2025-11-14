#ifndef _SURFACECOLLIDER_C_
#define _SURFACECOLLIDER_C_

#include "component.h"
#include "point3d.h"
#include <cmath>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

struct SurfaceCollider : Component
{
    float friction = 1.0f;
    float AriesNebulaLerpFactor = 0.0f;

    // Параметры поверхности соответствующие шейдеру
    point3d basePos = point3d(0, 0, 50);  // Базовая позиция (аналогично gConst[0].xyz в шейдере)
    float scale = 1.0f;                   // Масштаб (аналогично gConst[0].w в шейдере)

    // Вычисляет высоту поверхности в точке (x, z)
    float getHeight(float x, float z) const
    {
        float localX = x - basePos.x;
        float localZ = z - basePos.z;

        float par = localX * localX + localZ * localZ;
        float L = lerp(50.0f, 40.0f, AriesNebulaLerpFactor);

        float height = cos(localX / 200 * PI) * cos(localZ / 200 * PI) * 15.0f
            - (par * 24.0f) / (L + par)
            + basePos.y;

        return height * scale;
    }

    // Вычисляет нормаль поверхности в точке (x, z)
    point3d getNormal(float x, float z) const
    {
        float localX = x - basePos.x;
        float localZ = z - basePos.z;

        float r2 = localX * localX + localZ * localZ;
        float L = lerp(50.0f, 40.0f, AriesNebulaLerpFactor);

        // Производные компоненты функции высоты
        float k = PI / 200.0f;

        // Производная от косинусной компоненты
        float dcos_dx = -sin(localX * k) * k * cos(localZ * k) * 15.0f;
        float dcos_dz = -cos(localX * k) * sin(localZ * k) * k * 15.0f;

        // Производная от рациональной компоненты
        float denom = L + r2;
        float dfrac_dx = -(24.0f * (2.0f * localX * denom - 2.0f * localX * r2)) / (denom * denom);
        float dfrac_dz = -(24.0f * (2.0f * localZ * denom - 2.0f * localZ * r2)) / (denom * denom);

        // Суммарные производные
        float df_dx = dcos_dx + dfrac_dx;
        float df_dz = dcos_dz + dfrac_dz;

        // Нормаль = (-df/dx, 1, -df/dz) с последующей нормализацией
        point3d normal(-df_dx * scale, 1.0f, -df_dz * scale);
        return normal.normalized();
    }

private:
    float lerp(float a, float b, float t) const
    {
        return a + (b - a) * t;
    }
};

#endif
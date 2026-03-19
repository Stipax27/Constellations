#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>

float toRad(float a)
{
    return a * PI / 180.;
}

// Шумовая функция
float fbm(float3 p, int octaves)
{
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    
    for (int i = 0; i < octaves; i++)
    {
        value += amplitude * noise3(p * frequency);
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    return value;
}

// Функция для создания формы Туманности Медузы
float3 medusaNebulaForm(float3 pos, float t, uint iid)
{
    float r = length(pos);
    float3 dir = normalize(pos);
    
    // Смещаем центр
    pos.x += 1.2;
    pos.y -= 0.5;
    
    // Создаем основную структуру - кольцо с щупальцами
    float angle = atan2(pos.y, pos.x);
    float height = pos.z;
    
    // Кольцевая структура
    float ring = sin(r * 1.5 - t * 0.2) * 1.5;
    
    // Щупальца, свисающие вниз
    float tentacle1 = sin(angle * 5.0 + t * 0.5) * exp(-abs(height + 1.0) * 0.5) * 2.0;
    float tentacle2 = cos(angle * 4.0 - t * 0.3) * exp(-abs(height + 1.5) * 0.4) * 1.5;
    
    // Добавляем шум
    float noise1 = fbm(pos * 1.2 + t * 0.2, 2) * 1.2;
    float noise2 = fbm(pos * 1.8 - t * 0.3, 2) * 1.0;
    
    // Разбрасываем частицы
    pos.x += tentacle1 * 0.4 + noise1 * 0.8;
    pos.y -= abs(tentacle1 * 0.6 + tentacle2 * 0.3) + noise2 * 0.5;
    pos.z += tentacle2 * 0.4 + noise1 * 0.8;
    
    // Добавляем выбросы
    pos += dir * (ring * 0.4 + noise2 * 0.8);
    
    return pos;
}

float3 pillar(uint qid, uint iid, float2 grid, float a, float t, float h)
{
    // Начальная позиция
    float3 pos = float3(
        (hash(iid/200.0) * 2.0 - 1.0) * 5.0,
        (hash(iid/140.0) * 2.0 - 1.0) * 6.0,
        (hash(iid/120.0) * 2.0 - 1.0) * 5.0
    );
    
    // Применяем форму Медузы
    pos = medusaNebulaForm(pos, t, iid);
    
    return pos * 2.5;
}

pos_color CalcParticles(uint qid, uint iid, float4 grid)
{
    float localTime = drawerV[0];
    float scale = nScale;
    float lowerScale = sqrt(scale);
    
    qid *= nSkipper;
    float t = localTime * 0.045;
    uint inStars = 17000;
    
    // Для звезд время не меняется
    if (iid % inStars == 0)
    {
        t = 0;
    }
    
    // Позиция частицы
    float3 pos = pillar(qid, iid, grid.xy, 0, t, 0) * scale * 2.0;
    float3 pos2 = pos;
    float len = length(pos);
    
    // Центр Медузы
    float3 medusaCenter = float3(1.2, -0.5, 0.0) * scale * 2.0;
    float distFromCenter = distance(pos, medusaCenter);
    
    pos_color p;
    p.wpos = float4(pos, 1);
    
    // ПОЛУЧАЕМ ЦВЕТ ИЗВНЕ через nBase_color
    float3 baseColor = nBase_color;
    
    // Создаем вариации цвета на основе шума
    float colorNoise1 = fbm(pos * 0.3 + t * 0.1, 3);
    float colorNoise2 = fbm(pos * 0.5 - t * 0.15, 3);
    float colorNoise3 = fbm(pos * 0.8 + t * 0.2, 2);
    
    // Создаем разные оттенки на основе базового цвета
    // Увеличиваем/уменьшаем компоненты для вариаций
    float3 colorLight = baseColor * 1.3;      // Светлее
    float3 colorMedium = baseColor;           // Базовый
    float3 colorDark = baseColor * 0.7;       // Темнее
    
    // Добавляем цветовые акценты (теплые/холодные оттенки)
    float3 warmAccent = float3(1.0, 0.8, 0.6);    // Теплый
    float3 coolAccent = float3(0.6, 0.8, 1.0);    // Холодный
    
    // Смешиваем оттенки в зависимости от шума
    float3 finalColor;
    
    if (colorNoise1 < 0.33)
        finalColor = lerp(colorLight, colorMedium, colorNoise2);
    else if (colorNoise1 < 0.66)
        finalColor = lerp(colorMedium, colorDark, colorNoise3);
    else
        finalColor = lerp(colorDark, colorLight, colorNoise2);
    
    // Добавляем небольшие цветные акценты в зависимости от угла
    float angle = atan2(pos.y - medusaCenter.y, pos.x - medusaCenter.x);
    float accentFactor = sin(angle * 4.0 + t) * 0.2;
    
    if (accentFactor > 0)
        finalColor += accentFactor * warmAccent * 0.3;
    else
        finalColor += abs(accentFactor) * coolAccent * 0.3;
    
    // Добавляем вариации на основе угла и расстояния
    float angleFactor = sin(angle * 3.0 + t) * 0.15;
    float distFactor = exp(-distFromCenter * 0.1) * 0.3;
    
    finalColor += float3(angleFactor * 0.2, angleFactor * 0.1, angleFactor * 0.2);
    finalColor += distFactor * baseColor * 0.5;
    
    // Ядро - более яркое
    float coreGlow = exp(-distFromCenter * 0.15) * 0.5;
    finalColor += coreGlow * baseColor * 1.5;
    
    // Плотность
    float density = fbm(pos * 0.25 + t * 0.1, 2);
    density = density * 0.5 + 0.2 + coreGlow * 0.3;
    density = min(density, 0.9);
    
    p.color = float4(finalColor * density, density * 0.6);
    
    if (nMode == 1) // Режим точек
    {
        float s = hash(iid) * 33 + 11;
        p.pos = transform(pos, grid.zw, s * lowerScale);
        p.color.rgb *= 12;
        p.sz = 172;
    }
    else // Режим свечения (основной)
    {
        p.pos = transform_unisize(pos, grid.zw, 1.5 * lowerScale);
        p.sz = 1;
        
        // ЗВЕЗДЫ - РАЗНОЦВЕТНЫЕ
        if (iid % inStars == 0)
        {
            p.pos = transform_unisize(pos, grid.zw, 151.5 * lowerScale);
            p.sz = 2;
            
            // Генерируем случайный цвет для звезды
            float starHue = hash(iid * 0.7);
            
            // Разные спектральные классы звезд
            float3 starColor;
            
            if (starHue < 0.15)
                starColor = float3(0.6, 0.7, 1.0); // Голубые (горячие)
            else if (starHue < 0.3)
                starColor = float3(0.7, 0.8, 1.0); // Голубовато-белые
            else if (starHue < 0.45)
                starColor = float3(1.0, 1.0, 0.9); // Желтовато-белые
            else if (starHue < 0.6)
                starColor = float3(1.0, 0.9, 0.7); // Желтые
            else if (starHue < 0.75)
                starColor = float3(1.0, 0.7, 0.5); // Оранжевые
            else if (starHue < 0.9)
                starColor = float3(1.0, 0.5, 0.4); // Красноватые
            else
                starColor = float3(0.9, 0.4, 0.8); // Пурпурные
            
            // Случайная яркость
            float starBrightness = 0.5 + hash(iid * 0.8) * 0.5;
            
            // Сохраняем оригинальный цвет туманности, но заменяем цвет звезды
            float3 originalColor = p.color.rgb;
            p.color.rgb = starColor * starBrightness;
            
            // Немного смешиваем с цветом туманности для большей гармонии
            p.color.rgb = lerp(p.color.rgb, originalColor * 0.3, 0.2);
            
            p.color.a = 1.0;
        }
    }
    
    p.sz *= lowerScale;
    
    return p;
}

#include <../lib/particleVS_main2.shader>
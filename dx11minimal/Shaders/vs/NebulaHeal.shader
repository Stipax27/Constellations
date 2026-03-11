#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>

float toRad(float a)
{
    return a * PI / 180.;
}

// Шумовая функция для создания более органичных форм
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

// Функция для создания красивых цветовых градиентов
float3 nebulaColor(float3 pos, float density, float t)
{
    float3 color = float3(0, 0, 0);
    
    // Базовые цвета на основе положения
    float r = sin(pos.x * 0.5 + t * 2.0) * 0.5 + 0.5;
    float g = cos(pos.y * 0.3 + t * 1.3) * 0.5 + 0.5;
    float b = sin(pos.z * 0.7 + t * 1.7) * 0.5 + 0.5;
    
    // Создаем красивые переходы между цветами
    float3 color1 = float3(0.8, 0.4, 1.0); // Фиолетовый
    float3 color2 = float3(0.2, 0.6, 1.0); // Голубой
    float3 color3 = float3(1.0, 0.5, 0.2); // Оранжевый
    
    float blend1 = sin(pos.x * 0.4 + t) * 0.5 + 0.5;
    float blend2 = cos(pos.y * 0.5 + t * 1.5) * 0.5 + 0.5;
    
    color = lerp(color1, color2, blend1);
    color = lerp(color, color3, blend2);
    
    // Добавляем вариации на основе плотности
    color *= density * 0.8 + 0.2;
    
    // Добавляем мерцание
    float sparkle = sin(pos.x * 10.0 + t * 20.0) * 
                    sin(pos.y * 8.0 + t * 15.0) * 
                    sin(pos.z * 12.0 + t * 25.0);
    sparkle = max(0.0, sparkle) * 0.3;
    
    return color + float3(sparkle, sparkle * 0.8, sparkle * 1.2);
}

float3 nebulaPillar(uint qid, uint iid, float2 grid, float a, float t, float h)
{
    // Генерация уникальной позиции для каждой частицы
    float3 pos = float3(
        hash(iid * 127.0),
        hash(iid * 257.0),
        hash(iid * 397.0)
    ) * 2.0 - 1.0;
    
    // Создаем спиральную структуру
    float radius = length(pos.xy);
    float angle = atan2(pos.y, pos.x);
    
    // Добавляем временную эволюцию
    angle += t * 0.5;
    
    // Преобразуем в спираль
    pos.x = radius * cos(angle + pos.z * 2.0);
    pos.y = radius * sin(angle + pos.z * 2.0);
    pos.z = pos.z * 3.0;
    
    // Добавляем турбулентность
    float3 noisePos = pos * 0.8 + float3(t * 0.2, t * 0.3, t * 0.1);
    float noise1 = fbm(noisePos, 3);
    float noise2 = fbm(noisePos * 2.0, 2);
    
    pos += normalize(pos) * noise1 * 0.5;
    pos += float3(noise2, noise2 * 0.7, noise2 * 1.3) * 0.3;
    
    // Создаем слоистую структуру
    float layers = sin(pos.z * 2.0 + t) * sin(pos.x * 3.0 + t * 1.5) * 0.2;
    pos += float3(layers, layers * 0.5, 0);
    
    return pos * 2.0;
}

pos_color CalcParticles(uint qid, uint iid, float4 grid)
{
    float localTime = drawerV[0];
    float scale = nScale;
    float lowerScale = sqrt(scale);
    
    qid *= nSkipper;
    float t = localTime * 0.002; // Замедляем анимацию для более плавного движения
    
    uint inStars = 5000; // Реже звезды
    
    // Генерация позиции частицы
    float3 pos = nebulaPillar(qid, iid, grid.xy, 0, t, 0) * scale;
    float3 posNorm = normalize(pos);
    float posLen = length(pos);
    
    // Расчет плотности для эффекта свечения
    float density = fbm(pos * 0.3 + float3(t * 0.1, t * 0.2, t * 0.15), 3);
    density = max(0.2, density * 0.8);
    
    pos_color p;
    p.wpos = float4(pos, 1);
    
    // Базовый цвет на основе позиции и плотности
    float3 baseColor = nebulaColor(pos, density, t);
    
    // Добавляем яркие ядра
    float cores = sin(posLen * 2.0 - t * 5.0) * 0.5 + 0.5;
    cores = pow(cores, 4.0) * 0.3;
    
    // Центральное свечение
    float centerGlow = exp(-posLen * 0.5) * 0.5;
    
    // Итоговый цвет
    p.color = float4(baseColor + float3(cores, cores * 0.8, cores * 1.2) + centerGlow, density);
    p.color *= nBase_color;
    
    // Обработка разных режимов рендеринга
    if (nMode == 1) // Режим точек
    {
        float s = hash(iid) * 20.0 + 5.0;
        p.pos = transform(pos, grid.zw, s * lowerScale);
        p.color *= 8.0; // Ярче для точек
        p.sz = 200.0;
    }
    else // Режим свечения
    {
        p.pos = transform_unisize(pos, grid.zw, 2.0 * lowerScale);
        p.sz = 1.5;
        
        // Добавляем редкие яркие звезды
        if (iid % inStars == 0)
        {
            p.pos = transform_unisize(pos, grid.zw, 200.0 * lowerScale);
            p.sz = 3.0;
            p.color *= 5.0;
            
            // Делаем звезды более белыми
            float starIntensity = length(p.color.rgb);
            p.color.rgb = float3(starIntensity, starIntensity * 0.9, starIntensity * 1.1);
        }
    }
    
    p.sz *= lowerScale;
    
    // Адаптация прозрачности на основе расстояния
    p.color.a *= 1.0 - saturate(posLen * 0.05);
    
    return p;
}

#include <../lib/particleVS_main2.shader>
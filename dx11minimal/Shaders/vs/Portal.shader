#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>

float toRad(float a)
{
    return a * PI / 180.;
}

// Функция шума для более насыщенной текстуры
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

// Функция сферической деформации (искривление пространства)
float3 warpSpace(float3 pos, float t)
{
    float angle = atan2(pos.y, pos.x);
    float radius = length(pos.xy);
    float warp = sin(angle * 3.0 + t) * 0.15 + cos(angle * 5.0 - t * 1.5) * 0.1;
    
    radius += warp * (1.0 - smoothstep(0.0, 2.0, abs(pos.z)));
    
    float newX = cos(angle) * radius;
    float newY = sin(angle) * radius;
    
    return float3(newX, newY, pos.z);
}

// Вращающаяся спиральная галактика
float3 galaxySpiral(float3 pos, float t)
{
    float r = length(pos);
    float angle = atan2(pos.y, pos.x);
    
    // Создаем рукава
    float spiralAngle = angle + r * 3.0 - t * 0.8;
    float spiralDensity = abs(cos(spiralAngle * 2.0)) * 1.5;
    float spiralDensity2 = abs(sin(spiralAngle * 3.0 - t)) * 1.2;
    
    // Смещаем частицы к рукавам
    float3 offset;
    offset.x = sin(spiralAngle) * (spiralDensity + spiralDensity2) * 0.3;
    offset.y = cos(spiralAngle) * (spiralDensity + spiralDensity2) * 0.3;
    offset.z = sin(r * 5.0 - t * 2.0) * 0.2 * (1.0 - r) * spiralDensity;
    
    return pos + offset;
}

// Эффект черной дыры в центре
float3 blackHoleWarp(float3 pos, float t)
{
    float r = length(pos);
    float strength = 1.0 / (r * r + 0.1) * 0.3;
    
    // Создаем аккреционный диск
    float diskAngle = atan2(pos.y, pos.x);
    float diskDist = abs(pos.z);
    float accretion = exp(-diskDist * 2.0) * sin(diskAngle * 8.0 - t * 5.0) * 0.5;
    
    pos.x += cos(diskAngle * 2.0) * accretion;
    pos.y += sin(diskAngle * 2.0) * accretion;
    pos.z += sin(diskAngle * 7.0) * accretion * 0.3;
    
    // Искривление пространства
    if (r < 1.0)
    {
        pos *= (1.0 + strength * 2.0);
        pos.z += sin(t * 10.0) * 0.1;
    }
    
    return pos;
}

// Основная функция формы порталов
float3 portalCosmicForm(float3 pos, float t, uint iid)
{
    float r = length(pos);
    float3 dir = normalize(pos);
    
    // Центр портала с эффектом линзы
    float lensEffect = 1.0 / (r * 0.5 + 1.0);
    pos *= lensEffect;
    
    // Спиральная галактика
    pos = galaxySpiral(pos, t);
    
    // Искривление пространства
    pos = warpSpace(pos, t);
    
    // Черная дыра в центре
    pos = blackHoleWarp(pos, t);
    
    // Пульсирующая мембрана портала
    float pulse = sin(r * 10.0 - t * 3.0) * 0.15;
    float pulse2 = cos(r * 15.0 + t * 2.0) * 0.1;
    pos += dir * (pulse + pulse2) * (1.0 - smoothstep(0.5, 2.0, r));
    
    // Энергетические нити
    float energyLines = sin(pos.y * 5.0 + t * 4.0) * cos(pos.x * 5.0 - t * 3.0);
    energyLines += sin(pos.z * 8.0 + t * 2.0) * 0.5;
    pos += dir * energyLines * 0.08;
    
    // Дополнительная турбулентность
    float noise1 = fbm(pos * 1.5 + t * 0.3, 3);
    float noise2 = fbm(pos * 2.5 - t * 0.4, 2);
    pos += float3(noise1, noise2, noise1 * 0.5) * 0.15;
    
    return pos * 1.2;
}

// Функция для создания цвета на основе позиции и времени
float4 getCosmicColor(float3 pos, float t, float intensity)
{
    float r = length(pos);
    float angle = atan2(pos.y, pos.x);
    float height = pos.z;
    
    // Градиент от центра к краям
    float centerGlow = exp(-r * 2.0);
    float outerGlow = 1.0 - smoothstep(0.5, 2.0, r);
    
    // Цвета космоса: синий, пурпурный, голубой, золотой
    float3 color1 = float3(0.2, 0.4, 0.9); // Глубокий синий
    float3 color2 = float3(0.7, 0.2, 0.8); // Пурпурный
    float3 color3 = float3(0.2, 0.6, 1.0); // Голубой
    float3 color4 = float3(0.9, 0.5, 0.2); // Золотой/оранжевый (для энергии)
    
    // Динамическое изменение цвета во времени
    float colorShift = angle + t * 0.5;
    float mix1 = sin(colorShift) * 0.5 + 0.5;
    float mix2 = cos(colorShift * 1.3) * 0.5 + 0.5;
    float mix3 = sin(colorShift * 2.0 + t) * 0.5 + 0.5;
    
    // Основной градиент
    float3 baseColor = lerp(color1, color2, mix1);
    baseColor = lerp(baseColor, color3, mix2);
    baseColor = lerp(baseColor, color4, centerGlow * 1.5);
    
    // Добавляем красные/оранжевые акценты на энергичные области
    float energy = sin(r * 15.0 - t * 8.0) * 0.5 + 0.5;
    energy += fbm(pos * 4.0 + t, 2) * 0.5;
    baseColor += float3(0.5, 0.2, 0.0) * energy * centerGlow;
    
    // Мерцание звезд
    float stars = fbm(pos * 8.0, 1);
    stars = pow(stars, 1.5);
    baseColor += float3(0.8, 0.6, 0.3) * stars * 0.4;
    
    // Голубое свечение портала
    float portalGlow = sin(r * 30.0 - t * 15.0) * 0.3 + 0.3;
    portalGlow *= centerGlow;
    baseColor += float3(0.3, 0.5, 1.0) * portalGlow;
    
    // Вращающиеся цветные кольца
    float rings = abs(sin(angle * 12.0 + r * 20.0 - t * 5.0));
    rings += abs(cos(angle * 18.0 - r * 15.0 + t * 7.0));
    baseColor += float3(0.4, 0.8, 1.0) * rings * 0.2 * outerGlow;
    
    return float4(baseColor * intensity, 0.8);
}

float3 pillar(uint qid, uint iid, float2 grid, float a, float t, float h)
{
    // Уникальная начальная позиция для каждой частицы
    float3 seed = float3(
        hash(iid * 1.0),
        hash(iid * 2.718),
        hash(iid * 3.14159)
    );
    
    // Создаем сферическое облако частиц
    float theta = seed.x * 2.0 * PI;
    float phi = acos(2.0 * seed.y - 1.0);
    float radius = 0.8 + pow(seed.z, 1.5) * 1.2;
    
    float3 pos = float3(
        sin(phi) * cos(theta) * radius,
        sin(phi) * sin(theta) * radius,
        cos(phi) * radius
    );
    
    // Добавляем дискообразное распределение для галактического вида
    float diskFactor = abs(pos.z) * 2.0;
    pos.x *= (1.0 + diskFactor * 0.5);
    pos.y *= (1.0 + diskFactor * 0.5);
    
    // Форма портала
    pos = portalCosmicForm(pos, t, iid);
    
    // Индивидуальное смещение для каждой частицы на основе её ID
    float individualWarp = hash(iid * 0.1) * 0.3;
    pos.x += sin(t * 2.0 + hash(iid)) * 0.05 * individualWarp;
    pos.y += cos(t * 2.3 + hash(iid * 1.7)) * 0.05 * individualWarp;
    pos.z += sin(t * 2.8 + hash(iid * 2.3)) * 0.05 * individualWarp;
    
    return pos * 1.8;
}

pos_color CalcParticles(uint qid, uint iid, float4 grid)
{
    float localTime = drawerV[0];
    float scale = nScale;
    float lowerScale = sqrt(scale);
    
    qid *= nSkipper;
    float t = localTime * 0.03;
    
    // Получаем позицию частицы
    float3 pos = pillar(qid, iid, grid.xy, 0, t, 0) * scale * 2.2;
    
    pos_color p;
    p.wpos = float4(pos, 1);
    
    // Вычисляем расстояние от центра для определения интенсивности цвета
    float distanceFromCenter = length(pos) / (scale * 2.2);
    float intensity = 0.8 + (1.0 - distanceFromCenter) * 0.5;
    
    // Добавляем мерцание для каждой частицы
    float twinkle = sin(t * 5.0 + hash(iid) * 100.0) * 0.3 + 0.7;
    intensity *= twinkle;
    
    // Получаем цвет на основе позиции и времени
    p.color = getCosmicColor(pos / scale, t, intensity);
    
    // Добавляем альфа-канал с учетом расстояния от центра
    float alpha = 0.6;
    if (distanceFromCenter > 0.7)
    {
        alpha *= 1.0 - (distanceFromCenter - 0.7) / 0.3;
    }
    p.color.a = alpha * (0.5 + twinkle * 0.5);
    
    // Центральная область делает частицы более яркими
    if (distanceFromCenter < 0.3)
    {
        p.color.rgb += float3(0.5, 0.3, 0.1) * (1.0 - distanceFromCenter * 3.0);
        p.color.a += 0.2;
    }
    
    // Трансформируем позицию для рендеринга
    p.pos = transform_unisize(pos, grid.zw, 1.5 * lowerScale);
    p.sz = 1.2 + hash(iid) * 0.5; // Разный размер частиц
    
    p.sz *= lowerScale;
    
    // Частицы в центре делаем крупнее
    p.sz *= 1.0 + (1.0 - distanceFromCenter) * 0.5;
    
    return p.xyz = {0,0,0};
}

#include <../lib/particleVS_main2.shader>
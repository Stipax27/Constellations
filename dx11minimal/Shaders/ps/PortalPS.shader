#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>

// Пиксельная функция шума (блочная)
float pixelNoise(float3 p, float cellSize)
{
    float3 grid = floor(p / cellSize);
    return hash(grid.x + grid.y * 257.0 + grid.z * 65537.0);
}

// Квантование позиции для пиксельного эффекта
float3 quantizePos(float3 pos, float step)
{
    return floor(pos / step) * step;
}

// Пиксельная спираль (ступенчатая)
float3 pixelGalaxySpiral(float3 pos, float t)
{
    float r = length(pos);
    float angle = atan2(pos.y, pos.x);
    
    // Квантуем углы для ступенчатого вращения (8 направлений)
    float quantAngle = floor(angle / (PI / 4.0)) * (PI / 4.0);
    r = floor(r * 8.0) / 8.0;
    
    // Ступенчатые рукава
    float spiral = floor((quantAngle + r * 5.0 - t) * 2.0) / 2.0;
    float offset = abs(cos(spiral * PI)) * 0.3;
    
    // Пиксельное смещение
    float3 offsetVec;
    offsetVec.x = floor(sin(quantAngle) * offset * 8.0) / 8.0;
    offsetVec.y = floor(cos(quantAngle) * offset * 8.0) / 8.0;
    offsetVec.z = floor(sin(r * 10.0 - t * 4.0) * 0.2 * 4.0) / 4.0;
    
    return pos + offsetVec;
}

// Пиксельная функция формы портала
float3 pixelPortalForm(float3 pos, float t, uint iid)
{
    // Квантуем начальную позицию в пиксельную сетку
    pos = quantizePos(pos, 0.15);
    
    float r = length(pos);
    float3 dir = normalize(pos);
    
    // Блочное вращение
    float blockRot = floor(t * 2.0) / 2.0;
    
    // Ступенчатая галактика
    pos = pixelGalaxySpiral(pos, blockRot);
    
    // Пиксельные энергетические линии (только по осям)
    float energyX = floor(sin(pos.y * 4.0 - t * 5.0) * 2.0);
    float energyY = floor(cos(pos.x * 4.0 + t * 5.0) * 2.0);
    pos.x += energyX * 0.1;
    pos.y += energyY * 0.1;
    
    // Блочное искривление пространства
    if (r < 1.0)
    {
        float warpIntensity = floor((1.0 - r) * 3.0) / 3.0;
        pos *= (1.0 + warpIntensity * 0.5);
    }
    
    // Пиксельные пульсации
    float pulse = floor(sin(r * 12.0 - t * 6.0) * 2.0) * 0.1;
    pos += dir * pulse;
    
    // Финальное квантование
    pos = quantizePos(pos, 0.12);
    
    return pos;
}

// Пиксельная цветовая палитра (ограниченные цвета)
float3 getPixelColor(int colorIndex)
{
    // Ретро-палитра как в старых играх
    float3 palette[16];
    
    // Базовые цвета космоса
    palette[0] = float3(0.0, 0.0, 0.05);     // Почти черный
    palette[1] = float3(0.05, 0.0, 0.15);    // Темно-фиолетовый
    palette[2] = float3(0.1, 0.0, 0.3);      // Фиолетовый
    palette[3] = float3(0.15, 0.05, 0.45);   // Насыщенный пурпурный
    palette[4] = float3(0.2, 0.1, 0.6);      // Сине-фиолетовый
    palette[5] = float3(0.1, 0.2, 0.7);      // Темно-синий
    palette[6] = float3(0.15, 0.35, 0.85);   // Синий
    palette[7] = float3(0.2, 0.5, 0.9);      // Голубой
    palette[8] = float3(0.3, 0.7, 1.0);      // Ярко-голубой
    palette[9] = float3(0.5, 0.3, 1.0);      // Электрик-пурпурный
    
    // Акцентные цвета (для энергии и звезд)
    palette[10] = float3(1.0, 0.8, 0.2);     // Золотой
    palette[11] = float3(1.0, 0.4, 0.1);     // Оранжевый
    palette[12] = float3(1.0, 0.2, 0.5);     // Розовый
    palette[13] = float3(0.1, 0.8, 0.6);     // Бирюзовый
    palette[14] = float3(0.8, 0.1, 0.1);     // Красный
    palette[15] = float3(1.0, 1.0, 1.0);     // Белый (для ярких звезд)
    
    return palette[clamp(colorIndex, 0, 15)];
}

// Получение индекса цвета на основе позиции (ступенчатый градиент)
int getPixelColorIndex(float3 pos, float t, float intensity)
{
    float r = length(pos);
    float angle = atan2(pos.y, pos.x);
    
    // Ступенчатое расстояние (4 уровня)
    int rLevel = int(floor(r * 6.0));
    rLevel = clamp(rLevel, 0, 5);
    
    // Ступенчатый угол (8 направлений)
    int angleLevel = int(floor(angle / (PI / 4.0))) % 8;
    
    // Временная составляющая
    int tLevel = int(floor(t * 2.0)) % 4;
    
    // Базовый цвет на основе расстояния от центра
    int colorIndex;
    
    if (rLevel < 1)
        colorIndex = 15; // Яркий центр
    else if (rLevel < 2)
        colorIndex = 8;  // Голубой
    else if (rLevel < 3)
        colorIndex = 6;  // Синий
    else if (rLevel < 4)
        colorIndex = 4;  // Пурпурный
    else
        colorIndex = 1;  // Темный фиолетовый
    
    // Добавляем угол для вариации
    if (angleLevel % 2 == 0 && rLevel > 1)
        colorIndex += 2;
    
    // Энергетические вспышки (золотые/оранжевые)
    float energy = floor(sin(r * 15.0 - t * 12.0) * 1.5);
    if (energy > 0.5 && rLevel < 3)
        colorIndex = 10 + (int(floor(t * 3.0)) % 2);
    
    // Случайные яркие пиксели (звезды)
    float starChance = pixelNoise(pos * 8.0, 0.5);
    if (starChance > 0.95 && rLevel > 1)
        colorIndex = 15;
    
    return clamp(colorIndex, 0, 15);
}

float3 pixelPillar(uint qid, uint iid, float2 grid, float a, float t, float h)
{
    // Пиксельная сетка для частиц
    float gridSize = 0.25;
    
    // Дискретная стартовая позиция на сфере
    int thetaSteps = 12;  // 12 направлений по горизонтали
    int phiSteps = 6;     // 6 уровней по вертикали
    
    int thetaIdx = int(hash(iid) * thetaSteps) % thetaSteps;
    int phiIdx = int(hash(iid * 1.7) * phiSteps) % phiSteps;
    float radius = 0.5 + (float(int(hash(iid * 2.3) * 4)) / 4.0) * 1.0;
    
    float theta = (float(thetaIdx) / float(thetaSteps)) * 2.0 * PI;
    float phi = acos(1.0 - 2.0 * (float(phiIdx) / float(phiSteps)));
    
    float3 pos = float3(
        sin(phi) * cos(theta) * radius,
        sin(phi) * sin(theta) * radius,
        cos(phi) * radius
    );
    
    // Квантуем позицию
    pos = quantizePos(pos, gridSize);
    
    // Применяем пиксельную форму портала
    pos = pixelPortalForm(pos, t, iid);
    
    // Дискретное вращение всей системы
    int rotStep = int(t * 2.0) % 8;
    float rotAngle = float(rotStep) * PI / 4.0;
    float cosA = cos(rotAngle);
    float sinA = sin(rotAngle);
    
    float3 rotatedPos;
    rotatedPos.x = pos.x * cosA - pos.y * sinA;
    rotatedPos.y = pos.x * sinA + pos.y * cosA;
    rotatedPos.z = pos.z;
    
    return rotatedPos * 1.8;
}

pos_color CalcParticles(uint qid, uint iid, float4 grid)
{
    float localTime = drawerV[0];
    float scale = nScale;
    float lowerScale = sqrt(scale);
    
    qid *= nSkipper;
    float t = localTime * 0.025;
    
    // Получаем пиксельную позицию
    float3 pos = pixelPillar(qid, iid, grid.xy, 0, t, 0) * scale * 2.2;
    
    // Квантуем финальную позицию для рендеринга
    pos = quantizePos(pos, 0.08);
    
    pos_color p;
    p.wpos = float4(pos, 1);
    
    // Вычисляем цвет (пиксельная палитра)
    float distanceFromCenter = length(pos) / (scale * 2.2);
    float intensity = 0.7 + (1.0 - distanceFromCenter) * 0.6;
    
    // Дискретное мерцание (вкл/выкл)
    float twinkle = float(int(t * 4.0 + hash(iid) * 100.0) % 2);
    intensity *= (0.6 + twinkle * 0.5);
    
    // Получаем индекс цвета из палитры
    int colorIdx = getPixelColorIndex(pos / scale, t, intensity);
    p.color.rgb = getPixelColor(colorIdx);
    
    // Дискретная альфа (только несколько уровней прозрачности)
    float alphaLevels[] = float[](0.0, 0.33, 0.66, 1.0);
    int alphaIdx = clamp(int(distanceFromCenter * 4.0), 0, 3);
    if (distanceFromCenter > 0.8) alphaIdx = 1;
    if (distanceFromCenter > 1.0) alphaIdx = 0;
    
    p.color.a = alphaLevels[alphaIdx] * (0.5 + twinkle * 0.5);
    
    // Пиксельный размер (квадратные частицы)
    p.sz = 1.5 + float(int(hash(iid) * 3)) * 0.5;
    p.sz *= lowerScale;
    
    // Частицы в центре больше
    if (distanceFromCenter < 0.3)
        p.sz *= 1.5;
    
    // Трансформация для рендеринга
    p.pos = transform_unisize(pos, grid.zw, 1.5 * lowerScale);
    
    return p;
}

#include <../lib/particleVS_main2.shader>
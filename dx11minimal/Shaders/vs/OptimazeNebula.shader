#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/constants.shader>
#include <../lib/utils.shader>

float toRad(float a)
{
    return a * PI / 180.;
}


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
    
    
    float noise1 = fbm(pos * 1.2 + t * 0.2, 2) * 1.2;
    float noise2 = fbm(pos * 1.8 - t * 0.3, 2) * 1.0;
    
   
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
    
   
    float3 pos = pillar(qid, iid, grid.xy, 0, t, 0) * scale * 2.0;
    
    pos_color p;
    p.wpos = float4(pos, 1);
    
    
    float3 baseColor = nBase_color;
    
   
    p.color = float4(baseColor, 0.6);
    
   
    p.pos = transform_unisize(pos, grid.zw, 1.5 * lowerScale);
    p.sz = 1;
    
    p.sz *= lowerScale;
    
    return p;
}

#include <../lib/particleVS_main2.shader>
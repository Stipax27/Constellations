cbuffer global : register(b5)
{
    float4 gConst[32];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer camera : register(b3)
{
    float4x4 world[2];
    float4x4 view[2];
    float4x4 proj[2];
};

cbuffer drawMat : register(b2)
{
    float4x4 model;
    float hilight;
};

cbuffer params : register(b1)
{
    float r, g, b, a;  // Добавил альфа-канал
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};

float3 rotZ(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), -sin(a),0,
        sin(a), cos(a), 0,
        0, 0, 1
    };
    pos = mul(pos, m);
    return pos;
}

float star(float2 uv)
{
    float c = saturate(1. - 1. * length(uv));
    c = pow(c, 3);
    c *= saturate(1. - 228. * abs(uv.x) * abs(uv.y));
    c += pow(max(sin(length(uv * 3.14)), 0), 118) * .03;
    return max(c, 0);
}

float4 PS(VS_OUTPUT input) : SV_Target  
{
    float2 uv = input.uv;
    float brightness = exp(-dot(uv, uv) * 20);
    
    // Используем gConst[1] из global буфера
    float4 starColor = float4(gConst[1].x, gConst[1].y, gConst[1].z, gConst[1].w);
    
    // Если цвет не задан (все нули) - используем белый
    if (dot(starColor.rgb, 1.0) == 0) {
        starColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    return float4(brightness, brightness, brightness, 1) * starColor * (1 + .9);
}
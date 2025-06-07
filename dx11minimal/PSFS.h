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
    float r, g, b;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv;

    float dist = length(uv);         // расстояние до центра
    float intensity = (1.0 - dist);  // простая линейная маска

    intensity = pow(saturate(intensity), 2.0); // экспоненциальное затухание

    float3 color = float3(1,1,1) * intensity;

    return float4(color, intensity); // альфа тоже по интенсивности
}

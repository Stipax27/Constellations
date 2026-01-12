Texture2D inputTexture : register(t1);
SamplerState samplerState : register(s1);

cbuffer global : register(b5)
{
    float4 gConst[256];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer camera : register(b3)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float4 cPos;
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
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
    float4 vnorm : NORMAL0;
    float2 uv : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv;
    float brightness = exp(-dot(uv, uv) * 20);

    float dist = length(cPos.xyz - input.wpos);
    float sat = min(max(dist - 1, 0) / 5, 1);
    brightness *= sat;

    return float4(brightness, brightness, brightness, 1) * float4(1, 0.6, 0.9, 1) * 0.01;

    float3 tex = inputTexture.Sample(samplerState, input.uv);

    return float4(tex, 1);
    //return saturate(float4(reflectDir, 1));
}
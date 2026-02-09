cbuffer global : register(b5)
{
    float4 gConst[1024];
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
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float progress = 1 - gConst[0].w;
    return float4(1, 1, 1, 1) * pow(progress, 2);

}

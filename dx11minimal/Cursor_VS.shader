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

cbuffer objParams : register(b0)
{
    float drawerV[32];
};

#define PI 3.1415926535897932384626433832795

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};


VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output;

    float size = 0.04;
    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    float2 mousepos = float2(gConst[0].xy);

    float2 pos = quadPos[vID];

    float2 screenPos = mul(float4(quadPos[vID], 0.0, 1.0), proj[0]).xy;
    screenPos = float2(mousepos.x + screenPos.x * size, mousepos.y - screenPos.y * size);

    output.pos = float4(screenPos.xy, 0, 1);
    output.uv = quadPos[vID];

    return output;
}
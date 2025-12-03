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
};

float3 rotY(float3 p, float a)
{
    float3x3 r;
    r[0] = float3(cos(a), 0, sin(a));
    r[1] = float3(-sin(a), 0, cos(a));
    r[2] = float3(0, 1, 0);
    return mul(p, r);
}

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output;

    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    float3 pos = gConst[0].xyz;
    float2 size = gConst[1].xy;
    float2 anchorPoint = gConst[2].xy;

    //pos = rotY(pos, 1);

    float3 screenPos = float3((quadPos[vID] - anchorPoint) * size, 0.0) * float3(gConst[1].zw, 1) + pos;

    output.pos = float4(screenPos, 1);
    output.uv  = quadPos[vID];

    return output;
}

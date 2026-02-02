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

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 worldpos : POSITION1;
};


VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output;

    //uint starID = vID / 6;
    uint vertexInQuad = vID % 6;

    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    //calc star position

    float size = 1000;
    float3 starPos = float3(0, 0, 0);

    //-----
    float4 viewPos = mul(float4(starPos, 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);
    projPos.xy += quadPos[vertexInQuad] * float2(aspect.x, 1) * size;

    output.uv = quadPos[vertexInQuad];
    output.pos = projPos;
    output.worldpos = float4(starPos, 1);
    return output;
}
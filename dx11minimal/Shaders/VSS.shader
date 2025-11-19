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
    float4x4 world;
    float4x4 view;
    float4x4 proj;
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

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output;
    float sz = gConst[0].w;
    float4 pos = float4(gConst[0].xyz, 1);

    float2 quadUV[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    //pos.xy += quadUV[vID] * sz;
        
    float4 viewPos = mul(pos, view);
    float4 projPos = mul(viewPos, proj);

    projPos.xy += quadUV[vID] * float2(aspect.x, 1) * sz;

    output.uv = quadUV[vID];
    output.pos = projPos;

    return output;
}
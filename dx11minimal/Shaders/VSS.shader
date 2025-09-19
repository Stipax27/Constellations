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
    float sz = 10;

    float2 quadUV[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };
        
    float4 viewPos = mul(float4(0, 0, 0, 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);

    projPos.xy += quadUV[vID] * float2(aspect.x,1) * 4 * sz;

    output.uv = quadUV[vID];
    output.pos = projPos;

    return output;
}
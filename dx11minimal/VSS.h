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
    float sz = gConst[0].z ;
    float2 C = gConst[0].xy;
    float2 quad[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    float2 pos = quad[vID]*sz+C;

    float width = aspect.z;   
    float height = aspect.w;  

    float2 normalizedPos = float2(pos.x / width, pos.y / height) * 2.0f;

    float2 ndc;
    ndc.x = normalizedPos.x - 1.0f;
    ndc.y = 1.0f - normalizedPos.y;
    output.pos = float4(ndc,0,1);
    output.uv = quad[vID];
    return output;
}
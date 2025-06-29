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
    float sz = gConst[0].w ;
    float3 starPos = float3(gConst[0].xyz);
     starPos.x = float(gConst[0].x);
     starPos.y = float(gConst[0].y);
    float2 quad[6] = {
        float2(-sz, -sz), float2(sz, -sz), float2(-sz, sz),
        float2(sz, -sz), float2(sz, sz), float2(-sz, sz)
    };

    float3 right = normalize(view[0]._m00_m10_m20); 
    float3 up = normalize(view[0]._m01_m11_m21); 

    float size = 0.95;

    float2 vertexOffset = quad[vID];
    float3 offset3D = right * vertexOffset.x * size + up * vertexOffset.y * size;

    float3 worldPos = starPos + offset3D;

    float4 viewPos = mul(float4(worldPos, 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);

    output.uv = quad[vID];
    output.pos = projPos;

    return output;
}
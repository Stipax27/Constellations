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

cbuffer drawMat : register(b2)
{
    float4x4 model;
    float hilight;
};

cbuffer objParams : register(b0)
{
    float drawerV[256];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};

#define PI 3.14159265358979323846

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output;
    
    uint n = drawerV[iID];
    uint triangles = n / 3;
    float angle = PI * 2 / triangles;

    uint triangleID = vID / 3;
    uint pID = vID % 3;

    float startAngle = angle * triangleID;
    float endAngle = angle * (triangleID + 1);

    float2 triangleUV[3] = {
        float2(0, 0), float2(sin(startAngle), cos(startAngle)), float2(sin(endAngle), cos(endAngle))
    };

    float2 quadUV[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    float4 pos = float4(gConst[iID].xyz, 1);
    float sz = gConst[iID].w;

    //pos.xy += quadUV[vID] * sz;
        
    float4 viewPos = mul(pos, view);
    float4 projPos = mul(viewPos, proj);

    projPos.xy += triangleUV[pID] * float2(aspect.x, 1) * sz * 2;

    output.uv = triangleUV[pID];
    output.pos = projPos;

    return output;
}
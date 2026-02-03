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

cbuffer objParams : register(b0)
{
    float drawerV[1024];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

#define PI 3.14159265358979323846

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output;

    uint triangles = drawerV[0];

    float angle = PI * 2 / triangles;

    uint triangleID = vID / 3;
    uint pID = vID % 3;

    float startAngle = angle * triangleID;
    float endAngle = angle * (triangleID + 1);

    float2 triangleUV[3] = {
        float2(0, 0), float2(sin(startAngle), cos(startAngle)), float2(sin(endAngle), cos(endAngle))
    };

    float4 pos = float4(gConst[iID].xyz, 1);
    float sz = gConst[iID].w;

    float3 eye = cPos.xyz;
    float3 front = normalize(eye - pos.xyz);

    float3 up = abs(dot(front, float3(0, 1, 0))) > 0.9999 ? float3(0, 0, 1) : float3(0, 1, 0);
    float3 right = normalize(cross(up, front));
    up = normalize(cross(front, right));

    float2 triangleUVpos = triangleUV[pID];
    pos.xyz += (right * triangleUVpos.x + up * triangleUVpos.y) * sz;
        
    float4 viewPos = mul(pos, view);
    float4 projPos = mul(viewPos, proj);

    //projPos.xy += triangleUV[pID] * float2(aspect.x, 1) * sz * 2;

    output.uv = triangleUV[pID];
    output.pos = projPos;

    return output;
}
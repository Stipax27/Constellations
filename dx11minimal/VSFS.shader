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

float hash11(uint n) {
    n = (n << 13u) ^ n;
    return frac((n * (n * n * 15731u + 789221u) + 1376312589u) * 0.000000000931322574615478515625f);
}

float3 randomPosition(uint index) {
    float x = hash11(index * 3u);
    float y = hash11(index * 3u + 1u);
    float z = hash11(index * 3u + 2u);
    return float3(x, y, z);
}

#define PI 3.1415926535897932384626433832795

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

    uint starID = vID / 6;
    uint vertexInQuad = vID % 6;

    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    //calc star position

    float size = 7.0f;
    float range = 2000.0f;
    float3 starPos = normalize(randomPosition(starID) * range * 2 - range) * range;

    float4x4 v = view[0];
    v._m30_m31_m32_m33 = 0.0f;

    //starPos = lerp(normalize(starPos)* 1400.0f, starPos,.5);

    //-----
    float4 viewPos = mul(float4(starPos, 1.0f), v);
    float4 projPos = mul(viewPos, proj[0]);
    projPos.xy += quadPos[vertexInQuad] * float2(aspect.x, 1) * size;

    output.uv = quadPos[vertexInQuad];
    output.pos = projPos;
    output.starID = starID;
    output.worldpos = float4(starPos, 1);
    return output;
}
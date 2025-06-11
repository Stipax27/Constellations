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
    float x = hash11(index * 3u) * 2000.0f - 1000.0f;
    float y = hash11(index * 3u + 1u) * 2000.0f - 1000.0f;
    float z = hash11(index * 3u + 2u) * 2000.0f - 1000.0f;
    return float3(x, y, z);
}

#define PI 3.1415926535897932384626433832795

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : TEXCOORD1;
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

    float3 starPos = randomPosition(starID);
    float3 dir = normalize(starPos);
    float r = pow(hash11(starID + 123u), 1.0f / 3.0f) * 1000;

    starPos = dir * r;
    float3 cameraPos = float3(-(view[0]._m02_m12_m22) * view[0]._m32);
    float3 forward = normalize(cameraPos - starPos);
    float3 worldUp = float3(0,1,0);

    if (abs(dot(forward, worldUp)) > 0.99f)
        worldUp = float3(0, 0, 1);

    float3 right = normalize(view[0]._m00_m10_m20); // X column
    float3 up = normalize(view[0]._m01_m11_m21); // Y column

    float size = 0.95;

    float widthScale = size / aspect.x;  
    float heightScale = size;

    float2 vertexOffset = quadPos[vertexInQuad];
    float3 offset3D = right * vertexOffset.x * size + up * vertexOffset.y * size;

    float3 worldPos = starPos + offset3D;

    float4 viewPos = mul(float4(worldPos, 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);

    output.uv = quadPos[vertexInQuad];
    output.pos = projPos;
    return output;
}
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
};


VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output;

    uint starID = vID / 6;
    uint vertexInQuad = vID % 6;


    float2 quad[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    float3 starPos = randomPosition(starID);

    float3 cameraPos = -(view[0]._m02_m12_m22) * view[0]._m32;

    float3 forward = normalize(cameraPos - starPos);
    float3 worldUp = float3(0, 1, 0);
    if (abs(dot(forward, worldUp)) > 0.99f)
        worldUp = float3(1, 0, 0);

    float3 right = normalize(cross(worldUp, forward));
    float3 up = cross(forward, right);

    float size = 0.95;
    float2 offset2D = quad[vertexInQuad] * size;
    float3 offset3D = right * offset2D.x + up * offset2D.y;

    float3 worldPos = starPos + offset3D;

    float4 viewPos = mul(float4(worldPos, 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);
    output.uv = quad[vertexInQuad];
    output.pos = projPos;
    return output;
}
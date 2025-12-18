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

cbuffer drawMat : register(b2)
{
    float4x4 model;
    float hilight;
};

cbuffer objParams : register(b0)
{
    float drawerV[256];
};

float noise(float3 pos) {
    return frac(sin(dot(pos, float3(12.9898, 78.233, 45.5432))) * 43758.5453);
}

float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float perlinNoise(float3 pos)
{
    float3 i = floor(pos);
    float3 j = frac(pos);

    float a = noise(i);
    float b = noise(i + float3(1, 0, 0));
    float c = noise(i + float3(0, 1, 0));
    float d = noise(i + float3(1, 1, 0));
    float e = noise(i + float3(0, 0, 1));
    float f = noise(i + float3(1, 0, 1));
    float g = noise(i + float3(0, 1, 1));
    float h = noise(i + float3(1, 1, 1));

    float3 u = j * j * (3.0f - 2.0f * j);
    return lerp(
        lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y),
        lerp(lerp(e, f, u.x), lerp(g, h, u.x), u.y),
        u.z
    );
}

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

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};


VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output;

    //uint starID = vID / 6;
    uint vertexInQuad = vID % 6;

    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    //calc star position

    float size = 16;
    float range = 2000.0f;
    float4 camPos = -view._m30_m31_m32_m33;
    //float3 starPos = float3(
    //    perlinNoise(camPos),
    //    perlinNoise(camPos + float3(0, 100.0f, 0)),
    //    perlinNoise(camPos + float3(0, 0, 100.0f))
    //) * range;
    float3 starPos = randomPosition(iID);

    float3 deltaPos = starPos - camPos / range;
    deltaPos *= range * 2 - range;

    //-----
    float4 viewPos = mul(float4(starPos, 1.0f), view);
    float4 projPos = mul(viewPos, proj[0]);
    projPos.xy += quadPos[vertexInQuad] * float2(aspect.x, 1) * size;

    output.uv = quadPos[vertexInQuad];
    output.pos = projPos;
    output.starID = iID;
    output.worldpos = float4(starPos, 1);
    return output;
}
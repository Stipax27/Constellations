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


float hash2(uint n) {
    n = (n << 13u) ^ n;
    return frac((n * (n * n * 15731u + 789221u) + 1376312589u) * 0.000000000931322574615478515625f);
}

float3 randomPosition(uint index) {
    float x = hash2(index * 3u);
    float y = hash2(index * 3u + 1u);
    float z = hash2(index * 3u + 2u);
    return float3(x, y, z);
}


#define PI 3.1415926535897932384626433832795

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};


VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output;

    int segments = gConst[0].z;
    float progress = gConst[0].w;
    float r = 1.0f;

    uint rectIndex = iID / segments;
    float localRect = iID % segments;
    uint localV = vID % 6;
    float2 A = gConst[rectIndex * 2 + 0].xy;
    float2 B = gConst[rectIndex * 2 + 1].xy;
    float2 vec = B - A;

    A = lerp(A, B, localRect / segments);
    B = lerp(A, B, (localRect + 1) / segments);

    float2 offset = (randomPosition(iID + A.x * B.x + A.y * B.y).xy * 2 - 1) * progress;
    A += offset;
    B += offset;

    float2 dir = normalize(vec);
    float2 perp = float2(-dir.y, dir.x);

    float2 A1 = A + perp * r;
    float2 A2 = A - perp * r;
    float2 B1 = B + perp * r;
    float2 B2 = B - perp * r;

    float2 centerA = A;
    float2 centerB = B;

    float2 quad[6];

    // Прямоугольник (2 треугольника)
    quad[0] = A1;
    quad[1] = B1;
    quad[2] = B2;

    quad[3] = B2;
    quad[4] = A2;
    quad[5] = A1;

    float2 pos = quad[localV]; // позиция в "пикселях" или локальной системе

    float width = aspect.z;   // передай width в константный буфер
    float height = aspect.w;  // и height

    float2 normalizedPos = float2(pos.x / width, pos.y / height) * 2.0f;

    float2 ndc;
    ndc.x = normalizedPos.x  - 1.0f;
    ndc.y = 1.0f - normalizedPos.y ;

    output.pos = float4(ndc, 0.0f, 1.0f);
    return output;
}

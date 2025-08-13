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
};


VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output;

    uint rectIndex = vID / 6;
    uint localV = vID % 6;
    float2 A = gConst[rectIndex * 2 + 0].xy;
    float2 B = gConst[rectIndex * 2 + 1].xy;
    float r = 1.0f;

    float2 vec = B - A;
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

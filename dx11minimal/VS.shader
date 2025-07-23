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

    const int arcSteps = 4;
    float angleStep = PI / (arcSteps - 1);
    const int vertsPerRoundedRect = (arcSteps - 1) * 3 * 2 + 6;
    uint rectIndex = vID / vertsPerRoundedRect;
    uint localV = vID % vertsPerRoundedRect;
    float2 arcA[arcSteps];
    float2 arcB[arcSteps];
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
    float step = PI / arcSteps;

    for (int i = 0; i < arcSteps; i++)
    {
        float angle = PI/2 + i * angleStep; 
        float2 offset = float2(cos(angle), sin(angle));
        float2 rotated = float2(
            offset.x * dir.x - offset.y * dir.y,
            offset.x * dir.y + offset.y * dir.x
        );
        arcA[i] = A + rotated * r;
    }

    for (int i = 0; i < arcSteps; i++)
    {
        float angle = -PI/2 + i * angleStep;
        float2 offset = float2(cos(angle), sin(angle));
        float2 rotated = float2(
            offset.x * dir.x - offset.y * dir.y,
            offset.x * dir.y + offset.y * dir.x
        );
        arcB[i] = B + rotated * r;
    }
    float2 centerA = A;
    float2 centerB = B;

    float2 quad[3 * (arcSteps - 1) * 2 + 6];
    int q = 0;

    // ????? ????
    for (int i = 0; i < arcSteps - 1; ++i)
    {
        quad[q++] = centerA;
        quad[q++] = arcA[i];
        quad[q++] = arcA[i + 1];
    }

    // ????????????? (2 ????????????)
    quad[q++] = A1;
    quad[q++] = B1;
    quad[q++] = B2;

    quad[q++] = B2;
    quad[q++] = A2;
    quad[q++] = A1;

    // ?????? ????
    for (int i = 0; i < arcSteps - 1; ++i)
    {
        quad[q++] = centerB;
        quad[q++] = arcB[i];
        quad[q++] = arcB[i + 1];
    }

    float2 pos = quad[localV]; // ??????? ? "????????" ??? ????????? ???????

    float width = aspect.z;   // ??????? width ? ??????????? ?????
    float height = aspect.w;  // ? height

    float2 normalizedPos = float2(pos.x / width, pos.y / height) * 2.0f;

    float2 ndc;
    ndc.x = normalizedPos.x  - 1.0f;
    ndc.y = 1.0f - normalizedPos.y ;

    output.pos = float4(ndc, 0.0f, 1.0f);
    return output;
}

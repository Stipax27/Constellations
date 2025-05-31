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
    float gx; // базовая позиция от CPU
    float gy;
};


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};

static const float2 rectVerts[6] = {
    float2(0, 0), // левый верхний
    float2(1, 0), // правый верхний
    float2(1, 1), // правый нижний

    float2(0, 0), // левый верхний
    float2(1, 1), // правый нижний
    float2(0, 1)  // левый нижний
};

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output;

    uint rectIndex = vID / 6;

    uint localV = vID % 6;

    float2 A = gConst[rectIndex * 2].xy;
    float2 B = gConst[rectIndex * 2 + 1].xy;

    float2 diag = B - A;

    float2 posRect = rectVerts[localV];

    float2 pos = A + diag * posRect;

    float2 ndc;
    ndc.x = (pos.x / 1000.0f) * 2.0f - 1.0f;
    ndc.y = 1.0f - (pos.y / 1000.0f) * 2.0f; 

    output.pos = float4(ndc, 0.0f, 1.0f);

    return output;
}
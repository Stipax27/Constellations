cbuffer global : register(b5)
{
    float4 gConst[32];
};

cbuffer camera : register(b3)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};

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

    float size = 0.025 * gConst[0].w;
    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    float2 mousepos = float2(gConst[0].xy);

    float2 pos = quadPos[vID];

    float2 screenPos = mul(float4(quadPos[vID], 0.0, 1.0), proj).xy;
    screenPos = float2(mousepos.x + screenPos.x * size, mousepos.y - screenPos.y * size);

    output.pos = float4(screenPos.xy, 0, 1);
    output.uv = quadPos[vID];

    return output;
}
cbuffer ColorBuffer : register(b5)
{
    float4 global[4];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    return global[3];
}
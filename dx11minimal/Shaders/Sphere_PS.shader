cbuffer global : register(b5)
{
    float4 gConst[256];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    return float4(gConst[1].xyz, 1);
}

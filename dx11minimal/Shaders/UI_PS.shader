cbuffer global : register(b5)
{
    float4 gConst[32];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 color = gConst[3];
    float c = 1 - saturate(length(input.uv));

    return color * float4(c, c, c, c);
}

#include <lib/constBuf.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};


float4 PS(VS_OUTPUT input) : SV_Target
{
    float y = input.uv.y;
    float c = saturate(1 - sqrt(sqrt(abs(y))));

    return float4(input.color.xyz, 1) * input.color.w * c;
}

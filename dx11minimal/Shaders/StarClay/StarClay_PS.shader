#include <../lib/constBuf.shader>

Texture2D inputTexture : register(t2);
SamplerState samplerState : register(s2);

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 vpos : POSITION0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 centerPos = gConst[0];
    centerPos = mul(centerPos, mul(view, proj));
    centerPos.xyz /= centerPos.w;

    //float2 screenUV = input.pos.xy / aspect.zw;

    float4 screenPos = input.vpos;
    screenPos.xyz /= screenPos.w;

    screenPos.xy -= centerPos.xy;

    float2 screenUV = (screenPos.xy + float2(1, 1)) / 2;

    float3 tex = inputTexture.Sample(samplerState, screenUV);

    return float4(tex, 1);

    //return gConst[1];
}

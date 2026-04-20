#include <../lib/constBuf.shader>

Texture2D inputTexture : register(t5);
SamplerState samplerState : register(s5);

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = (input.uv + float2(1, 1)) / 2;
    uv.y = 1 - uv.y;

    float4 tex = inputTexture.Sample(samplerState, uv);
    
    return tex * gConst[3];
}

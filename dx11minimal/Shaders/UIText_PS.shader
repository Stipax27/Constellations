Texture2D textAtlas : register(t0);
SamplerState samplerState : register(s0);

cbuffer global : register(b5)
{
    float4 gConst[1024];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv * 0.5f + 0.5f;
    uv.y = 1.0f - uv.y;
    float2 atlasUv = lerp(gConst[4].xy, gConst[4].zw, uv);

    float4 sampleColor = textAtlas.Sample(samplerState, atlasUv);
    float alpha = sampleColor.a;

    return float4(gConst[3].rgb, gConst[3].a * alpha);
}

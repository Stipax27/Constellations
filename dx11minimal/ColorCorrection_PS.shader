struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D renderTexture : register(t0);
SamplerState samplerState : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 color = renderTexture.Sample(samplerState, input.uv).rgb;
    color.b *= 100;

    return float4(1, 1, 1, 1);
}
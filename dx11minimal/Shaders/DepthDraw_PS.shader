Texture2D renderTexture : register(t0);
SamplerState samplerState : register(s0);

Texture2D depthTexture : register(t3);
SamplerState depthSampler : register(s3);

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float depth = depthTexture.Sample(depthSampler, input.uv).r;
    float3 color = renderTexture.Sample(samplerState, input.uv).rgb;

    return float4(color * floor(1 - depth), 1);
}
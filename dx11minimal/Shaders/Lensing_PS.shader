struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

static const float2 center = float2(0.5, 0.5);

Texture2D renderTexture : register(t0);
SamplerState samplerState : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
    float aberrationAmount = 0.02;

    float2 dir = input.uv - center;
    
    float2 uvR = input.uv - dir * aberrationAmount;
    float2 uvB = input.uv + dir * aberrationAmount;

    float4 color;
    color.r = renderTexture.Sample(samplerState, uvR).r;
    color.g = renderTexture.Sample(samplerState, input.uv).g;
    color.b = renderTexture.Sample(samplerState, uvB).b;
    color.a = 1;

    return color;
}
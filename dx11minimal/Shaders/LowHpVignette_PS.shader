cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

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
    float2 uv = input.uv * 2 - 1;

    color.r += pow(max(length(uv) - 0.4, 0), 2) * abs(sin(time.x / 4));

    return float4(color, 1);
}
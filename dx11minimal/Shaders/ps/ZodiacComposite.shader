Texture2D screen : register(t0);
Texture2D screenMid : register(t1);
Texture2D screenLow : register(t2);
SamplerState sam1 : register(s0);

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 c = screen.SampleLevel(sam1, input.uv, 0);
    c += screenMid.SampleLevel(sam1, input.uv, 0);
    c += screenLow.SampleLevel(sam1, input.uv, 0);

    c.rgb = ACESFilm(max(c.rgb, 0.0) * 0.2);
    return float4(c.rgb, 1.0);
}

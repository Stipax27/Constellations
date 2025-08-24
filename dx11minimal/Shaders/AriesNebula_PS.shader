Texture2D perlinTexture : register(t0);
SamplerState perlinSamplerState : register(s0);

cbuffer factors : register(b6)
{
    float AriesNebulaLerpFactor;
};

cbuffer global : register(b5)
{
    float4 gConst[32];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer camera : register(b3)
{
    float4x4 world[2];
    float4x4 view[2];
    float4x4 proj[2];
};

cbuffer drawMat : register(b2)
{
    float4x4 model;
    float hilight;
};

cbuffer params : register(b1)
{
    float r, g, b;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};


float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv;

    float range = 50000;

    float n = saturate(perlinTexture.Sample(perlinSamplerState, input.worldpos.xz / (range * 2) + 0.5).r - 0.3);

    float3 lowerColor = lerp(lerp(float3(1, 0.25, 0.25), float3(1, 0.95, 0.2), n), lerp(float3(0.25, 1, 0.25), float3(1, 0, 1), n), AriesNebulaLerpFactor);
    float3 upperColor = lerp(float3(1, 1, 1), float3(0.8, 0.5, 0.05), AriesNebulaLerpFactor);
    float3 color = lerp(upperColor, lowerColor, max(min(lerp((input.worldpos.y - 3000) / 1500, input.worldpos.y / 1000, AriesNebulaLerpFactor), 1), 0));
    float brightness = exp(-dot(uv, uv) * 20) * 0.1f;

    float offset = max(length(input.worldpos.xz) - 40000, 0);
    float sat = max(1 - offset / 10000, 0);
    brightness *= sat;

    float shine = 1 + 0.4 * sin(log2(input.starID * 0.25) + time.x * -0.1);

    return saturate(float4(color, 1) * float4(brightness, brightness, brightness * 1.4, 1) * shine);
}
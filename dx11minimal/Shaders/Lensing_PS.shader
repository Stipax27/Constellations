Texture2D renderTexture : register(t0);
SamplerState samplerState : register(s0);

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

// cbuffer BlackHoleParams : register(b0)
// {
//     float2 BlackHolePosition;  // Позиция черной дыры в UV координатах (0-1)
//     float SchwarzschildRadius; // Радиус Шварцшильда
//     float DistortionStrength;  // Сила искажения
//     float Time;                // Время для анимации
// }

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 BlackHolePosition = float2(0.5, 0.5);
    float mass = 1000;

    float G = 6.6743 * pow(10, -11);
    float c = 299792458;

    float R_s = (2 * G * mass) / (c * c);
    float A_e = (4 * G * mass) / (c * c);
}
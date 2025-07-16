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

float hash11(uint n) {
    n = (n << 13u) ^ n;
    return frac((n * (n * n * 15731u + 789221u) + 1376312589u) * 0.000000000931322574615478515625f);
}

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};


float3 HSLToRGB(float3 hsl) {
    float h = hsl.x; // Hue [0, 1]
    float s = hsl.y; // Saturation [0, 1]
    float l = hsl.z; // Lightness [0, 1]

    float c = (1 - abs(2 * l - 1)) * s; // Chroma
    float x = c * (1 - abs(fmod(h * 6, 2) - 1));
    float m = l - c * 0.5;

    float3 rgb;
    if (h < 1.0 / 6.0) {
        rgb = float3(c, x, 0);
    } else if (h < 2.0 / 6.0) {
        rgb = float3(x, c, 0);
    } else if (h < 3.0 / 6.0) {
        rgb = float3(0, c, x);
    } else if (h < 4.0 / 6.0) {
        rgb = float3(0, x, c);
    } else if (h < 5.0 / 6.0) {
        rgb = float3(x, 0, c);
    } else {
        rgb = float3(c, 0, x);
    }

    return rgb + m;
}

float3 ApplyRainbowEffect() {
    // Ќормализуем врем€ в диапазон [0, 1] дл€ Hue
    float hue = frac(time * 0.005); // frac аналогичен %1.0
    float saturation = 1.0;
    float lightness = 0.5;

    float3 hsl = float3(hue, saturation, lightness);
    return HSLToRGB(hsl);
}


float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv;
    float3 color = ApplyRainbowEffect();
    float brightness = exp(-dot(uv, uv) * 20) * 0.01f;

    return float4(color, 1) * float4(brightness, brightness, brightness * 1.4, 1) * (1 + 0.9 * sin(input.starID * 1.2 + time.x * 0.1));
}
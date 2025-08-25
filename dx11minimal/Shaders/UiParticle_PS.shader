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
    float hue = frac(time * 0.01);
    float saturation = 1.0;
    float lightness = 0.5;

    float3 hsl = float3(hue, saturation, lightness);
    return HSLToRGB(hsl);
}


float3 rotZ(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), -sin(a),0,
        sin(a), cos(a), 0,
        0, 0, 1
    };
    pos = mul(pos, m);
    return pos;
}

float star(float2 uv)
{
    float c = saturate(1. - 1. * length(uv));
    c = pow(c, 3);
    c *= saturate(1. - 228. * abs(uv.x) * abs(uv.y));
    c += pow(max(sin(length(uv * 3.14)), 0), 118) * .03;
    return c;
}


float4 PS(VS_OUTPUT input) : SV_Target
{
    float c = star(input.uv);
    c += star(rotZ(float3(input.uv,0),45*3.14/180).xy*1.5);

    return float4(c, c, c, c);

}

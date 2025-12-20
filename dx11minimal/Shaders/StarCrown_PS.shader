cbuffer global : register(b5)
{
    float4 gConst[256];
};

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

#define PI 3.14159265358979323846

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 basecolor = float4(1.0, 0.95, 0.3, 1.0);
    float4 color;

    float angle = atan2(input.uv.y, input.uv.x) + PI;
    float deg = degrees(angle);

    float fading = 1 - length(input.uv);

    color = basecolor * (pow(fading, 0.85) - 0.3);
    //color += basecolor * max(sin(deg % 4), 0);

    return saturate(color);

}

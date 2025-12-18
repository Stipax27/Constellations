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
    float angle = atan2(input.uv.y, input.uv.x) + PI;
    float deg = degrees(angle);

    float fading = 1 - length(input.uv);

    float c = max(sin(angle * 64 + time.x), 0) * pow(fading, 2 + abs(sin(time.x * 0.2)) * 0.5);
    c += max(-sin(angle * 48 + time.x * cos(time.x * 0.04) * 0.25), 0) * pow(fading, 2 + abs(sin(time.x * 0.2)) * 0.25);
    c += max(cos(angle * 16 - time.x * 0.5), 0) * pow(fading, 2.5) * (abs(sin(time.x * 0.03)) / 2 + 0.5);

    return saturate(float4(1.0, 0.95, 0.4, 1.0) * c);

}

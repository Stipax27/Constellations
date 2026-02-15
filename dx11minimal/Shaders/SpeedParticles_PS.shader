#include <lib/constBuf.shader>
#include <lib/pi.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

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
    c += pow(sin(length(uv * 3.14)), 118) * .03;
    return c;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
    float lifeAspect = 1 - gConst[0].w;

    //return float4(1, 1, 1, 1);

    float2 uv = input.uv;
    float brightness = exp(-dot(uv, uv) * 20);
    return float4(brightness, brightness, brightness, brightness) * float4(1, 1, 1.4, 1) * saturate(sin(sqrt(lifeAspect * PI) * 1.78)) * 0.75;
}

#include <lib/constBuf.shader>
#include <lib/constants.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 basecolor = gConst[0];
    float4 color;

    //float angle = atan2(input.uv.y, input.uv.x) + PI;
    //float deg = degrees(angle);

    float fading = 1 - length(input.uv);

    color = basecolor * (pow(fading, 0.85) - 0.3);
    //color += basecolor * max(sin(deg % 4), 0);

    return saturate(color * 4.5);

}

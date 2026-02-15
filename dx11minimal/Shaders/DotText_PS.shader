#include <lib/constBuf.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float progress = 1 - gConst[0].w;
    return float4(1, 1, 1, 1) * pow(progress, 2);

}

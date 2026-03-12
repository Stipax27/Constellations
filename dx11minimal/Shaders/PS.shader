#include <lib/constBuf.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    return float4(1, 1, 1, 1.);

}

#include <lib/constBuf.shader>
#include <lib/depthTest.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};


float4 PS(VS_OUTPUT input) : SV_Target
{
    depthTest(input.pos);
    
    //return input.worldpos.x/2000+.5;

    float2 uv = input.uv;
    float brightness = exp(-dot(uv, uv) * 20);

    return float4(brightness, brightness, brightness, 1) * float4(1, 0.6, 0.9, 1) * (1 + 0.9 * sin(input.starID * 1.2 + time.x * 0.1));
}
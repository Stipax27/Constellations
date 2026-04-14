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

    float3 colors[4] = {
        float3(1, 0.6, 0.9),
        float3(1, 0.6, 0.6),
        float3(0.38, 1, 0.59),
        float3(1, 1, 1)
    };
    
    //return input.worldpos.x/2000+.5;

    float2 uv = input.uv;
    float brightness = exp(-dot(uv, uv) * 20);
    float3 color = colors[input.starID % 4];

    return float4(brightness, brightness, brightness, 1) * float4(color.xyz, 1) * (1 + 0.9 * sin(input.starID * 1.2 + time.x * 0.1));
}
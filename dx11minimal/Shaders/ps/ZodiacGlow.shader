#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>
#include <../lib/depthTest.shader>

float4 PS(VS_OUTPUT_PARTICLE input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    depthTest(input.pos);

    float3 c = saturate(1.0 - 2.0 * length(input.uv - 0.5));
    return float4(c * input.color.rgb, 1.0);
}

#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>
#include <../lib/depthTest.shader>

float4 PS(VS_OUTPUT_PARTICLE input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    depthTest(input.pos);

    float3 c = saturate(1.0 - 2.0 * length(input.uv - 0.5));

    float2 uv = 2.0 * (input.uv - 0.5);
    float2 suv = abs(uv);
    float d = 2.0 / ((length(uv.xy - uv.yx) + length(uv.xy + uv.yx)) / 2.0);
    d += 2.0 / (suv.x + suv.y);
    d *= saturate(1.0 - max(suv.x, suv.y));

    if (length(input.size) <= 1.1)
    {
        return float4(15.0 * input.color.rgb / 9.0, 1.0);
    }

    return float4(d * input.color.rgb / 9.0, 1.0);
}

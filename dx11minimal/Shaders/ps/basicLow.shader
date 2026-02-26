#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>


float4 PS(VS_OUTPUT_PARTICLE input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    float3 c = saturate(1.-2.*length(input.uv-.5));
    float4 color = float4(c * input.color.rgb, 1);

    float dist = max(length(input.wpos.xyz - cPos.xyz), 1);
    color /= dist;

    return color;
}

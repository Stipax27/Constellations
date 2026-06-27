#include <../lib/constBuf.shader>
#include <../lib/io.shader>
#include <../lib/utils.shader>


float4 PS(VS_OUTPUT_PARTICLE input, bool isFrontFace : SV_IsFrontFace) : SV_Target
{
    float3 c = saturate(1.-2.*length(input.uv-.5));

    float2 uv=2*(input.uv-.5);
    float2 suv =abs(uv);
    float d=2./((length(uv.xy-uv.yx)+length(uv.xy+uv.yx))/2);
          d+=2./(suv.x+suv.y);
    d*=saturate(1-max(suv.x,suv.y));
    //  d+=c*3;

    float4 color = float4(d*input.color.rgb,1);
    if (length(input.size)<=1.1) color = float4(15*input.color.rgb,1);

    float dist = max(length(input.wpos.xyz - cPos.xyz), 1);
    color /= dist;

    return color;
}

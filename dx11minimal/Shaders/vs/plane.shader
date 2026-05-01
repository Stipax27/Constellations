#include <../lib/constBuf.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;

    float2 quadUV[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };
    float2 p = quadUV[vID];
    float4 pos = float4(p, 0, 1);

    pos = mul(pos, world);
    float4 vpos = mul(pos, view);
    vpos = mul(vpos, proj);

    output.pos = vpos;
    output.uv = p;

    return output;
}

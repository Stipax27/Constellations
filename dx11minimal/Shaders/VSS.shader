#include <lib/constBuf.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output;

    float4 pos = float4(gConst[iID].xyz, 1);
    float sz = gConst[iID].w;

    float2 quadUV[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    //pos.xy += quadUV[vID] * sz;
        
    float4 viewPos = mul(pos, view);
    float4 projPos = mul(viewPos, proj);

    projPos.xy += quadUV[vID] * float2(aspect.x, 1) * sz;

    output.uv = quadUV[vID];
    output.pos = projPos;

    return output;
}
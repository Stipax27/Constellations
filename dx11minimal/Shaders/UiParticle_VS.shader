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

    float2 mousepos = gConst[iID].xy;
    float angle = gConst[iID].z;
    float lifetime = gConst[iID].w;
    float size = 0.025 * lifetime;

    float2 offset = float2(sin(angle) * aspect.x, cos(angle)) * sqrt(1 - lifetime) * 0.15;

    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    float2 pos = quadPos[vID];

    float2 screenPos = mul(float4(quadPos[vID], 0.0, 1.0), proj).xy;
    screenPos = float2(mousepos.x + screenPos.x * size, mousepos.y - screenPos.y * size) + offset;

    output.pos = float4(screenPos.xy, 0, 1);
    output.uv = quadPos[vID];

    return output;
}
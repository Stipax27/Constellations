#include <lib/constBuf.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   iID : COLOR0;
    float4 wpos : POSITION1;
};


VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output;

    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(-1, 1), float2(1, -1), float2(1, 1),
    };

    float localTime = drawerV[0];
    float t = localTime - fConst[iID]._m30;
    float lifetime = t / pLifetime;

    float size = lerp(pSize.x, pSize.y, lifetime);
    float speed = 10;

    float3 pos = fConst[iID]._m00_m10_m20;
    float3 direction = fConst[iID]._m01_m11_m21;

    float realtime = t * 0.001;
    float a = (pSpeed.y - pSpeed.x) / (pLifetime * 0.001);
    float s = pSpeed.x * realtime + a * pow(realtime, 2) / 2;
    pos += direction * s;

    //float2 offset = float2(sin(angle) * aspect.x, cos(angle)) * sqrt(1 - lifetime) * 0.15;

    float4 projPos = mul(float4(pos, 1), mul(view, proj));
    projPos.xy += quadPos[vID] * float2(aspect.x, 1) * size;

    output.pos = projPos;
    output.uv = quadPos[vID];
    output.iID = iID;
    output.wpos = float4(pos, 0);

    return output;
}
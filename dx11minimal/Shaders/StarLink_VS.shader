#include <lib/constBuf.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

float3 rotZ(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), -sin(a),0,
        sin(a), cos(a), 0,
        0, 0, 1
    };
    pos = mul(pos, m);
    return pos;
}

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output;

    float4 p1 = fConst[iID]._m00_m10_m20_m30;
    float4 p2 = fConst[iID]._m01_m11_m21_m31;

    float4 pointsProj[] = {
        mul(mul(float4(p1.xyz, 1.0f), view), proj),
        mul(mul(float4(p2.xyz, 1.0f), view), proj)
    };

    float4 direction = pointsProj[1] - pointsProj[0];
    float2 perpendicular = normalize(float2(direction.y, -direction.x) * float2(aspect.x, 1));

    float2 quadUV[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(-1, 1), float2(1, 1)
    };

    float4 poses[2] = {p1, p2};
    float4 pos = poses[vID % 2];

    float4 viewPos = mul(float4(pos.xyz, 1.0f), view);
    float4 projPos = mul(viewPos, proj);

    projPos.xy += perpendicular * quadUV[vID].y * pos.w * float2(aspect.x, 1);

    output.uv = quadUV[vID];
    output.pos = projPos;

    float4 colors[2] = {fConst[iID]._m02_m12_m22_m32, fConst[iID]._m03_m13_m23_m33};
    output.color = colors[vID % 2];

    return output;
}
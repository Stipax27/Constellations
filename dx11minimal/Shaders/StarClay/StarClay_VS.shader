#include <../lib/constBuf.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 vpos : POSITION0;
    float4 projPos : POSITION1;
    float3 vnorm : NORMAL0;
};

float3 ball(float2 p)
{
    float n = (float)drawerV[0];

    p.x = (p.x / n) * 3.141592653589793;
    p.y = (p.y / n) * 3.141592653589793 / 2;

    float3 pos = float3(cos(p.x) * cos(p.y), sin(p.y), sin(p.x) * cos(p.y));

    return pos;
}

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output;

    uint n = drawerV[0];
    uint quadID = vID / 6;

    float row = quadID % n;
    float col = quadID / n;

    float2 quadUV[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };
    float2 p = quadUV[vID % 6];

    float4 pos = float4(p, 0, 1);
    pos.y += col * 2;
    pos.x += row * 2;
    pos.xy -= (float)n - 1;

    pos.xyz = ball(pos.xy);

    output.vnorm = normalize(pos);

    pos = mul(pos, model[iID]);

    float4 vpos = mul(pos, view);
    float4 projPos = mul(vpos, proj);

    output.pos = projPos;
    output.projPos = projPos;
    output.vpos = vpos;
    output.uv = float2(1, -1) * p / 2. + .5;

    return output;
}
#include <lib/constBuf.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};

float3 ball(float2 p, float radius)
{
    float n = (float)drawConst[0];

    p.x = (p.x / n) * 3.141592653589793;
    p.y = (p.y / n) * 3.141592653589793 / 2;

    float3 pos = float3(cos(p.x) * cos(p.y) * radius, sin(p.y) * radius, sin(p.x) * cos(p.y) * radius);

    return pos;
}

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output;
    float3 worldPos = float3(gConst[0].xyz);

    uint n = drawConst[0];
    uint instanceID = vID / 6;

    float row = instanceID % n;
    float col = instanceID / n;

    float2 quadUV[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };
    float2 p = quadUV[vID % 6];

    float4 pos = float4(p, 0, 1);
    pos.y += col * 2;
    pos.x += row * 2;
    pos.xy -= (float)n - 1;

    pos.xyz = ball(pos.xy, gConst[0].w) + worldPos;

    output.pos = mul(pos, mul(view, proj));
    output.uv = float2(1, -1) * p / 2. + .5;

    return output;
}
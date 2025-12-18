cbuffer global : register(b5)
{
    float4 gConst[256];
};

cbuffer camera : register(b3)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};

cbuffer drawerV : register(b0)
{
    float drawConst[256];
}

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
};

float3 ball(float2 p, float radius)
{
    float n = (float)drawConst[0];

    p.x = -(p.x / n) * 3.141592653589793;
    p.y = (p.y / n) * 3.141592653589793 / 2;

    float3 pos = float3(cos(p.x) * cos(p.y) * radius, sin(p.y) * radius, sin(p.x) * cos(p.y) * radius);

    return pos;
}

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output;

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

    output.uv = float2(1, -1) * pos.xy / 2. + .5;

    pos.xyz = ball(pos.xy, 1);

    output.vpos = pos;

    pos = mul(pos, world);

    output.pos = mul(pos, mul(view, proj));
    output.wpos = float4(pos.xyz, 0);

    return output;
}
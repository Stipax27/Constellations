cbuffer global : register(b5)
{
    float4 gConst[32];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer camera : register(b3)
{
    float4x4 world[2];
    float4x4 view[2];
    float4x4 proj[2];
};

cbuffer drawMat : register(b2)
{
    float4x4 model;
    float hilight;
};

cbuffer objParams : register(b0)
{
    float drawerV[32];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;

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

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output;

    float lifeAspect = 1 - gConst[0].w;
    float sz = 0.02;
    float len = length(gConst[2]) * 1000;

    float4 p1 = gConst[0];
    float4 p2 = p1 + normalize(gConst[2]) * (len * (lifeAspect + 0.5));

    float4 pointsProj[] = {
        mul(mul(float4(p1.xyz, 1), view[0]), proj[0]),
        mul(mul(float4(p2.xyz, 1), view[0]), proj[0])
    };
    pointsProj[0].xy /= max(pointsProj[0].w, 0);
    pointsProj[1].xy /= max(pointsProj[1].w, 0);

    float2 quadUV[6] = { //If I won't understand it in month I'll give 100 rubles to Nikita
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(-1, 1), float2(1, 1)
    };

    float2 direction = pointsProj[1].xy - pointsProj[0].xy;
    float2 perpendicular = normalize(float2(direction.y, -direction.x) * float2(aspect.x, 1));

    float2 pos = pointsProj[vID % 2];

    pos += perpendicular * quadUV[vID].y * float2(aspect.x, 1) * sz;

    output.uv = quadUV[vID];
    output.pos = float4(pos, 0, 1);

    return output;
}
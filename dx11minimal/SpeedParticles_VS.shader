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

    float lifeAspect = gConst[0].w;
    float sz = 150;

    float4 p1 = gConst[0];
    float4 p2 = p1 - gConst[2] * 500 * lifeAspect;

    float4 pointsProj[] = {
        mul(mul(float4(p1.xyz, 1), view[0]), proj[0]),
        mul(mul(float4(p2.xyz, 1), view[0]), proj[0])
    };

    float4 direction = pointsProj[1] - pointsProj[0];
    float2 perpendicular = normalize(float2(direction.y, -direction.x));

    float2 quadUV[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(-1, 1), float2(1, 1)
    };

    float4 pos = pointsProj[vID % 2];
    
   // float4 viewPos = mul(float4(gConst[0].xyz, 1.0f), view[0]);
    //float4 projPos = mul(viewPos, proj[0]);

    pos.xy += perpendicular * quadUV[vID].y * gConst[0].w * float2(aspect.x, 1) * sz;

    output.uv = quadUV[vID];
    output.pos = pos;

    return output;
}
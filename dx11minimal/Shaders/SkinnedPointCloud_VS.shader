cbuffer BoneBuffer : register(b1)
{
    float4x4 gBones[128];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer camera : register(b3)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float4 cPos;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
    uint4  boneIds  : BLENDINDICES;
    float4 weights  : BLENDWEIGHT;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
    float4 vnorm : NORMAL0;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    uint4 ids = min(input.boneIds, 127);

    float4 skinnedPos =
        mul(float4(input.position, 1), gBones[ids.x]) * input.weights.x +
        mul(float4(input.position, 1), gBones[ids.y]) * input.weights.y +
        mul(float4(input.position, 1), gBones[ids.z]) * input.weights.z +
        mul(float4(input.position, 1), gBones[ids.w]) * input.weights.w;

    float3 skinnedNormal =
        mul(input.normal, (float3x3)gBones[ids.x]) * input.weights.x +
        mul(input.normal, (float3x3)gBones[ids.y]) * input.weights.y +
        mul(input.normal, (float3x3)gBones[ids.z]) * input.weights.z +
        mul(input.normal, (float3x3)gBones[ids.w]) * input.weights.w;

    skinnedNormal = normalize(skinnedNormal);

    float4 pos = mul(skinnedPos, world);

    output.pos = mul(pos, mul(view, proj));
    output.vpos = mul(float4(pos.xyz, 1), view);
    output.wpos = float4(pos.xyz, 0);
    output.vnorm = float4(skinnedNormal, 1);
    output.uv = input.uv;

    return output;
}

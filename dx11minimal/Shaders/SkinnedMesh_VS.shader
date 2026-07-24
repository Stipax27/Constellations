#include <lib/structBuf.shader>

cbuffer CameraBuffer : register(b3)
{
    float4x4 view;
    float4x4 proj;
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
    float4 pos    : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv     : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT o;

    // Clamp bone indices to avoid out-of-range access.
    uint4 ids = min(input.boneIds, 127);

    // ----- Skin Position -----

    float4 skinnedPos =
        mul(float4(input.position,1), boneMatrices[ids.x]) * input.weights.x +
        mul(float4(input.position,1), boneMatrices[ids.y]) * input.weights.y +
        mul(float4(input.position,1), boneMatrices[ids.z]) * input.weights.z +
        mul(float4(input.position,1), boneMatrices[ids.w]) * input.weights.w;

    // ----- Skin Normal -----

    float3 skinnedNormal =
        mul(input.normal, (float3x3)boneMatrices[ids.x]) * input.weights.x +
        mul(input.normal, (float3x3)boneMatrices[ids.y]) * input.weights.y +
        mul(input.normal, (float3x3)boneMatrices[ids.z]) * input.weights.z +
        mul(input.normal, (float3x3)boneMatrices[ids.w]) * input.weights.w;

    skinnedNormal = normalize(skinnedNormal);

    // World -> View -> Projection.

    float4 worldPos = mul(skinnedPos, modelMatrices[0]);
    float4 viewPos  = mul(worldPos, view);
    o.pos           = mul(viewPos, proj);

    o.normal = normalize(mul(skinnedNormal, (float3x3)modelMatrices[0]));
    o.uv     = input.uv;

    return o;
}

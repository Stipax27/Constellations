#include <../lib/constBuf.shader>

Texture2D inputTexture : register(t4);
SamplerState samplerState : register(s4);

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 vpos : POSITION0;
    float4 projPos : POSITION1;
    float3 vnorm : NORMAL0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 centerPos = gConst[0];
    float4 vCenterPos = mul(float4(centerPos.xyz, 1), view);
    float4 pCenterPos = mul(vCenterPos, proj);
    pCenterPos.xyz /= pCenterPos.w;

    float3 vec = normalize(input.vpos.xzy);
    vec.z *= -1;

    float c = 1 - saturate(cross(input.vnorm, vec));
    c *= c * c * 0.4;

    float3 shapeColor = float3(0.92, 0.59, 1) * c;

    float4 screenPos = input.projPos;
    screenPos.xyz /= screenPos.w;

    screenPos.xy -= pCenterPos.xy;
    screenPos.y *= -1;

    float2 screenUV = (screenPos.xy + float2(1, 1)) / 2;

    float3 tex = inputTexture.Sample(samplerState, screenUV);


    return float4(tex + shapeColor, 1);


    //return float4(tex, 1) + float4(shapeColor, 1);
}

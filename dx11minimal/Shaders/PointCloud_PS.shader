#include <lib/constBuf.shader>

Texture2D<float> DepthTexture : register(t0);
SamplerState DepthSampler : register(s0);

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
    float4 vnorm : NORMAL0;
    float2 uv : TEXCOORD0;
};


float4 PS(VS_OUTPUT input) : SV_Target
{
    // DEPTH TEST //

    if (drawInt[0] > 1)
    {
        uint width, height;
        DepthTexture.GetDimensions(width, height);

        float2 halfResUV = input.pos.xy / float2(width, height);
        float sceneDepth = DepthTexture.Sample(DepthSampler, halfResUV);
        //float bias = 0.005 * (1 - input.depth);
    
        if (input.pos.z > sceneDepth)
        {
            discard;
        }
    }

    ////////////////

    float2 uv = input.uv;
    float brightness = exp(-dot(uv, uv) * 20) * gConst[0].w;

    float dist = length(cPos.xyz - input.wpos);
    float sat = min(max(dist - 0.5, 0) / 2, 1);
    //sat *= 1 - clamp(abs(dot(float3(-1, 0, 0), input.vnorm)), 0, 1);
    brightness *= sat;

    return float4(brightness, brightness, brightness, 1) * float4(gConst[0].xyz, 1) * 0.015;
}
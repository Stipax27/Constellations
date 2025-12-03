Texture2D<float> depthTexture : register(t0);
SamplerState depthSampler : register(s0);

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float PS(VS_OUTPUT input) : SV_TARGET
{
    float width, height;

    depthTexture.GetDimensions(width, height);

    float2 srcSize = float2(width, height);
    float2 srcUV = input.uv * srcSize;
    
    float2 baseUV = floor(srcUV) / srcSize;
    float2 stepUV = 1.0f / srcSize;

    float maxDepth = 0.0f;
    [unroll]
    for (int i = 0; i < 2; ++i)
    {
        [unroll]
        for (int j = 0; j < 2; ++j)
        {
            float depth = depthTexture.Sample(depthSampler, baseUV + float2(i, j) * stepUV);
            maxDepth = max(maxDepth, depth);
        }
    }
    
    return maxDepth;
}
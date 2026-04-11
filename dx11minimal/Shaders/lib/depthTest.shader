Texture2D<float> DepthTexture : register(t0);
SamplerState DepthSampler : register(s0);

void depthTest(float4 pos)
{
	if (drawInt[0] > 1)
    {
        uint width, height;
        DepthTexture.GetDimensions(width, height);

        float2 halfResUV = pos.xy / float2(width, height);
        float sceneDepth = DepthTexture.Sample(DepthSampler, halfResUV);
    
        if (pos.z > sceneDepth)
        {
            discard;
        }
    }
}

void depthTestAbs(float4 pos)
{
	uint width, height;
    DepthTexture.GetDimensions(width, height);

    float2 halfResUV = pos.xy / float2(width, height);
    float sceneDepth = DepthTexture.Sample(DepthSampler, halfResUV);
    
    if (sceneDepth < 1.0)
    {
        discard;
    }
}
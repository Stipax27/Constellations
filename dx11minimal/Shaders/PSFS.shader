Texture2D<float> DepthTexture : register(t0);
SamplerState DepthSampler : register(s0);

cbuffer global : register(b5)
{
    float4 gConst[256];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer drawerInt : register(b7)
{
    int drawInt[256];
}

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
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
    
    //return input.worldpos.x/2000+.5;

    float2 uv = input.uv;
    float brightness = exp(-dot(uv, uv) * 20);

    return float4(brightness, brightness, brightness, 1) * float4(1, 0.6, 0.9, 1) * (1 + 0.9 * sin(input.starID * 1.2 + time.x * 0.1));
}
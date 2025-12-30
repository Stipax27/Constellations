Texture2D<float> DepthTexture : register(t0);
SamplerState DepthSampler : register(s0);

Texture2D perlinTexture : register(t1);
SamplerState perlinSamplerState : register(s1);

cbuffer factors : register(b6)
{
    float AriesNebulaLerpFactor;
};

cbuffer global : register(b5)
{
    float4 gConst[256];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer objParams : register(b0)
{
    float drawerV[256];
};

cbuffer drawerInt : register(b7)
{
    int drawInt[256];
}

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 worldpos : POSITION0;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
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

    //return float4(1, 1, 1, 1);

    float2 uv = input.uv;

    float3 pos = gConst[0].xyz;
    float scale = gConst[0].w;
    float timeScale = drawerV[0];

    float range = 65 * scale;

    float2 posXZ = (input.worldpos.xz - pos.xz) / scale;
    float posY = (input.worldpos.y - pos.y) / scale;

    float n = saturate(perlinTexture.Sample(perlinSamplerState, posXZ / (range * 2) + 0.5).r - 0.25);

    float3 lowerColor = lerp(lerp(float3(1, 0.25, 0.25), float3(1, 0.95, 0.1), n - 0.2), lerp(float3(0.25, 1, 0.25), float3(1, 0, 1), n), AriesNebulaLerpFactor);
    float3 upperColor = lerp(float3(1, 1, 1), float3(0.8, 0.5, 0.05), AriesNebulaLerpFactor);
    float3 color = lerp(lowerColor, upperColor, max(min(lerp((posY + 10) / 6 * scale, posY / 4 * scale, AriesNebulaLerpFactor), 1), 0));
    float brightness = exp(-dot(uv, uv) * 20) * 0.025f;

    float offset = max(length(posXZ) - 40, 0);
    float sat = max(1 - offset / 20, 0);
    brightness *= sat;

    float shine = 1 + 0.3 * sin(log2(input.starID) * 3 + time.x * -0.25 * timeScale);

    return saturate(float4(color, 1) * float4(brightness, brightness, brightness * 1.4, 1) * shine);
}
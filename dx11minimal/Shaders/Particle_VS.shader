cbuffer global : register(b5)
{
    float4 gConst[1024];
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

cbuffer particlesDesc : register(b9)
{
	float2 pSize;
	float2 pOpacity;
	float3 pColor;
	float pLifetime;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   iID : COLOR0;
    float4 worldpos : POSITION1;
};


VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output;

    float3 pos = gConst[iID].xyz;
    float lifetime = (time.x * 100 - gConst[iID].w) / pLifetime;

    float size = lerp(pSize.x, pSize.y, lifetime);

    //float2 offset = float2(sin(angle) * aspect.x, cos(angle)) * sqrt(1 - lifetime) * 0.15;

    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(-1, 1), float2(1, -1), float2(1, 1),
    };

    float4 projPos = mul(float4(pos, 1), mul(view, proj));
    projPos.xy += quadPos[vID] * float2(aspect.x, 1) * size;

    output.pos = projPos;
    output.uv = quadPos[vID];
    output.iID = iID;

    return output;
}
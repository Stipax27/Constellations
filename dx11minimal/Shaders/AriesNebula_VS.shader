#include <lib/constBuf.shader>
#include <lib/constants.shader>

Texture2D perlinTexture : register(t1);
SamplerState perlinSamplerState : register(s1);

Texture2D voronoiTexture : register(t2);
SamplerState voronoiSamplerState : register(s2);

uint getPointLevel(uint index) {
    if (index < 3) {
        return 1;
    }

    float d = 9 * 9 - 4 * 3 * (4 - 2 * index);
    return ceil((-9 + sqrt(d)) / (2 * 3));
}

uint getSectorsOfLevel(uint level) {
    return 3 * (level + 1);
}

float3 spiral(uint index) {
    uint level = getPointLevel(index);
    uint sectors = getSectorsOfLevel(level);
    uint localIndex = index - getSectorsOfLevel(level - 1);

    float angle = PI * 2 / sectors * localIndex;
    float radius = 0.25 * level / 6 * gConst[0].w * 2;

    return float3(sin(angle), 0, cos(angle)) * radius;
}


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 wpos : POSITION0;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
};

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output;

    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    //calc star position

    float3 pos = gConst[0].xyz;
    float scale = gConst[0].w;
    float localTime = drawerV[0];

    float range = 65 * scale;
    float size = 1 * scale;
    //starPos = spiral(iID) * range * 2 - range;
    float3 spiralPos = spiral(iID);
    float3 starPos = spiralPos + pos;

    float par = spiralPos.x * spiralPos.x + spiralPos.z * spiralPos.z;
    starPos.y = cos(spiralPos.x / 200 * PI) * cos(spiralPos.z / 200 * PI) * 15 - (par * lerp(24, 16, 0)) / (lerp(50, 40, AriesNebulaLerpFactor) + par) + pos.y;
    
    starPos.y -= perlinTexture.SampleLevel(perlinSamplerState, spiralPos.xz / (range * 2) + 0.5, 1).r * lerp(10, 8, AriesNebulaLerpFactor);

    float3 voronoi = voronoiTexture.SampleLevel(voronoiSamplerState, spiralPos.xz / (range * 2) + 0.5, 1);
    starPos.y += voronoi.x * voronoi.y * voronoi.z * lerp(70, 15, AriesNebulaLerpFactor);

    starPos.y += (sin(spiralPos.x * PI / range * 5 + localTime * 0.05) + cos(spiralPos.z * PI / range * 5 + localTime * 0.05)) * lerp(0.4, 0.2, AriesNebulaLerpFactor);
    starPos.y += cos((spiralPos.x + spiralPos.z) * PI / range * 2 + localTime * -0.05) * lerp(0.8, 0.4, AriesNebulaLerpFactor);

    starPos.y *= scale;

    // //-----

    float4 viewPos = mul(float4(starPos, 1.0f), view);
    float4 projPos = mul(viewPos, proj);
    //projPos.xy /= max(projPos.w, 0);
    //projPos.xy += quadPos[vID] * float2(aspect.x, 1) * size;

    //projPos.xy += quadPos[vID] * float2(aspect.x, 1) * size;

    output.uv = quadPos[vID];
    output.pos = projPos;
    output.starID = iID;
    output.wpos = float4(starPos, 0);

    return output;
}
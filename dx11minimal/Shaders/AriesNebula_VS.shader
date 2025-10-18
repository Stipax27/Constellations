Texture2D perlinTexture : register(t0);
SamplerState perlinSamplerState : register(s0);

Texture2D voronoiTexture : register(t1);
SamplerState voronoiSamplerState : register(s1);

cbuffer factors : register(b6)
{
    float AriesNebulaLerpFactor;
};

cbuffer global : register(b5)
{
    float4 gConst[32];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer camera : register(b3)
{
    //float4x4 world[2];
    float4x4 view;
    float4x4 proj;
};

cbuffer drawMat : register(b2)
{
    float4x4 model;
    float hilight;
};

cbuffer objParams : register(b0)
{
    float drawerV[32];
};

#define PI 3.1415926535897932384626433832795


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
    float radius = 400 * level / 6;

    return float3(sin(angle), 0, cos(angle)) * radius;
}


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output;

    uint vertexInQuad = vID % 6;

    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    //calc star position

    float range = 50000;
    float size = 800;
    //starPos = spiral(iID) * range * 2 - range;
    float3 starPos = spiral(iID);
    //starPos.y = 1000;

    float par = starPos.x * starPos.x + starPos.z * starPos.z;
    starPos.y = cos(starPos.x / 100000 * PI) * cos(starPos.z / 100000 * PI) * -7500 + (par * lerp(16000, 12000, AriesNebulaLerpFactor)) / (lerp(60000000, 50000000, AriesNebulaLerpFactor) + par);
    
    starPos.y += perlinTexture.SampleLevel(perlinSamplerState, starPos.xz / (range * 2) + 0.5, 1).r * lerp(5000, 4000, AriesNebulaLerpFactor);

    float3 voronoi = voronoiTexture.SampleLevel(voronoiSamplerState, starPos.xz / (range * 2) + 0.5, 1);
    starPos.y -= voronoi.x * voronoi.y * voronoi.z * lerp(35000, 7500, AriesNebulaLerpFactor);

    starPos.y += (sin(starPos.x * PI / range * 5 + time.x * 0.05) + cos(starPos.z * PI / range * 5 + time.x * 0.05)) * lerp(300, 150, AriesNebulaLerpFactor);
    starPos.y += cos((starPos.x + starPos.z) * PI / range * 2 + time.x * -0.05) * lerp(600, 300, AriesNebulaLerpFactor);

    // //-----

    float4 viewPos = mul(float4(starPos, 1.0f), view);
    float4 projPos = mul(viewPos, proj);
    //projPos.xy /= max(projPos.w, 0);
    projPos.xy += quadPos[vertexInQuad] * float2(aspect.x, 1) * size;

    output.uv = quadPos[vertexInQuad];
    output.pos = projPos;
    output.starID = iID;
    output.worldpos = float4(starPos, 1);
    return output;
}
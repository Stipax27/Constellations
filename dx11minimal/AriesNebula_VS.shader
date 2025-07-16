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
    float4x4 world[2];
    float4x4 view[2];
    float4x4 proj[2];
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

float hash11(uint n) {
    n = (n << 13u) ^ n;
    return frac((n * (n * n * 15731u + 789221u) + 1376312589u) * 0.000000000931322574615478515625f)*2147483647;
}

float hash2(uint n) {
    n = (n << 13u) ^ n;
    return frac((n * (n * n * 15731u + 789221u) + 1376312589u) * 0.000000000931322574615478515625f);
}

float3 randomPosition(uint index) {
    float x = hash2(index * 3u);
    float y = hash2(index * 3u + 1u);
    float z = hash2(index * 3u + 2u);
    return float3(x, y, z);
}

#define PI 3.1415926535897932384626433832795

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};

float3 rotX(float3 pos,float a)
{
    float3x3 m =
    {
        1, 0,       0,
        0, cos(a), -sin(a),
        0, sin(a), cos(a)
    };
    pos = mul(pos, m);
    return pos;
}

float3 rotY(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), 0, sin(a),
        0, 1, 0,
        -sin(a), 0, cos(a)
    };
    pos = mul(pos, m);
    return pos;
}

float3 rotZ(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), -sin(a),0,
        sin(a), cos(a), 0,
        0, 0, 1
    };
    pos = mul(pos, m);
    return pos;
}

float hash( float n ) {
    return frac(sin(n)*43758.5453);
}
     
float noise( float3 x ) {
    float3 p = floor(x);
    float3 f = frac(x);
     
    f = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + 113.0*p.z;
     
    float a = lerp(lerp(lerp( hash(n+0.0), hash(n+1.0),f.x),
            lerp( hash(n+57.0), hash(n+58.0),f.x),f.y),
            lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
            lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);

    return a - 0.5;
}

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output;

    uint starID = vID / 6;
    uint vertexInQuad = vID % 6;

    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    //calc star position

    float size = 128;
    float range = 2000.;
    float3 starPos = randomPosition(starID)*range*2-range;
    
    float diskRadius = 1000.f;
    float coreRadius = 80.f;
    float thickness = 140.f;
    float armsCount = 2.f;
    float armWidth = 0.5f;
    float rotationSpeed = 0.5f;
    float rotationAngle = time.x * rotationSpeed * 0.01;
    float armRadius = 90.;

    float dh = hash2(starID*12);
    float distance = coreRadius * 0 + dh *(diskRadius - coreRadius);
    float height = hash2(starID*22) *thickness - thickness / 2;

    float armAngle = (starID) % armsCount * armRadius*2;
        
    float spiralFactor = 3.5f * log(distance / coreRadius + 1);
    float angle = armAngle + spiralFactor * (distance / diskRadius) * 360;
    
    float randomOffset = hash2(starID)* armWidth * distance;
    angle += randomOffset;
    
    
    starPos.x = noise(starPos.xyz * 0.083 * 100 * 0.00007 + float3(53.529, 31.611, 59.317)) * 10000;
    starPos.y = noise(starPos.yxz * 0.131 * 100 * 0.00011 + float3(65.235, 14.631, 41.547)) * 10000;
    starPos.z = noise(starPos.zxy * 0.157 * 100 * 0.00009 + float3(27.153, 63.721, 12.591)) * 10000;

    starPos.z += 400;  


    //-----
    float4 viewPos = mul(float4(starPos, 1.0f), view[0]);
    float4 projPos = mul(viewPos, proj[0]);
    projPos.xy += quadPos[vertexInQuad] * float2(aspect.x, 1) * size;
    output.uv = quadPos[vertexInQuad];
    output.pos = projPos;
    output.starID = starID;
    output.worldpos = float4(starPos, 1);
    return output;
}
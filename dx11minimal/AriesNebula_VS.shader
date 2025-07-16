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
    // The noise function returns a value in the range -1.0f -> 1.0f
    float3 p = floor(x);
    float3 f = frac(x);
     
    f = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + 113.0*p.z;
     
    float a= lerp(lerp(lerp( hash(n+0.0), hash(n+1.0),f.x),
            lerp( hash(n+57.0), hash(n+58.0),f.x),f.y),
            lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
            lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);

    return a-.5;
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

    float size = 7;
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
    float distance = coreRadius*0 + dh *(diskRadius - coreRadius);
    float height = hash2(starID*22) *thickness - thickness / 2;

    float armAngle = (starID) % armsCount * armRadius*2;
        
    float spiralFactor = 3.5f * log(distance / coreRadius + 1);
    float angle = armAngle + spiralFactor * (distance / diskRadius) * 360;
    
    float randomOffset = hash2(starID)* armWidth * distance;
    angle += randomOffset;
    

    starPos.x = distance * cos(angle * PI / 180.);
    starPos.y = distance * sin(angle * PI / 180.);
    starPos.z = height*(1-dh);
    
    float3 corePos;
   //if (hash2(starID) <.5) {

    
     
     float coreDistance = hash2(starID*32) * coreRadius; 
     float coreAngle = hash2(starID*30) * 360.;
     corePos.x = coreDistance * cos(coreAngle * PI / 180.);
     corePos.y = coreDistance * sin(coreAngle * PI / 180.);
     corePos.z=starPos.z;
     corePos.z = sign(corePos.z)*pow(cos(length(corePos.xyz)/coreRadius*PI/2),.5)*coreRadius*hash2(starID*130);
     //starPos=normalize(starPos)* coreRadius;
   //}

   //float ds = length(starPos.xy)/coreRadius;
   //starPos.z = sign(starPos.z)*pow(sin(ds)/ds,4)*coreRadius*hash2(starID*130);

   starPos = lerp(starPos,corePos,pow(saturate(dh),8));
    
   
   starPos = rotZ(starPos,rotationAngle);

   starPos.z += 400;  
    //starPos = lerp(normalize(starPos)* 1400.0f, starPos,.5);

   // starPos = randomPosition(starID)*range*2-range;


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
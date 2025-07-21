float DEG_TO_RAD = 0.0174699591855f;
float MAX = 10000.0f;

int   GALAXY_FIELD_VOXEL_STEPS = 10;
float GALAXY_FIELD_VOXEL_STEP_SIZE = 250000.0f; // 2,500,000 AL
float GALAXY_RADIUS = 0.015f;  // (% of 250000)  50,000 AL

int   STAR_FIELD_VOXEL_STEPS = 13;
float STAR_FIELD_VOXEL_STEP_SIZE = 0.5f;  // 5AL 
float STAR_RADIUS = 0.01f; // 2e-8 in true life !   // (% of 5)   1e-8


float kU2G = 500000.0f;
float kG2U = 0.000002f;


float fract(float x)
{
    return x - floor(x);
}

float2 fract(float2 x)
{
    return x - floor(x);
}

float3 fract(float3 x)
{
    return x - floor(x);
}

float4 fract(float4 x)
{
    return x - floor(x);
}


// Time spend traveling to clicked point
#define TRAVEL_DELAY 4.0f

#define IN_UNIVERSE     1.0f
#define IN_GALAXY       2.0f
#define IN_SOLAR_SYSTEM 3.0f

#define MOVING     1.0f
#define STATIONARY 2.0f

#define NONE     0.0f
#define GALAXY   1.0f
#define STAR     2.0f
#define PLANET   3.0f
    
    
#define HASHSCALE1 0.1031f
float hash(float3 p3) {
	p3  = fract(p3 * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19f);
    return fract((p3.x + p3.y) * p3.z);
}


#define HASHSCALE3 float3(0.1031f, 0.1030f, 0.0973f)
float3 hash33(float3 p3){
	p3 = fract(p3 * HASHSCALE3);
    p3 += dot(p3, p3.yxz + 19.19f);
    return fract(float3((p3.x + p3.y) * p3.z, (p3.x+p3.z)*p3.y, (p3.y + p3.z) * p3.x));
}

//--------------------------------------------------------------------

#define R(p, a) p = cos(a) * p+sin(a) * vec2(p.y, -p.x)

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};

VS_OUTPUT VS(uint vID : SV_VertexID ) {

    VS_OUTPUT output;

    uint vertexInQuad = vID % 6;

    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    float2 pos = quadPos[vID];

    float2 uv = float2(0.5 + pos.x * 0.5, 0.5 - pos.y * 0.5);

    output.pos = float4(pos.xy, 0, 1);
    output.uv = uv;

    return output;
}
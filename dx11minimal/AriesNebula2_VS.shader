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

    f *= f;
     
    f = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + 113.0*p.z;
     
    float a = lerp(lerp(lerp( hash(n+0.0), hash(n+1.0),f.x),
            lerp( hash(n+57.0), hash(n+58.0),f.x),f.y),
            lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
            lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);

    return a - 0.5;
}

float random(float2 p) {
    float2 r = float2(23.14069263277926, 2.665144142690225);
    return frac(cos(dot(p, r)) * 12345.6789);
}

// Функция шума Вороного
float3 voronoiNoise(float2 value){
    float2 baseCell = floor(value);

    //first pass to find the closest cell
    float minDistToCell = 10;
    float2 toClosestCell;
    float2 closestCell;
    [unroll]
    for(int x1=-1; x1<=1; x1++){
        [unroll]
        for(int y1=-1; y1<=1; y1++){
            float2 cell = baseCell + float2(x1, y1);
            float2 cellPosition = cell + float2(random(cell.xy), random(cell.yx));
            float2 toCell = cellPosition - value;
            float distToCell = length(toCell);
            if(distToCell < minDistToCell){
                minDistToCell = distToCell;
                closestCell = cell;
                toClosestCell = toCell;
            }
        }
    }

    //second pass to find the distance to the closest edge
    float minEdgeDistance = 10;
    [unroll]
    for(int x2=-1; x2<=1; x2++){
        [unroll]
        for(int y2=-1; y2<=1; y2++){
            float2 cell = baseCell + float2(x2, y2);
            float2 cellPosition = cell + float2(random(cell.xy), random(cell.yx));
            float2 toCell = cellPosition - value;

            float2 diffToClosestCell = abs(closestCell - cell);
            bool isClosestCell = diffToClosestCell.x + diffToClosestCell.y < 0.1;
            if(!isClosestCell){
                float2 toCenter = (toClosestCell + toCell) * 0.5;
                float2 cellDifference = normalize(toCell - toClosestCell);
                float edgeDistance = dot(toCenter, cellDifference);
                minEdgeDistance = min(minEdgeDistance, edgeDistance);
            }
        }
    }

    float rand = random(closestCell);
    return float3(minDistToCell, rand, minEdgeDistance);
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

    float3 starPos;

    //calc star position

    float size = 384;
    float range = 50000;
    starPos = randomPosition(starID) * range * 2 - range;
    starPos.y = 0.0;
    
    float par = starPos.x * starPos.x + starPos.z * starPos.z;
    starPos.y = cos(starPos.x / 100000 * PI) * cos(starPos.z / 100000 * PI) * -7500 + (par * 12000) / (50000000 + par);
    
    starPos.y += noise(starPos.zxy * 0.131 * 5 * 0.00011 + float3(41.547, 14.631, 51.591) + time.x * -0.005) * 4000;

    float3 n = voronoiNoise(starPos.xz * 0.131 * 12 * 0.00011 + time.x * 0.005 + float2(37.613, 83.1312));
    starPos.y -= n.x * n.y * n.z * 10000;


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
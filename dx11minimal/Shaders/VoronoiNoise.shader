float random(float2 p) {
    float2 r = float2(23.14069263277926, 2.665144142690225);
    return frac(cos(dot(p, r)) * 12345.6789);
}

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

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv;

    float3 n = voronoiNoise(uv.xy * 24 + float2(41.547, 14.631));

    return float4(n, 1);
}
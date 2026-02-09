cbuffer global : register(b5)
{
    float4 gConst[1024];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float radius = gConst[0].w;
    
    float2 cornerVector = abs(input.uv) - (1.0 - radius);
    float inCornerRegion = max(cornerVector.x, cornerVector.y) > 0.0;
    float cornerDistance = length(max(cornerVector, 0.0));
    
    float roundMask = 1.0 - smoothstep(0.0, radius, cornerDistance);
    roundMask = inCornerRegion ? roundMask : 1.0;
    
    return gConst[3] * roundMask;
}

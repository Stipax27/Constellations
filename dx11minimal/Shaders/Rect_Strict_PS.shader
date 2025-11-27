cbuffer global : register(b5)
{
    float4 gConst[256];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv;
    float radius = gConst[0].w;
    
    float2 centerBounds = 1.0 - radius;
    float inCenter = all(step(abs(uv), centerBounds));
    
    float inVerticalStrip = (abs(uv.x) <= centerBounds.x) && (abs(uv.y) > centerBounds.y);
    float inHorizontalStrip = (abs(uv.y) <= centerBounds.y) && (abs(uv.x) > centerBounds.x);
    
    float inCornerRegion = (abs(uv.x) > centerBounds.x) && (abs(uv.y) > centerBounds.y);
    
    float2 cornerPos = abs(uv) - centerBounds;
    float inCornerCircle = inCornerRegion && (dot(cornerPos, cornerPos) <= radius * radius);
    
    float finalMask = inCenter || inVerticalStrip || inHorizontalStrip || inCornerCircle;
    
    return gConst[3] * finalMask;
}

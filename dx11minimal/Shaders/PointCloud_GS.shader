#include <lib/constBuf.shader>

bool IsPointInFrustum(float4 pos)
{
    float3 c = pos.xyz / pos.w;
    return (abs(c.x) <= 1.0 && abs(c.y) <= 1.0 && abs(c.z) <= 1.0);
}

bool IsSphereInFrustum(float4 pos, float radius)
{
    float3 center = pos.xyz / pos.w;
    float r = radius / abs(pos.w);
    
    return (abs(center.x) - r <= 1.0 && 
            abs(center.y) - r <= 1.0 && 
            abs(center.z) - r <= 1.0);
}


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
    float4 vnorm : NORMAL0;
    float2 uv : TEXCOORD0;
};


[maxvertexcount(36)]
void GS( point VS_OUTPUT input[1], inout TriangleStream<VS_OUTPUT> output )
{
    if (!IsPointInFrustum(input[0].pos))
    {
        return;
    }

    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(-1, 1), float2(1, -1), float2(1, 1),
    };

    [unroll]
    for (uint i = 0; i < 6; i++) {
        VS_OUTPUT element = input[0];

        float dist = length(element.wpos.xyz - cPos.xyz);
        float scale = clamp(abs(dot(float3(-1, 0, 0), element.vnorm)), 0.75, 1);
        //float scale = 1 - clamp(abs(dot(float3(-1, 0, 0), element.vnorm)), 0, 1);

        float2 offset = quadPos[i] * float2(aspect.x, 1) * min(dist, 18) * scale * 0.04 ;

        element.pos.xy += offset;
        element.uv = quadPos[i];
        output.Append( element );
    }

    output.RestartStrip();
}
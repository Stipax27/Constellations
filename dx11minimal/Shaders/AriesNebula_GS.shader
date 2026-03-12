#include <lib/constBuf.shader>

bool IsPointInFrustum(float4 pos)
{
    float3 c = pos.xyz / pos.w;
    return (abs(c.x) <= 1.0 && abs(c.y) <= 1.0 && abs(c.z) <= 1.0);
}


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 wpos : POSITION0;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
};


[maxvertexcount(6)]
void GS( point VS_OUTPUT input[1], inout TriangleStream<VS_OUTPUT> output )
{
    if (!IsPointInFrustum(input[0].pos))
    {
        return;
    }

	float scale = gConst[0].w;
	float size = 1 * scale;

	float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(-1, 1), float2(1, -1), float2(1, 1),
    };

	for (uint i = 0; i < 6; i++) {
		VS_OUTPUT element = input[0];

        float dist = length(cPos.xyz - element.wpos.xyz);
		float2 offset = quadPos[i] * float2(aspect.x, 1) * size;

		element.pos.xy += offset;
		element.uv = quadPos[i];
		output.Append( element );

        if (i == 2) {
            output.RestartStrip();
        }
	}
}
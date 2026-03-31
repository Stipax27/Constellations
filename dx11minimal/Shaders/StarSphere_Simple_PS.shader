#include <lib/constBuf.shader>

Texture2D starTexture : register(t1);
SamplerState starSamplerState : register(s1);

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
	float2 uv : TEXCOORD0;
	uint iID : COLOR0;
};


float4 PS(VS_OUTPUT input) : SV_Target
{
	float n = starTexture.Sample(starSamplerState, input.uv).r;

	return lerp(float4(gConst[0].xyz, 1), float4(gConst[1].xyz, 1), n);
}

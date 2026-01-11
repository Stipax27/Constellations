cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer camera : register(b3)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float4 cPos;
};


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
    float2 uv : TEXCOORD0;
};


[maxvertexcount(6)]
void GS( point VS_OUTPUT input[1], inout TriangleStream<VS_OUTPUT> output )
{
	float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(-1, 1), float2(1, -1), float2(1, 1),
    };

	[unroll]
	for (uint i = 0; i < 6; i++) {
		VS_OUTPUT element = input[0];

		float dist = length(cPos.xyz - element.wpos.xyz);
		float2 offset = quadPos[i] * float2(aspect.x, 1) * min(dist, 30) * 0.03;

		element.pos.xy += offset;
		element.uv = quadPos[i];
		output.Append( element );
	}

    output.RestartStrip();
}
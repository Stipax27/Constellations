cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
    float4 vnorm : NORMAL0;
    float2 uv : TEXCOORD0;
};


[maxvertexcount(6)]
void GS( point VS_OUTPUT input[1], inout TriangleStream<VS_OUTPUT> output )
{
	float2 quadPos[6] = {
		float2(-1, -1), float2(1, -1), float2(-1, 1),
		float2(1, -1), float2(1, 1), float2(-1, 1)
	};

	for (uint i = 0; i < 6; i++) {
		float2 offset = quadPos[i] * float2(aspect.x, 1) * 0.1;
		VS_OUTPUT element = input[0];
		element.pos.xy += offset;
		output.Append( element );
	}

    output.RestartStrip();
}
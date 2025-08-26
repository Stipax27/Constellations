cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};

[maxvertexcount(3)]
void GS(triangle VS_OUTPUT input[3], inout TriangleStream< VS_OUTPUT > output)
{
	float2 quadPos[6] = {
		float2(-1, -1), float2(1, -1), float2(-1, 1),
		float2(1, -1), float2(1, 1), float2(-1, 1)
	};
	float size = 0.02;

	// for (uint i = 0; i < 3; i++) {
	// 	VS_OUTPUT inElement = input[0];
	// 	inElement.pos.xy += quadPos[i];
	// 	output.Append( inElement );
	// }
	// output.RestartStrip();

	// input[0].pos += float4(25000, 0, 0, 0);
	// input[1].pos += float4(25000, 0, 0, 0);
	// input[2].pos += float4(25000, 0, 0, 0);

	output.Append( input[0] );
	output.Append( input[1] );
	output.Append( input[2] );

	output.RestartStrip();
}
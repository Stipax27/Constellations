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

float2 quadPos[6] = {
    float2(-1, -1), float2(1, -1), float2(-1, 1),
    float2(1, -1), float2(1, 1), float2(-1, 1)
};
float size = 0.02;

[maxvertexcount(6)]
void GS(triangle VS_OUTPUT input[3], inout TriangleStream< VS_OUTPUT > output)
{
	//VS_OUTPUT inElement = input[0];
	//float4 pos = inElement.pos;

	//for (uint i = 0; i < 6; i++)
	//{
		// float2 qPos = quadPos[i];
		// float4 pointPos = float4(pos.xy + qPos * float2(aspect.x, 1) * size, pos.zw);

		// VS_OUTPUT element;
		// element.pos = pointPos;
		// element.uv = qPos;
		// element.starID = inElement.starID;
		// element.worldpos = inElement.worldpos;

		// output.Append(element);
		// if ((i + 1) % 3 == 0) {
		// 	output.RestartStrip();
		// }

	//}

	VS_OUTPUT inElement = input[0];
	inElement.pos += float4(100, 0, 0, 0);

	output.Append( inElement );
	output.Append( input[1] );
	output.Append( input[2] );
	output.RestartStrip();
}
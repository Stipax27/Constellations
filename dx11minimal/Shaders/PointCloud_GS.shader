//struct GSOutput
//{
//	float4 pos : SV_POSITION;
//};
//
//[maxvertexcount(3)]
//void main(
//	triangle float4 input[3] : SV_POSITION, 
//	inout TriangleStream< GSOutput > output
//)
//{
//	for (uint i = 0; i < 3; i++)
//	{
//		GSOutput element;
//		element.pos = input[i];
//		output.Append(element);
//	}
//}













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
	float size = 100;

	// for (uint i = 0; i < 3; i++) {
	// 	VS_OUTPUT inElement = input[0];
	// 	inElement.pos.xy += quadPos[i] * size;
	// 	output.Append( inElement );
	// }
	// output.RestartStrip();

	output.Append( input[0] );
	output.Append( input[1] );
	output.Append( input[2] );

	output.RestartStrip();
}
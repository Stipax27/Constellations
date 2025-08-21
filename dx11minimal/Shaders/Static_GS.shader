struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};

struct GS_OUTPUT
{
	float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};

[maxvertexcount(3)]
void GS(triangle VS_OUTPUT input[3], inout TriangleStream< GS_OUTPUT > output)
{
	for (uint i = 0; i < 3; i++)
	{
		GS_OUTPUT element;
		element.pos = input[i].pos;
		element.uv = input[i].uv;
		element.starID = input[i].starID;
		element.worldpos = input[i].worldpos;

		output.Append(element);
	}
}
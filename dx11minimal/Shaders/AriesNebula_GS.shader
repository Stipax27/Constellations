struct GS_OUT
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

[maxvertexcount(6)]
void GS(point float4 input[1] : SV_POSITION, inout TriangleStream< GS_OUT > output)
{
	for (uint i = 0; i < 6; i++)
	{
		GSOutput element;
		element.pos = input[i];
		output.Append(element);
	}
}
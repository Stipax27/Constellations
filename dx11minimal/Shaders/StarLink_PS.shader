cbuffer global : register(b5)
{
    float4 gConst[256];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};


float4 PS(VS_OUTPUT input) : SV_Target
{
    float y = input.uv.y;
    float c = saturate(1 - sqrt(sqrt(abs(y))));
    float4 color = gConst[255] * c;

    return color;
}

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};


float4 PS(VS_OUTPUT input) : SV_Target
{
    float x = input.uv.x;
    float y = input.uv.y;

    float c = saturate(1 - sqrt(abs(y)));
    c *= saturate(cos(x * 3.141592653589793 / 2));

    return float4(input.color.xyz, 1) * input.color.w * c;
}

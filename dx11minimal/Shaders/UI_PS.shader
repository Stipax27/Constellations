cbuffer ColorBuffer : register(b5)
{
    float4 global[4];
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float x = input.uv.x;
    float y = input.uv.y;
    return float4(saturate(global[3].xyz),saturate(global[3].w)* (saturate(1-pow(abs(y-.5)*2.,1)) * saturate(1-pow(abs(x-.5)*2.,16))));
}
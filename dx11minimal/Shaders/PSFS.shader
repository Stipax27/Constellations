cbuffer global : register(b5)
{
    float4 gConst[32];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

float hash11(uint n) {
    n = (n << 13u) ^ n;
    return frac((n * (n * n * 15731u + 789221u) + 1376312589u) * 0.000000000931322574615478515625f);
}

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   starID : COLOR0;
    float4 worldpos : POSITION1;
};


float4 PS(VS_OUTPUT input) : SV_Target
{
    //return input.worldpos.x/2000+.5;

    float2 uv = input.uv;
float brightness = exp(-dot(uv, uv) * 20);
return float4(brightness, brightness, brightness, 1)*float4(1,1,1.4,1)*(1+.9*sin(input.starID*1.2+time.x*.1));
}
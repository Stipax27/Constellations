cbuffer global : register(b5)
{
    float4 gConst[1024];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer drawMat : register(b2)
{
    float4x4 model;
    float hilight;
};

cbuffer particlesDesc : register(b9)
{
	float2 pSize;
	float2 pOpacity;
	float3 pColor;
	float pLifetime;
};


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   iID : COLOR0;
};


float3 rotZ(float3 pos, float a)
{
    float3x3 m =
    {
        cos(a), -sin(a),0,
        sin(a), cos(a), 0,
        0, 0, 1
    };
    pos = mul(pos, m);
    return pos;
}

float star(float2 uv)
{
    float c = saturate(1. - 1. * length(uv));
    c = pow(c, 3);
    c *= saturate(1. - 228. * abs(uv.x) * abs(uv.y));
    c += pow(max(sin(length(uv * 3.14)), 0), 118) * .03;
    return c;
}


float4 PS(VS_OUTPUT input) : SV_Target
{
    float lifetime = (time.x * 100 - gConst[input.iID].w) / pLifetime;
    float4 color = float4(pColor, 1) * lerp(pOpacity.x, pOpacity.y, lifetime);

    float brightness = exp(-dot(input.uv, input.uv) * 20);

    return saturate(color * float4(brightness, brightness, brightness, brightness));

}

cbuffer drawerFloat4x4 : register(b10)
{
    float4x4 fConst[1024];
};

cbuffer frame : register(b4)
{
    float4 time;
    float4 aspect;
};

cbuffer camera : register(b3)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
    float4 cPos;
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
	float2 pSpeed;
};

cbuffer objParams : register(b0)
{
    float drawerV[1024];
};


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   iID : COLOR0;
    float4 wpos : POSITION1;
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
    float localTime = drawerV[0];
    float lifetime = (localTime - fConst[input.iID]._m30) / pLifetime;
    float4 color = float4(pColor, 1) * lerp(pOpacity.x, pOpacity.y, lifetime);

    float brightness = exp(-dot(input.uv, input.uv) * 20);

    float dist = length(cPos.xyz - input.wpos);
    brightness *= min(max(dist - 1, 0) / 10, 1);

    return saturate(color * float4(brightness, brightness, brightness, brightness));

}

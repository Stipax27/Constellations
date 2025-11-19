cbuffer drawerV : register(b0)
{
    float drawConst[32];
}

cbuffer global : register(b5)
{
    float4 gConst[32];
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
};

cbuffer drawMat : register(b2)
{
    float4x4 model;
    float hilight;
};

struct VS_INPUT
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
    float4 vnorm : NORMAL1;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    float3 pos = gConst[0].xyz;
    float3 scale = gConst[1].xyz;

    output.pos = mul(float4(input.position.xyz * scale + pos, 1), mul(view, proj));
    output.vpos = mul(output.pos, view);
    output.wpos = float4(input.position.xyz, 1);
    //output.vnorm = normalize(mul(input.normal, world));
    output.uv = input.uv;

    return output;
}

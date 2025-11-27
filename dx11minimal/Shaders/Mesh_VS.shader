cbuffer drawerV : register(b0)
{
    float drawConst[256];
}

cbuffer global : register(b5)
{
    float4 gConst[256];
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
    float4 vnorm : NORMAL0;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    float4 pos = float4(input.position.xyz, 1);
    pos = mul(pos, world);

    output.pos = mul(pos, mul(view, proj));
    output.vpos = mul(float4(pos.xyz, 1), view);
    output.wpos = float4(pos.xyz, 0);
    //output.vnorm = normalize(mul(input.normal, world));
    output.vnorm = float4(normalize(input.normal), 1);
    output.uv = input.uv;

    return output;
}

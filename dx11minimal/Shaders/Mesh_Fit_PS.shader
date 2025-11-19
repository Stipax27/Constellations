Texture2D inputTexture : register(t1);
SamplerState samplerState : register(s1);

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

cbuffer params : register(b1)
{
    float r, g, b;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
    float4 vnorm : NORMAL1;
    float2 uv : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    //return float4(1, 0, 1, 1);

    float3 lightDir = normalize(float3(1, -1, 0.25));
    float4 ambientColor = float4(0.15, 0.15, 0.15, 1);
    float specularPower = 2;

    lightDir = -lightDir;

    float3 eye = -(view._m02_m12_m22) * view._m32;
    float3 viewDir = normalize(input.wpos.xyz - eye);

    float3 diffuse = saturate(dot(lightDir, input.vnorm));

    float3 reflectDir = normalize(reflect(lightDir, input.vnorm));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float3 specular = specularPower * spec;

    float3 tex = inputTexture.Sample(samplerState, input.uv);

    return float4(tex, 1);
    //return saturate(float4(reflectDir, 1));
    return saturate(float4(tex * (ambientColor.xyz + diffuse + specular), 1));
}
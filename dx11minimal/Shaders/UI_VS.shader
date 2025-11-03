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
    //float4x4 world[2];
    float4x4 view;
    float4x4 proj;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

VS_OUTPUT VS(uint vID : SV_VertexID)
{
    VS_OUTPUT output;

    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };

    float2 pos = gConst[0].xy;
    float2 size = gConst[1].xy;
    float2 anchorPoint = gConst[2].xy;
    //float2 screenSize = gConst[1].xy;

    float2 screenPos = mul(float4(quadPos[vID], 0.0, 1.0), proj).xy;

    float2 basePos = pos + size;
   
    float2 screenPos = basePos + quadPos[vID] * size;

    // переводим в NDC
    float2 ndc;
    ndc.x = (screenPos.x / screenSize.x) * 2.0 - 1.0;
    ndc.y = 1.0 - (screenPos.y / screenSize.y) * 2.0;

    output.pos = float4(ndc, 0, 1);
    output.uv  = quadPos[vID];

    return output;
}

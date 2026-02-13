cbuffer objParams : register(b0)
{
    float drawerV[1024];
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


struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
    float4 vnorm : NORMAL0;
    float2 uv : TEXCOORD0;
};

#define PI 3.14159265358979323846
#define triangles 12

[maxvertexcount(36)]
void GS( point VS_OUTPUT input[1], inout TriangleStream<VS_OUTPUT> output )
{
    float2 quadPos[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(-1, 1), float2(1, -1), float2(1, 1),
    };

    [unroll]
    for (uint i = 0; i < 6; i++) {
        VS_OUTPUT element = input[0];

        float dist = length(element.wpos.xyz - cPos.xyz);
        float scale = clamp(abs(dot(float3(-1, 0, 0), element.vnorm)), 0.75, 1);
        //float scale = 1 - clamp(abs(dot(float3(-1, 0, 0), element.vnorm)), 0, 1);

        float2 offset = quadPos[i] * float2(aspect.x, 1) * min(dist, 18) * scale * 0.04 ;

        element.pos.xy += offset;
        element.uv = quadPos[i];
        output.Append( element );
    }

    output.RestartStrip();


    // float angle = PI * 2 / triangles;
    // float pointSize = drawerV[0];

    // [unroll]
    // for (int triangleID = 0; triangleID < triangles; triangleID++) {
    //     [unroll]
    //     for (int pID = 0; pID < 3; pID++) {
    //         float startAngle = angle * triangleID;
    //         float endAngle = angle * (triangleID + 1);

    //         float2 triangleUV[3] = {
    //             float2(0, 0), float2(sin(startAngle), cos(startAngle)), float2(sin(endAngle), cos(endAngle))
    //         };

    //         VS_OUTPUT element = input[0];

    //         float dist = length(element.wpos.xyz - cPos.xyz);
    //         float scale = clamp(abs(dot(float3(-1, 0, 0), element.vnorm)), 0.5, 1);
    //         //float scale = 1 - clamp(abs(dot(float3(-1, 0, 0), element.vnorm)), 0, 1);

    //         float2 offset = triangleUV[pID] * float2(aspect.x, 1) * min(dist, 18) * scale * pointSize * 0.05;

    //         element.pos.xy += offset;
    //         element.uv = triangleUV[pID];
    //         output.Append( element );
    //     }
    //     output.RestartStrip();
    // }
}
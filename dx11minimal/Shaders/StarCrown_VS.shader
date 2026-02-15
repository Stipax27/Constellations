#include <lib/constBuf.shader>
#include <lib/pi.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output;

    uint triangles = drawerV[0];

    float angle = PI * 2 / triangles;

    uint triangleID = vID / 3;
    uint pID = vID % 3;

    float startAngle = angle * triangleID;
    float endAngle = angle * (triangleID + 1);

    float2 triangleUV[3] = {
        float2(0, 0), float2(sin(startAngle), cos(startAngle)), float2(sin(endAngle), cos(endAngle))
    };

    float4 pos = float4(gConst[iID + 1].xyz, 1);
    float sz = gConst[iID + 1].w;

    float3 eye = cPos.xyz;
    float3 front = normalize(eye - pos.xyz);

    float3 up = abs(dot(front, float3(0, 1, 0))) > 0.9999 ? float3(0, 0, 1) : float3(0, 1, 0);
    float3 right = normalize(cross(up, front));
    up = normalize(cross(front, right));

    float2 triangleUVpos = triangleUV[pID];
    pos.xyz += (right * triangleUVpos.x + up * triangleUVpos.y) * sz;
        
    float4 viewPos = mul(pos, view);
    float4 projPos = mul(viewPos, proj);

    //projPos.xy += triangleUV[pID] * float2(aspect.x, 1) * sz * 2;

    output.uv = triangleUV[pID];
    output.pos = projPos;

    return output;
}
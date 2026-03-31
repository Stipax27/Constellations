#include <lib/constBuf.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 vpos : POSITION0;
    float4 wpos : POSITION1;
	float2 uv : TEXCOORD0;
	uint iID : COLOR0;
};

float2 calculateUV(float3 pos)
{
    float3 dir = normalize(pos);
    
    float theta = atan2(dir.z, dir.x);  // azimut angle (-PI .. PI)
    float phi = acos(dir.y);             // polar angle (0 .. PI)
    
    float u = (theta + 3.14159265359) / (2.0 * 3.14159265359);
    float v = phi / 3.14159265359;
    
    return float2(u, v);
}

float3 ball(float2 p, float radius, uint iID)
{
    float n = (float)drawInt[iID];

    p.x = -(p.x / n) * 3.141592653589793;
    p.y = (p.y / n) * 3.141592653589793 / 2;

    float3 pos = float3(cos(p.x) * cos(p.y) * radius, sin(p.y) * radius, sin(p.x) * cos(p.y) * radius);

    return pos;
}

VS_OUTPUT VS(uint vID : SV_VertexID, uint iID : SV_InstanceID)
{
    VS_OUTPUT output;

    uint n = drawInt[iID];
    uint instanceID = vID / 6;

    float row = instanceID % n;
    float col = instanceID / n;

    float2 quadUV[6] = {
        float2(-1, -1), float2(1, -1), float2(-1, 1),
        float2(1, -1), float2(1, 1), float2(-1, 1)
    };
    float2 p = quadUV[vID % 6];

    float4 pos = float4(p, 0, 1);
    pos.y += col * 2;
    pos.x += row * 2;
    pos.xy -= (float)n - 1;

	float radius = gConst[iID].w;
    pos.xyz = ball(pos.xy, radius, iID);

    output.vpos = pos;
	output.uv = calculateUV(pos);

    pos = mul(pos, model[iID]);

    output.pos = mul(pos, mul(view, proj));
    output.wpos = float4(pos.xyz, 0);
	output.iID = iID;

    return output;
}
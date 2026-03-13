#include <lib/constBuf.shader>

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    uint   iID : COLOR0;
    float4 wpos : POSITION1;
};


float4 PS(VS_OUTPUT input) : SV_Target
{

    float localTime = drawerV[0];
    float startTime = fConst[input.iID]._m30;

    float lifetime = abs(localTime - startTime) / pLifetime;
    lifetime = pTimescale < 0 ? 1 - lifetime : lifetime;

    float4 color = float4(pColor, 1) * lerp(pOpacity.x, pOpacity.y, lifetime);

    float brightness = exp(-dot(input.uv, input.uv) * 20);

    float dist = length(cPos.xyz - input.wpos);
    brightness *= min(max(dist - 1, 0) / 10, 1);

    return saturate(color * float4(brightness, brightness, brightness, brightness));

}
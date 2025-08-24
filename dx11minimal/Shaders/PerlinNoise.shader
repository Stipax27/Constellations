float hash( float n ) {
    return frac(sin(n)*43758.5453);
}

float noise( float3 x ) {
    float3 p = floor(x);
    float3 f = frac(x);

    f *= f;
     
    f = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + 113.0*p.z;
     
    float a = lerp(lerp(lerp( hash(n+0.0), hash(n+1.0),f.x),
            lerp( hash(n+57.0), hash(n+58.0),f.x),f.y),
            lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
            lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);

    return a;
}

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 PS(VS_OUTPUT input) : SV_Target
{
    float2 uv = input.uv;

    float n = noise(float3(uv.x, uv.y, 0) * 16 + float3(41.547, 14.631, 51.591));

    return float4(n, n, n, 1);
}
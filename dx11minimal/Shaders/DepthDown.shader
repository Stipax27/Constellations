cbuffer drawerInt : register(b7)
{
    int drawInt[256];
}


Texture2D<float> InputDepth : register(t0);
RWTexture2D<float> OutputDepth : register(u0);

[numthreads(8, 8, 1)]
void CS(uint3 id : SV_DispatchThreadID)
{
    int scale = drawInt[0];

    uint2 srcPos = id.xy * scale;

    float depths[64];
    
    for (int a = 0; a < scale; a++)
    {
        for (int b = 0; b < scale; b++)
        {
            depths[a * scale + b] = InputDepth[srcPos + uint2(a, b)];
        }
    }

    float minDepth = depths[0];
    [unroll]
    for (int i = 1; i < scale * scale; i++)
    {
        minDepth = min(minDepth, depths[i]);
    }
    
    OutputDepth[id.xy] = minDepth;
}
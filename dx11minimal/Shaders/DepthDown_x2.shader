Texture2D<float> InputDepth : register(t0);
RWTexture2D<float> OutputDepth : register(u0);

[numthreads(8, 8, 1)]
void CS(uint3 id : SV_DispatchThreadID)
{
    uint2 srcPos = id.xy * 2;
    
    float d1 = InputDepth[srcPos];
    float d2 = InputDepth[srcPos + uint2(1, 0)];
    float d3 = InputDepth[srcPos + uint2(0, 1)];
    float d4 = InputDepth[srcPos + uint2(1, 1)];
    
    float minDepth = min(min(d1, d2), min(d3, d4));
    
    OutputDepth[id.xy] = minDepth;
}
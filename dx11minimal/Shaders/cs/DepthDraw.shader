Texture2D<float> InputDepth : register(t0);
RWTexture2D<float> OutputDepth : register(u0);

[numthreads(8, 8, 1)]
void CS(uint3 id : SV_DispatchThreadID)
{
    float depth = InputDepth[id.xy];
    OutputDepth[id.xy] = depth;
}
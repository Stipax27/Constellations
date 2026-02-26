//[
VS_OUTPUT_PARTICLE VS(uint vID : SV_VertexID,uint iID : SV_InstanceID)
{
    VS_OUTPUT_PARTICLE output;

    float4 grid = getGridInst(vID, iID, lgX, lgY); 
    pos_color p = CalcParticles(vID, iID, grid);
    
    output.pos = p.pos;
    output.wpos = p.wpos;
    output.color = p.color;
    output.uv = grid.zw;
    output.size = p.sz;

    return output;

}
//]

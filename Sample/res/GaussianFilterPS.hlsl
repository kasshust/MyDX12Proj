///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position : SV_POSITION; // 位置座標です.
    float2 TexCoord : TEXCOORD; // テクスチャ座標です.
};

///////////////////////////////////////////////////////////////////////////////
// CbTonemap constant buffer
///////////////////////////////////////////////////////////////////////////////
cbuffer CbGau : register(b0)
{
    float ReductionRatio;    
};

//-----------------------------------------------------------------------------
// Textures and Sampler
//-----------------------------------------------------------------------------
Texture2D ColorMap : register(t0);
SamplerState ColorSmp : register(s0);


float4 main(const VSOutput input) : SV_TARGET
{
    
    float2 uv   = input.TexCoord.xy * ReductionRatio;
    float2 div  = 2.f / float2(960.f, 540.f) * ReductionRatio;
    
    float weight[4];
    weight[0] = 0.269f;
    weight[1] = 0.216f;
    weight[2] = 0.113f;
    weight[3] = 0.036f;
    
    float3 col = float3(0.0,0.0,0.0);
    
    // 横
    
    col += ColorMap.Sample(ColorSmp, uv) * weight[0] * 2.0;
    
    [unroll]
    for (int i = 1; i < 4; i++)
    {
        col += ColorMap.Sample(ColorSmp, uv + float2(div.x, 0.0f) * i) * weight[i];
        col += ColorMap.Sample(ColorSmp, uv + float2(div.x, 0.0f) * -i) * weight[i];
    }
    
    [unroll]
    for (int j = 1; j < 4; j++)
    {
        col += ColorMap.Sample(ColorSmp, uv - float2(0.0f, div.y) * j) * weight[j];
        col += ColorMap.Sample(ColorSmp, uv - float2(0.0f, div.y) * -j) * weight[j];
    }
    
    return float4(col * 0.5f,1.0);
}
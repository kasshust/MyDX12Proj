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
cbuffer CbBlo: register(b0)
{
};

//-----------------------------------------------------------------------------
// Textures and Sampler
//-----------------------------------------------------------------------------
Texture2D ColorMap : register(t0);

Texture2D BloomMap0 : register(t1);
Texture2D BloomMap1 : register(t2);
Texture2D BloomMap2 : register(t3);
Texture2D BloomMap3 : register(t4);

SamplerState ColorSmp : register(s0);


float4 main(const VSOutput input) : SV_TARGET
{
    float4 src = ColorMap.Sample(ColorSmp, input.TexCoord.xy);
    
    float4 blm0 = BloomMap0.Sample(ColorSmp, input.TexCoord.xy);
    float4 blm1 = BloomMap1.Sample(ColorSmp, input.TexCoord.xy);
    float4 blm2 = BloomMap2.Sample(ColorSmp, input.TexCoord.xy);
    float4 blm3 = BloomMap3.Sample(ColorSmp, input.TexCoord.xy);
    
    float4 blm = lerp(lerp(blm0, blm1, 0.5), lerp(blm2, blm3, 0.5), 0.5);
    
    float4 last = src + float4(blm.xyz, 1.0);
    
    return last;
}
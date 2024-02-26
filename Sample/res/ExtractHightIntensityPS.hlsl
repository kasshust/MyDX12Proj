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
cbuffer CbExt : register(b0)
{
};

//-----------------------------------------------------------------------------
// Textures and Sampler
//-----------------------------------------------------------------------------
Texture2D ColorMap : register(t0);
SamplerState ColorSmp : register(s0);

// 輝度値の計算 出展必要
float GetLuminance(float3 color)
{
    return dot(color, float3(0.299, 0.587, 0.114));
}

float4 main(const VSOutput input) : SV_TARGET
{
    float4 src = ColorMap.Sample(ColorSmp, input.TexCoord.xy );
    float luminance = GetLuminance(src.rgb);
    
    float HIGH_LUM_THRESHOLD = 0.1f;
    float EXPOSURE = 2.0f;
    
    return float4(src.rgb * max(0.0f, (luminance - HIGH_LUM_THRESHOLD)) * EXPOSURE, 1.0f);
}
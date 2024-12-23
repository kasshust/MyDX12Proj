
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
cbuffer CbPreNormal : register(b0)
{
}

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4      Position        : SV_POSITION; // 位置座標です.
    float2      TexCoord        : TEXCOORD; // テクスチャ座標です.
    float3      WorldPos        : WORLD_POS; // ワールド空間の位置座標です.
    float3x3    InvTangentBasis : INV_TANGENT_BASIS; // 接線空間への基底変換行列の逆行列です.
    float3      WorldNormal     : TEXCOORD1;
};

///////////////////////////////////////////////////////////////////////////////
// PSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct PSOutput
{
    float4 Color : SV_TARGET0; // 出力カラーです.
};


//-----------------------------------------------------------------------------
//      ピクセルシェーダのメインエントリーポイントです.
//-----------------------------------------------------------------------------
PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput) 0;
    
    output.Color = float4( (input.WorldNormal.xyz + 1.0f.xxx) / 2.0f, 1.0f);
    return output;
}
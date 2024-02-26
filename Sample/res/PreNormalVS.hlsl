#include "VSCommonBuffer.hlsli"

///////////////////////////////////////////////////////////////////////////////
// VSInput structure
///////////////////////////////////////////////////////////////////////////////
struct VSInput
{
    float3 Position : POSITION; // 位置座標です.
    float3 Normal : NORMAL; // 法線ベクトルです.
    float2 TexCoord : TEXCOORD; // テクスチャ座標です.
    float3 Tangent : TANGENT; // 接線ベクトルです.
};

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position             : SV_POSITION;  // 位置座標です.
    float2 TexCoord             : TEXCOORD;     // テクスチャ座標です.
    float3 WorldPos             : WORLD_POS;    // ワールド空間の位置座標です.
    float3x3 InvTangentBasis    : INV_TANGENT_BASIS; // 接線空間への基底変換行列の逆行列です.
    float3 WorldNormal          : TEXCOORD1;
};


//-----------------------------------------------------------------------------
//      頂点シェーダのメインエントリーポイントです.
//-----------------------------------------------------------------------------
VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    float4 localPos     = float4(input.Position, 1.0f);
    float4 worldPos     = mul(World, localPos);
    float4 viewPos      = mul(View, worldPos);
    float4 projPos      = mul(Proj, viewPos);

    float4 worldNormal = mul(World, input.Normal);
    
    output.Position = projPos;
    output.TexCoord = input.TexCoord;
    output.WorldPos = worldPos.xyz;
    output.WorldNormal = worldNormal.xyz;
    
    // 基底ベクトル
    float3 N = normalize(mul((float3x3) World, input.Normal));
    float3 T = normalize(mul((float3x3) World, input.Tangent));
    float3 B = normalize(cross(N, T));

    // 基底変換行列の逆行列.
    output.InvTangentBasis = transpose(float3x3(T, B, N));

    return output;
}
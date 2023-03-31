#include "VSCommonBuffer.hlsli"


cbuffer CbLight : register(b2)
{
    float TextureSize : packoffset(c0);         // テクスチャサイズです.
    float MipCount : packoffset(c0.y);          // ミップカウントです.
    float LightIntensity : packoffset(c0.z);    // ライト強度(スケール値).
    float3 LightDirection : packoffset(c1);     // ディレクショナルライトの方向.
    float4x4 LightVP : packoffset(c2);          // ライトVP
};


struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 UV       : TEXCOORD;
};

//シャドーマップ計算用頂点シェーダ
float4 main(VS_INPUT input) : SV_POSITION
{
    float4 Pos = float4(input.Position, 1.0f);
    Pos = mul(Pos, World);
    Pos = mul(Pos, LightVP);

    return Pos;
}

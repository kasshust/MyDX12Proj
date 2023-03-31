#ifndef LIGHT_BUFFER_HLSLI
#define LIGHT_BUFFER_HLSLI
#endif //LIGHT_BUFFER_HLSLI


///////////////////////////////////////////////////////////////////////////////
// Light constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbLight : register(b3)
{
    float       TextureSize         : packoffset(c0);       // テクスチャサイズです.
    float       MipCount            : packoffset(c0.y);     // ミップカウントです.
    float       LightIntensity      : packoffset(c0.z);     // ライト強度(スケール値).
    float3      LightDirection      : packoffset(c1);       // ディレクショナルライトの方向.
    float4x4    LightVP             : packoffset(c2);       // ライトVP
};



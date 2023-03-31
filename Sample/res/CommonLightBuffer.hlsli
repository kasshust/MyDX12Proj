#ifndef LIGHT_BUFFER_HLSLI
#define LIGHT_BUFFER_HLSLI
#endif //LIGHT_BUFFER_HLSLI


///////////////////////////////////////////////////////////////////////////////
// Light constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbLight : register(b3)
{
    float       TextureSize         : packoffset(c0);       // �e�N�X�`���T�C�Y�ł�.
    float       MipCount            : packoffset(c0.y);     // �~�b�v�J�E���g�ł�.
    float       LightIntensity      : packoffset(c0.z);     // ���C�g���x(�X�P�[���l).
    float3      LightDirection      : packoffset(c1);       // �f�B���N�V���i�����C�g�̕���.
    float4x4    LightVP             : packoffset(c2);       // ���C�gVP
};



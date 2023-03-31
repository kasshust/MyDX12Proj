#include "VSCommonBuffer.hlsli"


cbuffer CbLight : register(b2)
{
    float TextureSize : packoffset(c0);         // �e�N�X�`���T�C�Y�ł�.
    float MipCount : packoffset(c0.y);          // �~�b�v�J�E���g�ł�.
    float LightIntensity : packoffset(c0.z);    // ���C�g���x(�X�P�[���l).
    float3 LightDirection : packoffset(c1);     // �f�B���N�V���i�����C�g�̕���.
    float4x4 LightVP : packoffset(c2);          // ���C�gVP
};


struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 UV       : TEXCOORD;
};

//�V���h�[�}�b�v�v�Z�p���_�V�F�[�_
float4 main(VS_INPUT input) : SV_POSITION
{
    float4 Pos = float4(input.Position, 1.0f);
    Pos = mul(Pos, World);
    Pos = mul(Pos, LightVP);

    return Pos;
}

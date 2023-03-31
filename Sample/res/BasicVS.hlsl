//-----------------------------------------------------------------------------
// File : BasicVS.hlsl
// Desc : Vertex Shader.
// Copyright(c) Pocol. All right reserved.
//-----------------------------------------------------------------------------
#include "VSCommonBuffer.hlsli"
#include "CommonLightBuffer.hlsli"

///////////////////////////////////////////////////////////////////////////////
// VSInput structure
///////////////////////////////////////////////////////////////////////////////
struct VSInput
{
    float3  Position : POSITION;    // �ʒu���W�ł�.
    float3  Normal   : NORMAL;      // �@���x�N�g���ł�.
    float2  TexCoord : TEXCOORD;    // �e�N�X�`�����W�ł�.
    float3  Tangent  : TANGENT;     // �ڐ��x�N�g���ł�.
};

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4      Position        : SV_POSITION;          // �ʒu���W�ł�.
    float4      PosSM           : POSITION_SM;          // 
    float2      TexCoord        : TEXCOORD;             // �e�N�X�`�����W�ł�.
    float3      WorldPos        : WORLD_POS;            // ���[���h��Ԃ̈ʒu���W�ł�.
    float3x3    InvTangentBasis : INV_TANGENT_BASIS;    // �ڐ���Ԃւ̊��ϊ��s��̋t�s��ł�.
};


//-----------------------------------------------------------------------------
//      ���_�V�F�[�_�̃��C���G���g���[�|�C���g�ł�.
//-----------------------------------------------------------------------------
VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput)0;

    float4 localPos = float4(input.Position, 1.0f);
    float4 worldPos = mul(World, localPos);
    float4 viewPos  = mul(View,  worldPos);
    float4 projPos  = mul(Proj,  viewPos);

    output.Position = projPos;
    output.TexCoord = input.TexCoord;
    output.WorldPos = worldPos.xyz;

    // ���x�N�g��
    float3 N = normalize(mul((float3x3)World, input.Normal));
    float3 T = normalize(mul((float3x3)World, input.Tangent));
    float3 B = normalize(cross(N, T));

    // ���ϊ��s��̋t�s��.
    output.InvTangentBasis = transpose(float3x3(T, B, N));
    
    // �V���h�E�̌v�Z
    // float4 _worldPos    = mul(World, localPos);
    float4 LvpPos       = mul(LightVP, worldPos);
    LvpPos.xyz          = LvpPos.xyz / LvpPos.w;
    output.PosSM.x      = (1.0f + LvpPos.x) / 2.0f;
    output.PosSM.y      = (1.0f - LvpPos.y) / 2.0f;
    output.PosSM.z      = LvpPos.z;

    return output;
}
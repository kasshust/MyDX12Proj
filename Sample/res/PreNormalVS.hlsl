#include "VSCommonBuffer.hlsli"

///////////////////////////////////////////////////////////////////////////////
// VSInput structure
///////////////////////////////////////////////////////////////////////////////
struct VSInput
{
    float3 Position : POSITION; // �ʒu���W�ł�.
    float3 Normal : NORMAL; // �@���x�N�g���ł�.
    float2 TexCoord : TEXCOORD; // �e�N�X�`�����W�ł�.
    float3 Tangent : TANGENT; // �ڐ��x�N�g���ł�.
};

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position             : SV_POSITION;  // �ʒu���W�ł�.
    float2 TexCoord             : TEXCOORD;     // �e�N�X�`�����W�ł�.
    float3 WorldPos             : WORLD_POS;    // ���[���h��Ԃ̈ʒu���W�ł�.
    float3x3 InvTangentBasis    : INV_TANGENT_BASIS; // �ڐ���Ԃւ̊��ϊ��s��̋t�s��ł�.
    float3 WorldNormal          : TEXCOORD1;
};


//-----------------------------------------------------------------------------
//      ���_�V�F�[�_�̃��C���G���g���[�|�C���g�ł�.
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
    
    // ���x�N�g��
    float3 N = normalize(mul((float3x3) World, input.Normal));
    float3 T = normalize(mul((float3x3) World, input.Tangent));
    float3 B = normalize(cross(N, T));

    // ���ϊ��s��̋t�s��.
    output.InvTangentBasis = transpose(float3x3(T, B, N));

    return output;
}
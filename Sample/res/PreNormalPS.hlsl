
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
    float4      Position        : SV_POSITION; // �ʒu���W�ł�.
    float2      TexCoord        : TEXCOORD; // �e�N�X�`�����W�ł�.
    float3      WorldPos        : WORLD_POS; // ���[���h��Ԃ̈ʒu���W�ł�.
    float3x3    InvTangentBasis : INV_TANGENT_BASIS; // �ڐ���Ԃւ̊��ϊ��s��̋t�s��ł�.
    float3      WorldNormal     : TEXCOORD1;
};

///////////////////////////////////////////////////////////////////////////////
// PSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct PSOutput
{
    float4 Color : SV_TARGET0; // �o�̓J���[�ł�.
};


//-----------------------------------------------------------------------------
//      �s�N�Z���V�F�[�_�̃��C���G���g���[�|�C���g�ł�.
//-----------------------------------------------------------------------------
PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput) 0;
    
    output.Color = float4( (input.WorldNormal.xyz + 1.0f.xxx) / 2.0f, 1.0f);
    return output;
}
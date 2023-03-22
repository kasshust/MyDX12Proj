//-----------------------------------------------------------------------------
// File : SphereToCubePS.hlsl
// Desc : Convert from sphere map to cube map.
// Copyright(c) Pocol. All right reserved.
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

//-----------------------------------------------------------------------------
// Textures and Samplers.
//-----------------------------------------------------------------------------
Texture2D       SphereMap : register(t0);
SamplerState    SphereSmp : register(s0);

//-----------------------------------------------------------------------------
//      ���C���G���g���[�|�C���g�ł�.
//-----------------------------------------------------------------------------
float4 main(const VSOutput input) : SV_TARGET0
{
    // �X�t�B�A�}�b�v���T���v�����܂�.
    return SphereMap.SampleLevel(SphereSmp, input.TexCoord, 0.0f);
}

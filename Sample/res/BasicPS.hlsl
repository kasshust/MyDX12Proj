//-----------------------------------------------------------------------------
// File : BasicPS.hlsl
// Desc : Pixel Shader.
// Copyright(c) Pocol. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "BRDF.hlsli"


///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4      Position        : SV_POSITION;          // �ʒu���W�ł�.
    float2      TexCoord        : TEXCOORD;             // �e�N�X�`�����W�ł�.
    float3      WorldPos        : WORLD_POS;            // ���[���h��Ԃ̈ʒu���W�ł�.
    float3x3    InvTangentBasis : INV_TANGENT_BASIS;    // �ڐ���Ԃւ̊��ϊ��s��̋t�s��ł�.
};

///////////////////////////////////////////////////////////////////////////////
// PSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct PSOutput
{
    float4  Color : SV_TARGET0;     // �o�̓J���[�ł�.
};

///////////////////////////////////////////////////////////////////////////////
// CbLight constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbLight : register(b1)
{
    float   TextureSize     : packoffset(c0);   // �e�N�X�`���T�C�Y�ł�.
    float   MipCount        : packoffset(c0.y); // �~�b�v�J�E���g�ł�.
    float   LightIntensity  : packoffset(c0.z); // ���C�g���x(�X�P�[���l).
    float3  LightDirection  : packoffset(c1);   // �f�B���N�V���i�����C�g�̕���.
};

///////////////////////////////////////////////////////////////////////////////
// CbCamera constant buffer.
///////////////////////////////////////////////////////////////////////////////
cbuffer CbCamera : register(b2)
{
    float3 CameraPosition : packoffset(c0);     // �J�����ʒu�ł�.
}

//-----------------------------------------------------------------------------
// Textures and Samplers
//-----------------------------------------------------------------------------
// DFG��.
Texture2D    DFGMap         : register(t0);
SamplerState DFGSmp         : register(s0);

// �f�B�t���[�YLD��.
TextureCube  DiffuseLDMap   : register(t1);
SamplerState DiffuseLDSmp   : register(s1);

// �X�y�L�����[LD��.
TextureCube  SpecularLDMap  : register(t2);
SamplerState SpecularLDSmp  : register(s2);

// �x�[�X�J���[�}�b�v.
Texture2D    BaseColorMap    : register(t3);
SamplerState BaseColorSmp    : register(s3);

// ���^���b�N�}�b�v.
Texture2D    MetallicMap     : register(t4);
SamplerState MetallicSmp     : register(s4);

// ���t�l�X�}�b�v.
Texture2D    RoughnessMap    : register(t5);
SamplerState RoughnessSmp    : register(s5);

// �@���}�b�v.
Texture2D    NormalMap       : register(t6);
SamplerState NormalSmp       : register(s6);


//-----------------------------------------------------------------------------
//      �X�y�L�����[�̎x�z�I�ȕ��������߂܂�.
//-----------------------------------------------------------------------------
float3 GetSpecularDomiantDir(float3 N, float3 R, float roughness)
{
    float smoothness = saturate(1.0f - roughness);
    float lerpFactor = smoothness * (sqrt(smoothness) + roughness);
    return lerp(N, R, lerpFactor);
}

//-----------------------------------------------------------------------------
//      �f�B�t���[�YIBL��]�����܂�.
//-----------------------------------------------------------------------------
float3 EvaluateIBLDiffuse(float3 N)
{
    // Lambert BRDF��DFG���͐ϕ������1.0�ƂȂ�̂ŁCLD���݂̂�ԋp����Ηǂ�
    return DiffuseLDMap.Sample(DiffuseLDSmp, N).rgb;
}

//-----------------------------------------------------------------------------
//      ���`���t�l�X����~�b�v���x�������߂܂�.
//-----------------------------------------------------------------------------
float RoughnessToMipLevel(float linearRoughness, float mipCount)
{
    return (mipCount - 1) * linearRoughness;
}

//-----------------------------------------------------------------------------
//      �X�y�L�����[IBL��]�����܂�.
//-----------------------------------------------------------------------------
float3 EvaluateIBLSpecular
(
    float           NdotV,          // �@���x�N�g���Ǝ����x�N�g���̓���.
    float3          N,              // �@���x�N�g��.
    float3          R,              // ���˃x�N�g��.
    float3          f0,             // �t���l����
    float           roughness,      // ���`���t�l�X.
    float           textureSize,    // �e�N�X�`���T�C�Y.
    float           mipCount        // �~�b�v���x����.
)
{
    float  a = roughness * roughness;
    float3 dominantR = GetSpecularDomiantDir(N, R, a);

    // �֐����č\�z.
    // L * D * (f0 * Gvis * (1 - Fc) + Gvis * Fc) * cosTheta / (4 * NdotL * NdotV).
    NdotV = max(NdotV, 0.5f / textureSize); // �[�����Z���������Ȃ��悤�ɂ���.
    float  mipLevel = RoughnessToMipLevel(roughness, mipCount);
    float3 preLD    = SpecularLDMap.SampleLevel(SpecularLDSmp, dominantR, mipLevel).xyz;
    
    // ���O�ϕ�����DFG���T���v������.
    // Fc = ( 1 - HdotL )^5
    // PreIntegratedDFG.r = Gvis * (1 - Fc)
    // PreIntegratedDFG.g = Gvis * Fc
    float2 preDFG   = DFGMap.SampleLevel(DFGSmp, float2(NdotV, roughness), 0).xy;

    // LD * (f0 * Gvis * (1 - Fc) + Gvis * Fc)
    return preLD * (f0 * preDFG.x + preDFG.y);
}


//-----------------------------------------------------------------------------
//      �s�N�Z���V�F�[�_�̃��C���G���g���[�|�C���g�ł�.
//-----------------------------------------------------------------------------
PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput)0;

    float3 V = normalize(input.WorldPos.xyz - CameraPosition);
    float3 N = NormalMap.Sample(NormalSmp, input.TexCoord).xyz * 2.0f - 1.0f;
    N = mul(input.InvTangentBasis, N);
    float3 R = normalize(reflect(V, N));

    float NV = saturate(dot(N, V));

    float3 baseColor = BaseColorMap.Sample(BaseColorSmp, input.TexCoord).rgb;
    float  metallic  = MetallicMap .Sample(MetallicSmp,  input.TexCoord).r;
    float  roughness = RoughnessMap.Sample(RoughnessSmp, input.TexCoord).r;

    float3 Kd = baseColor * (1.0f - metallic);
    float3 Ks = baseColor * metallic;

    float3 lit = 0;
    lit += EvaluateIBLDiffuse(N) * Kd;
    lit += EvaluateIBLSpecular(NV, N, R, Ks, roughness, TextureSize, MipCount);

    output.Color.rgb = lit * LightIntensity;
    output.Color.a   = 1.0f;

    return output;
}
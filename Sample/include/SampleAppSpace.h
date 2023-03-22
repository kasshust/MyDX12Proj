#pragma once

#include "DirectXHelpers.h"
#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

namespace SampleAppSpace {

    ///////////////////////////////////////////////////////////////////////////////
    // COLOR_SPACE_TYPE enum
    ///////////////////////////////////////////////////////////////////////////////
    enum COLOR_SPACE_TYPE
    {
        COLOR_SPACE_BT709,      // ITU-R BT.709
        COLOR_SPACE_BT2100_PQ,  // ITU-R BT.2100 PQ System.
    };

    ///////////////////////////////////////////////////////////////////////////////
    // TONEMAP_TYPE enum
    ///////////////////////////////////////////////////////////////////////////////
    enum TONEMAP_TYPE
    {
        TONEMAP_NONE = 0,   // �g�[���}�b�v�Ȃ�.
        TONEMAP_REINHARD,   // Reinhard�g�[���}�b�v.
        TONEMAP_GT,         // GT�g�[���}�b�v.
    };

    ///////////////////////////////////////////////////////////////////////////////
    // CbTonemap structure
    ///////////////////////////////////////////////////////////////////////////////
    struct alignas(256) CbTonemap
    {
        int     Type;               // �g�[���}�b�v�^�C�v.
        int     ColorSpace;         // �o�͐F���.
        float   BaseLuminance;      // ��P�x�l[nit].
        float   MaxLuminance;       // �ő�P�x�l[nit].
    };

    ///////////////////////////////////////////////////////////////////////////////
    // CbMesh structure
    ///////////////////////////////////////////////////////////////////////////////
    struct alignas(256) CbMesh
    {
        Matrix   World;      //!< ���[���h�s��ł�.
    };

    ///////////////////////////////////////////////////////////////////////////////
    // CbTransform structure
    ///////////////////////////////////////////////////////////////////////////////
    struct alignas(256) CbTransform
    {
        Matrix   View;       //!< �r���[�s��ł�.
        Matrix   Proj;       //!< �ˉe�s��ł�.
    };

    ///////////////////////////////////////////////////////////////////////////////
    // CbLight structure
    ///////////////////////////////////////////////////////////////////////////////
    struct alignas(256) CbLight
    {
        float   TextureSize;      //!< �L���[�u�}�b�v�T�C�Y�ł�.
        float   MipCount;         //!< �~�b�v���ł�.
        float   LightIntensity;   //!< ���C�g���x�ł�.
        float   Padding0;         //!< �p�f�B���O.
        Vector3 LightDirection;   //!< �f�B���N�V���i�����C�g�̕���.
    };

    ///////////////////////////////////////////////////////////////////////////////
    // CbCamera structure
    ///////////////////////////////////////////////////////////////////////////////
    struct alignas(256) CbCamera
    {
        Vector3  CameraPosition;    //!< �J�����ʒu�ł�.
    };

    ///////////////////////////////////////////////////////////////////////////////
    // CbMaterial structure
    ///////////////////////////////////////////////////////////////////////////////
    struct alignas(256) CbMaterial
    {
        Vector3 BaseColor;  //!< ��{�F.
        float   Alpha;      //!< ���ߓx.
        float   Roughness;  //!< �ʂ̑e���ł�(�͈͂�[0,1]).
        float   Metallic;   //!< �����x�ł�(�͈͂�[0,1]).
    };

    //-----------------------------------------------------------------------------
    //      �F�x���擾����.
    //-----------------------------------------------------------------------------
    inline UINT16 GetChromaticityCoord(double value)
    {
        return UINT16(value * 50000);
    }

} // namespace
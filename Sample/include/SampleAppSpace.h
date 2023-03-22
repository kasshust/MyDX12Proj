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
        TONEMAP_NONE = 0,   // トーンマップなし.
        TONEMAP_REINHARD,   // Reinhardトーンマップ.
        TONEMAP_GT,         // GTトーンマップ.
    };

    ///////////////////////////////////////////////////////////////////////////////
    // CbTonemap structure
    ///////////////////////////////////////////////////////////////////////////////
    struct alignas(256) CbTonemap
    {
        int     Type;               // トーンマップタイプ.
        int     ColorSpace;         // 出力色空間.
        float   BaseLuminance;      // 基準輝度値[nit].
        float   MaxLuminance;       // 最大輝度値[nit].
    };

    ///////////////////////////////////////////////////////////////////////////////
    // CbMesh structure
    ///////////////////////////////////////////////////////////////////////////////
    struct alignas(256) CbMesh
    {
        Matrix   World;      //!< ワールド行列です.
    };

    ///////////////////////////////////////////////////////////////////////////////
    // CbTransform structure
    ///////////////////////////////////////////////////////////////////////////////
    struct alignas(256) CbTransform
    {
        Matrix   View;       //!< ビュー行列です.
        Matrix   Proj;       //!< 射影行列です.
    };

    ///////////////////////////////////////////////////////////////////////////////
    // CbLight structure
    ///////////////////////////////////////////////////////////////////////////////
    struct alignas(256) CbLight
    {
        float   TextureSize;      //!< キューブマップサイズです.
        float   MipCount;         //!< ミップ数です.
        float   LightIntensity;   //!< ライト強度です.
        float   Padding0;         //!< パディング.
        Vector3 LightDirection;   //!< ディレクショナルライトの方向.
    };

    ///////////////////////////////////////////////////////////////////////////////
    // CbCamera structure
    ///////////////////////////////////////////////////////////////////////////////
    struct alignas(256) CbCamera
    {
        Vector3  CameraPosition;    //!< カメラ位置です.
    };

    ///////////////////////////////////////////////////////////////////////////////
    // CbMaterial structure
    ///////////////////////////////////////////////////////////////////////////////
    struct alignas(256) CbMaterial
    {
        Vector3 BaseColor;  //!< 基本色.
        float   Alpha;      //!< 透過度.
        float   Roughness;  //!< 面の粗さです(範囲は[0,1]).
        float   Metallic;   //!< 金属度です(範囲は[0,1]).
    };

    //-----------------------------------------------------------------------------
    //      色度を取得する.
    //-----------------------------------------------------------------------------
    inline UINT16 GetChromaticityCoord(double value)
    {
        return UINT16(value * 50000);
    }

} // namespace
﻿//-----------------------------------------------------------------------------
// File : Material.h
// Desc : Material Module.
// Copyright(c) Pocol. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <DescriptorPool.h>
#include <ResourceUploadBatch.h>
#include <Texture.h>
#include <ConstantBuffer.h>
#include <map>


///////////////////////////////////////////////////////////////////////////////
// Material class
///////////////////////////////////////////////////////////////////////////////
class Material
{
    //=========================================================================
    // list of friend classes and method.
    //=========================================================================
    /* NOTHING */

public:
    ///////////////////////////////////////////////////////////////////////////
    // TEXTURE_USAGE enum
    ///////////////////////////////////////////////////////////////////////////
    enum TEXTURE_USAGE
    {
        TEXTURE_USAGE_DIFFUSE = 0,  //!< ディフューズマップとして利用します.
        TEXTURE_USAGE_SPECULAR,     //!< スペキュラーマップとして利用します.
        TEXTURE_USAGE_SHININESS,    //!< シャイネスマップとして利用します.
        TEXTURE_USAGE_NORMAL,       //!< 法線マップとして利用します.

        TEXTURE_USAGE_BASE_COLOR,   //!< ベースカラーマップとして利用します.
        TEXTURE_USAGE_METALLIC,     //!< メタリックマップとして利用します.
        TEXTURE_USAGE_ROUGHNESS,    //!< ラフネスマップとして利用します.

        TEXTURE_USAGE_COUNT
    };

    //=========================================================================
    // public variables.
    //=========================================================================
    /* NOTHING */

    //=========================================================================
    // public methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //-------------------------------------------------------------------------
    Material();

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    ~Material();

    //-------------------------------------------------------------------------
    //! @brief      初期化処理を行います.
    //!
    //! @param[in]      pDevice         デバイスです.
    //! @param[in]      pPool           ディスクリプタプールです(CBV_UAV_SRV用のものを設定します).
    //! @param[in]      bufferSize      1マテリアルあたりの定数バッファのサイズです.
    //! @param[in]      count           マテリアル数です.
    //! @retval true    初期化に成功.
    //! @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    bool Init(
        ID3D12Device*   pDevice,
        DescriptorPool* pPool,
        size_t          bufferSize,
        size_t          count);

    //-------------------------------------------------------------------------
    //! @brief      終了処理を行います.
    //-------------------------------------------------------------------------
    void Term();

    //-------------------------------------------------------------------------
    //! @brief      テクスチャを設定します.
    //!
    //! @param[in]      index       マテリアル番号です.
    //! @param[in]      usage       テクスチャの使用用途です.
    //! @param[in]      path        テクスチャパスです.
    //! @param[out]     batch       リソースアップロードバッチです.
    //! @retval true    設定に成功.
    //! @retval false   設定に失敗.
    //-------------------------------------------------------------------------
    bool SetTexture(
        size_t                          index,
        TEXTURE_USAGE                   usage,
        const std::wstring&             path,
        DirectX::ResourceUploadBatch&   batch);

    //-------------------------------------------------------------------------
    //! @brief      定数バッファのポインタを取得します.
    //!
    //! @param[in]      index       取得するマテリアル番号です.
    //! @return     指定された番号に一致する定数バッファのポインタを返却します.
    //!             一致するものが無い場合は nullptr を返却します.
    //-------------------------------------------------------------------------
    void* GetBufferPtr(size_t index) const;

    //-------------------------------------------------------------------------
    //! @brief      定数バッファのポインタを取得します.
    //!
    //! @param[in]      index       取得するマテリアル番号です.
    //! @return     指定された番号に一致する定数バッファのポインタを返却します.
    //!             一致するものが無い場合は nullptr を返却します.
    //-------------------------------------------------------------------------
    template<typename T>
    T* GetBufferPtr(size_t index) const
    { return reinterpret_cast<T*>(GetBufferPtr(index)); }

    //-------------------------------------------------------------------------
    //! @brief      定数バッファのGPU仮想アドレスを取得します.
    //!
    //! @param[in]      index       取得するマテリアル番号です.
    //! @return     指定された番号に一致する定数バッファのGPU仮想アドレスを返却します.
    //-------------------------------------------------------------------------
    D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress(size_t index) const;

    //-------------------------------------------------------------------------
    //! @brief      バッファハンドルを取得します.
    //!
    //! @param[in]      index       取得するマテリアル番号です.
    //! @return     指定された番号に一致する定数バッファのディスクリプタハンドルを返却します.
    //-------------------------------------------------------------------------
    D3D12_GPU_DESCRIPTOR_HANDLE GetBufferHandle(size_t index) const;

    //-------------------------------------------------------------------------
    //! @brief      テクスチャハンドルを取得します.
    //!
    //! @param[in]      index       取得するマテリアル番号です.
    //! @param[in]      usage       取得するテクスチャの使用用途です.
    //! @return     指定された番号に一致するテクスチャのディスクリプタハンドルを返却します.
    //-------------------------------------------------------------------------
    D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(size_t index, TEXTURE_USAGE usage) const;

    //-------------------------------------------------------------------------
    //! @brief      マテリアル数を取得します.
    //!
    //! @return     マテリアル数を返却します.
    //-------------------------------------------------------------------------
    size_t GetCount() const;

private:
    ///////////////////////////////////////////////////////////////////////////
    // Subset structure
    ///////////////////////////////////////////////////////////////////////////
    struct Subset
    {
        ConstantBuffer*                 pCostantBuffer;                     //!< 定数バッファです.
        D3D12_GPU_DESCRIPTOR_HANDLE     TextureHandle[TEXTURE_USAGE_COUNT]; //!< テクスチャハンドルです.
    };

    //=========================================================================
    // private variables.
    //=========================================================================
    std::map<std::wstring, Texture*>    m_pTexture;     //!< テクスチャです.
    std::vector<Subset>                 m_Subset;       //!< サブセットです.
    ID3D12Device*                       m_pDevice;      //!< デバイスです.
    DescriptorPool*                     m_pPool;        //!< ディスクリプタプールです(CBV_UAV_SRV).

    //=========================================================================
    // private methods.
    //=========================================================================
    Material        (const Material&) = delete;
    void operator = (const Material&) = delete;
};

constexpr auto TU_DIFFUSE    = Material::TEXTURE_USAGE_DIFFUSE;
constexpr auto TU_SPECULAR   = Material::TEXTURE_USAGE_SPECULAR;
constexpr auto TU_SHININESS  = Material::TEXTURE_USAGE_SHININESS;
constexpr auto TU_NORMAL     = Material::TEXTURE_USAGE_NORMAL;

constexpr auto TU_BASE_COLOR = Material::TEXTURE_USAGE_BASE_COLOR;
constexpr auto TU_METALLIC   = Material::TEXTURE_USAGE_METALLIC;
constexpr auto TU_ROUGHNESS  = Material::TEXTURE_USAGE_ROUGHNESS;

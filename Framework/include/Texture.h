﻿//-----------------------------------------------------------------------------
// File : Texture.h
// Desc : Texture Module.
// Copyright(c) Pocol. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <d3d12.h>
#include <ComPtr.h>
#include <ResourceUploadBatch.h>

//-----------------------------------------------------------------------------
// Forward Declarations.
//-----------------------------------------------------------------------------
class DescriptorHandle;
class DescriptorPool;

///////////////////////////////////////////////////////////////////////////////
// Texture class
///////////////////////////////////////////////////////////////////////////////
class Texture
{
	//=========================================================================
	// list of friend classes and methods.
	//=========================================================================
	/* NOTHING */

public:
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
	Texture();

	//-------------------------------------------------------------------------
	//! @brief      デストラクタです.
	//-------------------------------------------------------------------------
	~Texture();

	//-------------------------------------------------------------------------
	//! @brief      初期化処理を行います.
	//!
	//! @param[in]      pDevice     デバイスです.
	//! @param[in]      pPool       ディスクリプタプールです.
	//! @param[in]      filename    ファイル名です.
	//! @param[out]     batch       更新バッチです. テクスチャの更新に必要なデータを格納します.
	//! @retval true    初期化に成功.
	//! @retval false   初期化に失敗.
	//-------------------------------------------------------------------------
	bool Init(
		ID3D12Device* pDevice,
		DescriptorPool* pPool,
		const wchar_t* filename,
		DirectX::ResourceUploadBatch& batch);

	//-------------------------------------------------------------------------
	//! @brief      初期化処理を行います.
	//!
	//! @param[in]      pDevice     デバイスです.
	//! @param[in]      pPool       ディスクリプタプールです.
	//! @param[in]      filename    ファイル名です.
	//! @param[in]      isSRGB      SRGBフォーマットを利用する場合は true を指定します.
	//! @param[out]     batch       更新バッチです. テクスチャの更新に必要なデータを格納します.
	//! @retval true    初期化に成功.
	//! @retval false   初期化に失敗.
	//-------------------------------------------------------------------------
	bool Init(
		ID3D12Device* pDevice,
		DescriptorPool* pPool,
		const wchar_t* filename,
		bool isSRGB,
		DirectX::ResourceUploadBatch& batch);

	//-------------------------------------------------------------------------
	//! @brief      初期化処理を行います.
	//!
	//! @param[in]      pDevice     デバイスです.
	//! @param[in]      pPool       ディスクリプタプールです
	//! @param[in]      pDesc       構成設定です.
	//! @param[in]      isCube      キューブマップである場合には true を指定します.
	//! @retval true    初期化に成功.
	//! @retval false   初期化に失敗.
	//-------------------------------------------------------------------------
	bool Init(
		ID3D12Device* pDevice,
		DescriptorPool* pPool,
		const D3D12_RESOURCE_DESC* pDesc,
		D3D12_RESOURCE_STATES       initState,
		bool                        isCube);

	//-------------------------------------------------------------------------
	//! @brief      初期化処理を行います.
	//!
	//! @param[in]      pDevice     デバイスです.
	//! @param[in]      pPool       ディスクリプタプールです
	//! @param[in]      pDesc       構成設定です.
	//! @param[in]      isCube      キューブマップである場合には true を指定します.
	//! @param[in]      isSRGB      SRGBフォーマットを利用する場合は true を指定します.
	//! @retval true    初期化に成功.
	//! @retval false   初期化に失敗.
	//-------------------------------------------------------------------------
	bool Init(
		ID3D12Device* pDevice,
		DescriptorPool* pPool,
		const D3D12_RESOURCE_DESC* pDesc,
		D3D12_RESOURCE_STATES       initState,
		bool                        isCube,
		bool                        isSRGB);

	//-------------------------------------------------------------------------
	//! @brief      終了処理を行います.
	//-------------------------------------------------------------------------
	void Term();

	//-------------------------------------------------------------------------
	//! @brief      CPUディスクリプタハンドルを取得します.
	//!
	//! @return     CPUディスクリプタハンドルを返却します.
	//-------------------------------------------------------------------------
	D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;

	//-------------------------------------------------------------------------
	//! @brief      GPUディスクリプタハンドルを取得します.
	//!
	//! @return     GPUディスクリプタハンドルを返却します.
	//-------------------------------------------------------------------------
	D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;

	//-------------------------------------------------------------------------
	//! @brief      リソースを取得します.
	//!
	//! @return     リソースを返却します.
	//-------------------------------------------------------------------------
	ID3D12Resource* GetResource() const;

private:
	//=========================================================================
	// private variables.
	//=========================================================================
	ComPtr<ID3D12Resource>  m_pTex;
	DescriptorHandle*		m_pHandle;
	DescriptorPool*			m_pPool;

	//=========================================================================
	// private methods.
	//=========================================================================
	Texture(const Texture&) = delete;      // アクセス禁止.
	void operator = (const Texture&) = delete;      // アクセス禁止.

	//-------------------------------------------------------------------------
	//! @brief      シェーダリソースビューの設定を求めます.
	//!
	//! @param[in]      isCube      キューブマップかどうか?
	//! @return     シェーダリソースビューの設定を返却します.
	//-------------------------------------------------------------------------
	D3D12_SHADER_RESOURCE_VIEW_DESC GetViewDesc(bool isCube);
};

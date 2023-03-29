#pragma once
#include "DirectXHelpers.h"
#include "Logger.h"
#include <App.h>
#include <ConstantBuffer.h>
#include <RootSignature.h>

class PostEffect {
public:
	virtual bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format) = 0;
	virtual void Term() = 0;

protected:

	ConstantBuffer                  m_CB[App::FrameCount];	//!< 定数バッファです.
	ComPtr<ID3D12PipelineState>     m_pPSO;                  //!< トーンマップ用パイプラインステートです.
	RootSignature                   m_RootSig;               //!< トーンマップ用ルートシグニチャです.

	virtual bool CreateRootSig(ComPtr<ID3D12Device> pDevice) = 0;
	virtual bool CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format) = 0;
	virtual bool CreateConstantBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool) = 0;

};
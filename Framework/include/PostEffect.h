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

	ConstantBuffer                  m_CB[App::FrameCount];	//!< �萔�o�b�t�@�ł�.
	ComPtr<ID3D12PipelineState>     m_pPSO;                  //!< �g�[���}�b�v�p�p�C�v���C���X�e�[�g�ł�.
	RootSignature                   m_RootSig;               //!< �g�[���}�b�v�p���[�g�V�O�j�`���ł�.

	virtual bool CreateRootSig(ComPtr<ID3D12Device> pDevice) = 0;
	virtual bool CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format) = 0;
	virtual bool CreateConstantBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool) = 0;

};
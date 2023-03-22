#pragma once
#include "DirectXHelpers.h"
#include "Logger.h"
#include <App.h>
#include <ConstantBuffer.h>
#include <RootSignature.h>
#include <SampleAppSpace.h>

class ToneMap {

public:
	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format);
	
	ToneMap();
	~ToneMap();
	void Term();
	void DrawTonemap(ID3D12GraphicsCommandList* pCmd, int frameindex, ColorTarget& colortarget, D3D12_VIEWPORT* viewport, D3D12_RECT* scissor, VertexBuffer& vb);
	void SetLuminance(float base, float max);
	void SetToneMapType(SampleAppSpace::TONEMAP_TYPE t);

private:
	int                             m_TonemapType;                  //!< �g�[���}�b�v�^�C�v.
	int                             m_ColorSpace;                   //!< �o�͐F���
	float                           m_BaseLuminance;                //!< ��P�x�l.
	float                           m_MaxLuminance;                 //!< �ő�P�x�l.

	ConstantBuffer                  m_TonemapCB[App::FrameCount];	//!< �萔�o�b�t�@�ł�.
	ComPtr<ID3D12PipelineState>     m_pTonemapPSO;                  //!< �g�[���}�b�v�p�p�C�v���C���X�e�[�g�ł�.
	RootSignature                   m_TonemapRootSig;               //!< �g�[���}�b�v�p���[�g�V�O�j�`���ł�.

	bool CreateToneMapRootSig(ComPtr<ID3D12Device> pDevice);
	bool CreateToneMapPipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format);
	bool CreateToneMapConstantBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool);
};
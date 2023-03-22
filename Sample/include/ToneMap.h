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
	int                             m_TonemapType;                  //!< トーンマップタイプ.
	int                             m_ColorSpace;                   //!< 出力色空間
	float                           m_BaseLuminance;                //!< 基準輝度値.
	float                           m_MaxLuminance;                 //!< 最大輝度値.

	ConstantBuffer                  m_TonemapCB[App::FrameCount];	//!< 定数バッファです.
	ComPtr<ID3D12PipelineState>     m_pTonemapPSO;                  //!< トーンマップ用パイプラインステートです.
	RootSignature                   m_TonemapRootSig;               //!< トーンマップ用ルートシグニチャです.

	bool CreateToneMapRootSig(ComPtr<ID3D12Device> pDevice);
	bool CreateToneMapPipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format);
	bool CreateToneMapConstantBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool);
};
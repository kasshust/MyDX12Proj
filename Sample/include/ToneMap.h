#pragma once
#include "DirectXHelpers.h"
#include "Logger.h"
#include <App.h>
#include <ConstantBuffer.h>
#include <RootSignature.h>

class ToneMap {
public:
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

	//-----------------------------------------------------------------------------
	//      色度を取得する.
	//-----------------------------------------------------------------------------
	inline static UINT16 GetChromaticityCoord(double value)
	{
		return UINT16(value * 50000);
	}

public:
	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format);

	ToneMap();
	~ToneMap();
	void Term();
	void DrawTonemap(ID3D12GraphicsCommandList* pCmd, int frameindex, ColorTarget& colorDest, DepthTarget& depthDest, ColorTarget& colorSource, D3D12_VIEWPORT* viewport, D3D12_RECT* scissor, VertexBuffer& vb);
	void SetLuminance(float base, float max);
	void SetToneMapType(TONEMAP_TYPE t);
	int                             m_TonemapType;              //!< トーンマップタイプ.

private:

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
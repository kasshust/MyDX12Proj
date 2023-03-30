#pragma once
#include "DirectXHelpers.h"
#include "Logger.h"
#include <App.h>
#include <ConstantBuffer.h>
#include <RootSignature.h>
#include <PostEffect.h>

class ToneMap : public PostEffect {
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

	//-----------------------------------------------------------------------------
	//      �F�x���擾����.
	//-----------------------------------------------------------------------------
	inline static UINT16 GetChromaticityCoord(double value)
	{
		return UINT16(value * 50000);
	}

public:
	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format) override;

	void Term() override;
	void DrawTonemap(ID3D12GraphicsCommandList* pCmd, int frameindex, ColorTarget& colorDest, DepthTarget& depthDest, ColorTarget& colorSource, D3D12_VIEWPORT* viewport, D3D12_RECT* scissor, VertexBuffer& vb);
	void SetLuminance(float base, float max);
	int                             m_TonemapType;              //!< �g�[���}�b�v�^�C�v.

private:

	const wchar_t* m_VSPath = L"QuadVS.cso";
	const wchar_t* m_PSPath = L"TonemapPS.cso";

	int                             m_ColorSpace;                   //!< �o�͐F���
	float                           m_BaseLuminance;                //!< ��P�x�l.
	float                           m_MaxLuminance;                 //!< �ő�P�x�l.

	bool CreateRootSig(ComPtr<ID3D12Device> pDevice) override;
	bool CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)  override;
};
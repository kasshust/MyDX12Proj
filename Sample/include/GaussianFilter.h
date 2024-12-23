#pragma once
#include "DirectXHelpers.h"
#include "Logger.h"
#include <App.h>
#include <ConstantBuffer.h>
#include <RootSignature.h>
#include <PostEffect.h>

// ガウシアンフィルター
class GaussianFilter : public PostEffect {
public:
	struct alignas(256) CbGaussianFilter
	{
		float ReductionRatio;
	};

	struct DrawSource {
		ColorTarget&	ColorDest;
		ColorTarget&	ColorSource;
		VertexBuffer&	VertexBuffer;
		int				index = 0;
		float			ReductionRatio = 1.0f;
		
	};

public:
	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format) override;
	void Term() override;
	void Draw(ID3D12GraphicsCommandList* pCmd, int frameindex, DrawSource& s);
private:

	ConstantBuffer                  m_SCB[4][App::FrameCount];

	const wchar_t* m_VSPath = L"QuadVS.cso";
	const wchar_t* m_PSPath = L"GaussianFilterPS.cso";

	bool CreateRootSig(ComPtr<ID3D12Device> pDevice) override;
	bool CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)  override;
};
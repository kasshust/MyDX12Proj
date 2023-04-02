#pragma once
#include "DirectXHelpers.h"
#include "Logger.h"
#include <App.h>
#include <ConstantBuffer.h>
#include <RootSignature.h>
#include <PostEffect.h>

// �J���[�^�[�Q�b�g�ƃu���[���^�[�Q�b�g������
class BloomComposition: public PostEffect {
public:
	struct alignas(256) CbBloomConposition
	{
	};

	struct DrawSource {
		ColorTarget&	ColorDest;
		ColorTarget&    ColorBaseSources;
		ColorTarget		(&ColorSources)[4];
		VertexBuffer&	VertexBuffer;
	};

public:
	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format) override;
	void Term() override;
	void Draw(ID3D12GraphicsCommandList* pCmd, int frameindex, DrawSource& s);
private:

	const wchar_t* m_VSPath = L"QuadVS.cso";
	const wchar_t* m_PSPath = L"BloomCompositionPS.cso";

	bool CreateRootSig(ComPtr<ID3D12Device> pDevice) override;
	bool CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)  override;
};
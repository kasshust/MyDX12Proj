#pragma once
#include "DirectXHelpers.h"
#include "Logger.h"
#include <App.h>
#include <ConstantBuffer.h>
#include <RootSignature.h>
#include <Renderer.h>

class PreDepthRenderer : public Renderer {
public:
	struct DrawSource {
		DepthTarget& DepthDest;
	};

public:
	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format);
	void Term() override;
	void Draw(ID3D12GraphicsCommandList* pCmd, int frameindex, DrawSource& s);
private:

	const wchar_t* m_VSPath = L"PreNormalVS.cso";
	const wchar_t* m_PSPath = L"PreNormalPS.cso";

	bool CreateRootSig(ComPtr<ID3D12Device> pDevice) override;
	bool CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)  override;
};
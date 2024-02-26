#pragma once
#include "DirectXHelpers.h"
#include "Logger.h"
#include <App.h>
#include <ConstantBuffer.h>
#include <RootSignature.h>
#include <Renderer.h>
#include <CommonBufferManager.h>
#include <GameObject.h>

class PreNormalRenderer : public Renderer{
public:
	struct alignas(256) CbPreNormal
	{
	};

	struct DrawSource {
		ColorTarget& ColorDest;
		DepthTarget& DepthDest;
		const CommonBufferManager& Commonbufmanager;
		const std::vector<GameObject*>& GameObjects;
	};

public:
	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format);
	void Term() override;
	void Draw(ID3D12GraphicsCommandList* pCmd, int frameindex, DrawSource& s);
private:

	const wchar_t* m_VSPath = L"PreNormalVS.cso";
	const wchar_t* m_PSPath = L"PreNormalPS.cso";
	ConstantBuffer	m_CB[App::FrameCount];

	bool CreateRootSig(ComPtr<ID3D12Device> pDevice) override;
	bool CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)  override;
};
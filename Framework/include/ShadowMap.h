#pragma once
#include "DirectXHelpers.h"
#include "Logger.h"
#include <App.h>
#include <ConstantBuffer.h>
#include <RootSignature.h>
#include <Renderer.h>
#include <CommonBufferManager.h>
#include <GameObject.h>

class ShadowMap : public Renderer {
public:
	struct alignas(256) CbDepthShadowMap
	{
		Matrix	LightVP;
	};

	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format);
	void Term();
	void DrawShadowMap(
		ID3D12GraphicsCommandList* pCmd,
		DepthTarget& depthDest,
		D3D12_VIEWPORT* viewport,
		D3D12_RECT* scissor,
		int frameindex,
		const CommonBufferManager& commonbufmanager,
		const std::vector<GameObject*> gameObjects,
		const Vector3				lightDirection
	);
	
protected:

	const wchar_t*	m_VSPath = L"DepthShadow.cso";
	const wchar_t*	m_PSPath = L"";
	ConstantBuffer	m_CB[App::FrameCount];		

	bool CreateRootSig(ComPtr<ID3D12Device> pDevice) override;
	void UpdateConstantBuffer(int frameindex, Vector3 lighrDir);
	bool CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format) override;
};

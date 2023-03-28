#pragma once
#include "DirectXHelpers.h"
#include "Logger.h"
#include <App.h>
#include <ConstantBuffer.h>
#include <RootSignature.h>
#include <Material.h>
#include <CommonBufferManager.h>
#include <IBLBaker.h>
#include <Shader.h>

class BasicShader : public Shader {
public:
	void SetShader(
		ID3D12GraphicsCommandList* pCmd,
		int frameindex, 
		Material& mat, 
		int id, 
		const ConstantBuffer* meshCB,
		const CommonBufferManager& commonbufmanager,
		const IBLBaker& baker
	) override;

protected:

	const wchar_t* m_VSPath = L"BasicVS.cso";
	const wchar_t* m_PSPath = L"BasicPS.cso";

	bool CreateRootSig(ComPtr<ID3D12Device> pDevice) override;
	bool CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat) override;
	
};
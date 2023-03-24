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
#include <ModelLoader.h>

class BasicShader : public Shader {
public:
	virtual void SetShader(
		ID3D12GraphicsCommandList* pCmd,
		int frameindex,
		const Material& mat,
		int id,
		const CommonBufferManager& commonbufmanager,
		const IBLBaker& baker,
		const ModelLoader& loader
	);

	virtual void SetShader(
		ID3D12GraphicsCommandList* pCmd,
		int frameindex,
		const Material& mat,
		int id,
		CommonBufferManager& commonbufmanager,
		const IBLBaker& baker,
		const ConstantBuffer& buf) override;

protected:

	const wchar_t* m_VSPath = L"BasicVS.cso";
	const wchar_t* m_PSPath = L"BasicPS.cso";

	bool CreateRootSig(ComPtr<ID3D12Device> pDevice) override;
	bool CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat) override;
};
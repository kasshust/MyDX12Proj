#pragma once

#include "DirectXHelpers.h"
#include "Logger.h"
#include <App.h>
#include <ConstantBuffer.h>
#include <RootSignature.h>
#include <IBLBaker.h>
#include <CommonBufferManager.h>
#include <Material.h>
#include <SkyTextureManager.h>

class Material;
class CommonBufferManager;

class Shader {
public:
	bool Init(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);
	void Term();
	virtual void SetShader(
		ID3D12GraphicsCommandList* pCmd,
		int frameindex,
		Material& mat,
		int id,
		const ConstantBuffer* meshCB,
		const CommonBufferManager& commonbufmanager,
		const SkyManager& manager
	)  = 0;

protected:
	ComPtr<ID3D12PipelineState>     m_pPSO;                    //!< シーン用パイプラインステートです.
	RootSignature                   m_RootSig;                 //!< シーン用ルートシグニチャです.
	virtual bool CreateRootSig(ComPtr<ID3D12Device> pDevice) { return false; };
	virtual bool CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat) { return false; };
};
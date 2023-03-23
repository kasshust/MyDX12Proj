#pragma once
#include "DirectXHelpers.h"
#include "Logger.h"
#include <App.h>
#include <ConstantBuffer.h>
#include <RootSignature.h>
#include <Material.h>
#include <CommonBufferManager.h>
#include <IBLBaker.h>

class BasicShader {
public:
	bool Init(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);
	void SetShader(ID3D12GraphicsCommandList* pCmd, int frameindex, Material& mat, int id, CommonBufferManager& commonbufmanager, IBLBaker& baker);
	void Term();

private:
	ComPtr<ID3D12PipelineState>     m_pBasicPSO;                    //!< シーン用パイプラインステートです.
	RootSignature                   m_BasicRootSig;                 //!< シーン用ルートシグニチャです.
	bool CreateBasicRootSig(ComPtr<ID3D12Device> pDevice);
	bool CreateBasicPipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);



};
#pragma once
#include <App.h>
#include <RootSignature.h>
#include <ConstantBuffer.h>

class Renderer {

protected:
	ComPtr<ID3D12PipelineState>     m_pPSO;
	RootSignature                   m_RootSig;

	virtual bool CreateRootSig(ComPtr<ID3D12Device> pDevice) = 0;
	virtual bool CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format) = 0;

};

bool SearchAndLoadShader(const std::wstring& shaderPath, ComPtr<ID3DBlob>& shaderBlob);
bool CreateGraphicsPipelineState(ComPtr<ID3D12Device> pDevice, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, ComPtr<ID3D12PipelineState>& pPSO);
bool InitRootSignature(ComPtr<ID3D12Device> pDevice, const RootSignature::Desc& desc, RootSignature& rootSig);
bool CreateConstantBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, ConstantBuffer* cb, size_t bufferSize);

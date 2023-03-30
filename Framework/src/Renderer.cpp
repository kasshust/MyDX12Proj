#include "Renderer.h"
#include "FileUtil.h"
#include "Logger.h"
#include "DirectXHelpers.h"
#include <d3dcompiler.h>
#include <RootSignature.h>
#include <ConstantBuffer.h>

bool SearchAndLoadShader(const std::wstring& shaderPath, ComPtr<ID3DBlob>& shaderBlob)
{
    std::wstring fullPath;
    if (!SearchFilePath(shaderPath.c_str(), fullPath)) {
        ELOG("Error : %ls Shader Not Found.", shaderPath.c_str());
        return false;
    }

    HRESULT hr = D3DReadFileToBlob(fullPath.c_str(), shaderBlob.GetAddressOf());
    if (FAILED(hr)) {
        ELOG("Error : D3DReadFileToBlob() Failed. path = %ls", fullPath.c_str());
        return false;
    }

    ELOG("Shader Loaded. Path = %ls", fullPath.c_str());
    return true;
}

bool CreateGraphicsPipelineState(ComPtr<ID3D12Device> pDevice, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, ComPtr<ID3D12PipelineState>& pPSO)
{
    HRESULT hr = pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pPSO.GetAddressOf()));
    if (FAILED(hr))
    {
        ELOG("Error : ID3D12Device::CreateGraphicsPipelineState() Failed. retcode = 0x%x", hr);
        return false;
    }

    return true;
}

bool InitRootSignature(ComPtr<ID3D12Device> pDevice, const RootSignature::Desc& desc, RootSignature& rootSig)
{
    if (!rootSig.Init(pDevice.Get(), desc.GetDesc()))
    {
        ELOG("Error : RootSignature::Init() Failed.");
        return false;
    }

    return true;
}

bool CreateConstantBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, ConstantBuffer* cb, size_t bufferSize)
{
    for (auto i = 0; i < App::FrameCount; ++i)
    {
        if (!cb[i].Init(pDevice.Get(), pool, bufferSize))
        {
            ELOG("Error : ConstantBuffer::Init() Failed.");
            return false;
        }
    }
    return true;
}

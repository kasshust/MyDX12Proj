#include "BasicShader.h"
#include "Logger.h"
#include "DirectXHelpers.h"
#include <CommonStates.h>
#include <FileUtil.h>

bool BasicShader::CreateBasicRootSig(ComPtr<ID3D12Device> pDevice) {
    RootSignature::Desc desc;
    desc.Begin(11)
        .SetCBV(ShaderStage::VS, 0, 0)
        .SetCBV(ShaderStage::VS, 1, 1)
        .SetCBV(ShaderStage::PS, 2, 1)
        .SetCBV(ShaderStage::PS, 3, 2)
        .SetSRV(ShaderStage::PS, 4, 0)
        .SetSRV(ShaderStage::PS, 5, 1)
        .SetSRV(ShaderStage::PS, 6, 2)
        .SetSRV(ShaderStage::PS, 7, 3)
        .SetSRV(ShaderStage::PS, 8, 4)
        .SetSRV(ShaderStage::PS, 9, 5)
        .SetSRV(ShaderStage::PS, 10, 6)
        .AddStaticSmp(ShaderStage::PS, 0, SamplerState::LinearWrap)
        .AddStaticSmp(ShaderStage::PS, 1, SamplerState::LinearWrap)
        .AddStaticSmp(ShaderStage::PS, 2, SamplerState::LinearWrap)
        .AddStaticSmp(ShaderStage::PS, 3, SamplerState::LinearWrap)
        .AddStaticSmp(ShaderStage::PS, 4, SamplerState::LinearWrap)
        .AddStaticSmp(ShaderStage::PS, 5, SamplerState::LinearWrap)
        .AddStaticSmp(ShaderStage::PS, 6, SamplerState::LinearWrap)
        .AllowIL()
        .End();

    if (!m_BasicRootSig.Init(pDevice.Get(), desc.GetDesc()))
    {
        ELOG("Error : RootSignature::Init() Failed.");
        return false;
    }

    return true;
}

bool BasicShader::CreateBasicPipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat) {
    std::wstring vsPath;
    std::wstring psPath;

    // 頂点シェーダを検索.
    if (!SearchFilePath(L"BasicVS.cso", vsPath))
    {
        ELOG("Error : Vertex Shader Not Found.");
        return false;
    }

    // ピクセルシェーダを検索.
    if (!SearchFilePath(L"BasicPS.cso", psPath))
    {
        ELOG("Error : Pixel Shader Node Found.");
        return false;
    }

    ComPtr<ID3DBlob> pVSBlob;
    ComPtr<ID3DBlob> pPSBlob;

    // 頂点シェーダを読み込む.
    auto hr = D3DReadFileToBlob(vsPath.c_str(), pVSBlob.GetAddressOf());
    if (FAILED(hr))
    {
        ELOG("Error : D3DReadFiledToBlob() Failed. path = %ls", vsPath.c_str());
        return false;
    }

    // ピクセルシェーダを読み込む.
    hr = D3DReadFileToBlob(psPath.c_str(), pPSBlob.GetAddressOf());
    if (FAILED(hr))
    {
        ELOG("Error : D3DReadFileToBlob() Failed. path = %ls", psPath.c_str());
        return false;
    }

    D3D12_INPUT_ELEMENT_DESC elements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // グラフィックスパイプラインステートを設定.
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
    desc.InputLayout                        = { elements, 4 };
    desc.pRootSignature                     = m_BasicRootSig.GetPtr();
    desc.VS                                 = { pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize() };
    desc.PS                                 = { pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize() };
    desc.RasterizerState                    = DirectX::CommonStates::CullNone;
    desc.BlendState                         = DirectX::CommonStates::Opaque;
    desc.DepthStencilState                  = DirectX::CommonStates::DepthDefault;
    desc.SampleMask                         = UINT_MAX;
    desc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets                   = 1;
    desc.RTVFormats[0]                      = rtvFormat;
    desc.DSVFormat                          = dsvFormat;
    
    desc.SampleDesc.Count                   = 1;
    desc.SampleDesc.Quality                 = 0;

    // パイプラインステートを生成.
    hr = pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pBasicPSO.GetAddressOf()));
    if (FAILED(hr))
    {
        ELOG("Error : ID3D12Device::CreateGraphicsPipelineState() Failed. retcode = 0x%x", hr);
        return false;
    }
}

bool BasicShader::Init(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat)
{
    if (!CreateBasicRootSig(pDevice.Get()))                  return false;
    if (!CreateBasicPipeLineState(pDevice.Get(),
        rtvFormat,
        dsvFormat))                                          return false;

    return true;
}

void BasicShader::SetShader(ID3D12GraphicsCommandList* pCmd, int frameindex, Material& mat,int id, CommonBufferManager& commonbufmanager, IBLBaker& baker)
{
    //　マテリアル共通のバッファ
    {
        pCmd->SetGraphicsRootSignature(m_BasicRootSig.GetPtr());
        pCmd->SetGraphicsRootDescriptorTable(0, commonbufmanager.m_TransformCB[frameindex].GetHandleGPU());
        pCmd->SetGraphicsRootDescriptorTable(1, commonbufmanager.m_MeshCB[frameindex].GetHandleGPU());
        pCmd->SetGraphicsRootDescriptorTable(2, commonbufmanager.m_LightCB[frameindex].GetHandleGPU());
        pCmd->SetGraphicsRootDescriptorTable(3, commonbufmanager.m_CameraCB[frameindex].GetHandleGPU());
        pCmd->SetGraphicsRootDescriptorTable(4, baker.GetHandleGPU_DFG());
        pCmd->SetGraphicsRootDescriptorTable(5, baker.GetHandleGPU_DiffuseLD());
        pCmd->SetGraphicsRootDescriptorTable(6, baker.GetHandleGPU_SpecularLD());
    }

    //　マテリアルごとに異なるバッファ
    {
        pCmd->SetGraphicsRootDescriptorTable(7,     mat.GetTextureHandle(id, TU_BASE_COLOR));
        pCmd->SetGraphicsRootDescriptorTable(8,     mat.GetTextureHandle(id, TU_METALLIC));
        pCmd->SetGraphicsRootDescriptorTable(9,     mat.GetTextureHandle(id, TU_ROUGHNESS));
        pCmd->SetGraphicsRootDescriptorTable(10,    mat.GetTextureHandle(id, TU_NORMAL));
    }

    pCmd->SetPipelineState(m_pBasicPSO.Get());
}

void BasicShader::Term()
{
    m_pBasicPSO.Reset();
    m_BasicRootSig.Term();
}

#include "ToneMap.h"
#include "DirectXHelpers.h"
#include "Logger.h"
#include "App.h"
#include <FileUtil.h>
#include "SampleAppSpace.h"
#include <CommonStates.h>

bool ToneMap::Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)
{
    if (!CreateToneMapRootSig(pDevice))                                             return false;
    if (!CreateToneMapPipeLineState(pDevice, rtv_format, dsv_format))               return false;
    if (!CreateToneMapConstantBuffer(pDevice, pool))                                return false;

    return true;
}

ToneMap::ToneMap()
{
    m_TonemapType   = (SampleAppSpace::TONEMAP_GT);
    m_ColorSpace    = (SampleAppSpace::COLOR_SPACE_BT709);
    m_BaseLuminance = (100.0f);
    m_MaxLuminance  = (100.0f);
}

ToneMap::~ToneMap()
{
}

void ToneMap::Term()
{
    for (auto i = 0; i < App::FrameCount; ++i)
    {
        m_TonemapCB[i].Term();
    }
    m_pTonemapPSO.Reset();
    m_TonemapRootSig.Term();

}

bool ToneMap::CreateToneMapRootSig(ComPtr<ID3D12Device> pDevice) {
    RootSignature::Desc desc;
    desc.Begin(2)
        .SetCBV(ShaderStage::PS, 0, 0)
        .SetSRV(ShaderStage::PS, 1, 0)
        .AddStaticSmp(ShaderStage::PS, 0, SamplerState::LinearWrap)
        .AllowIL()
        .End();

    if (!m_TonemapRootSig.Init(pDevice.Get(), desc.GetDesc()))
    {
        ELOG("Error : RootSignature::Init() Failed.");
        return false;
    }
    return true;
}

bool ToneMap::CreateToneMapPipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format) {
    std::wstring vsPath;
    std::wstring psPath;

    // ���_�V�F�[�_������.
    if (!SearchFilePath(L"QuadVS.cso", vsPath))
    {
        ELOG("Error : Vertex Shader Not Found.");
        return false;
    }

    // �s�N�Z���V�F�[�_������.
    if (!SearchFilePath(L"TonemapPS.cso", psPath))
    {
        ELOG("Error : Pixel Shader Node Found.");
        return false;
    }

    ComPtr<ID3DBlob> pVSBlob;
    ComPtr<ID3DBlob> pPSBlob;

    // ���_�V�F�[�_��ǂݍ���.
    auto hr = D3DReadFileToBlob(vsPath.c_str(), pVSBlob.GetAddressOf());
    if (FAILED(hr))
    {
        ELOG("Error : D3DReadFiledToBlob() Failed. path = %ls", vsPath.c_str());
        return false;
    }

    // �s�N�Z���V�F�[�_��ǂݍ���.
    hr = D3DReadFileToBlob(psPath.c_str(), pPSBlob.GetAddressOf());
    if (FAILED(hr))
    {
        ELOG("Error : D3DReadFileToBlob() Failed. path = %ls", psPath.c_str());
        return false;
    }

    D3D12_INPUT_ELEMENT_DESC elements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // �O���t�B�b�N�X�p�C�v���C���X�e�[�g��ݒ�.
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
    desc.InputLayout                        = { elements, 2 };
    desc.pRootSignature                     = m_TonemapRootSig.GetPtr();
    desc.VS                                 = { pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize() };
    desc.PS                                 = { pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize() };
    desc.RasterizerState                    = DirectX::CommonStates::CullNone;
    desc.BlendState                         = DirectX::CommonStates::Opaque;
    desc.DepthStencilState                  = DirectX::CommonStates::DepthDefault;
    desc.SampleMask                         = UINT_MAX;
    desc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets                   = 1;
    desc.RTVFormats[0]                      = rtv_format;
    desc.DSVFormat                          = dsv_format;
    desc.SampleDesc.Count                   = 1;
    desc.SampleDesc.Quality                 = 0;



    // �p�C�v���C���X�e�[�g�𐶐�.
    hr = pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pTonemapPSO.GetAddressOf()));
    if (FAILED(hr))
    {
        ELOG("Error : ID3D12Device::CreateGraphicsPipelineState() Failed. retcode = 0x%x", hr);
        return false;
    }

    return true;
}

bool ToneMap::CreateToneMapConstantBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool) {
    
    // 
    for (auto i = 0; i < App::FrameCount; ++i)
    {
        if (!m_TonemapCB[i].Init(pDevice.Get(), pool, sizeof(SampleAppSpace::CbTonemap)))
        {
            ELOG("Error : ConstantBuffer::Init() Failed.");
            return false;
        }
    }
}

void ToneMap::DrawTonemap(ID3D12GraphicsCommandList* pCmd, int frameindex, ColorTarget& colortarget, D3D12_VIEWPORT* viewport, D3D12_RECT* scissor, VertexBuffer &vb)
{
    // �萔�o�b�t�@�X�V
    {
        auto ptr           = m_TonemapCB[frameindex].GetPtr<SampleAppSpace::CbTonemap>();
        ptr->Type          = m_TonemapType;
        ptr->ColorSpace    = m_ColorSpace;
        ptr->BaseLuminance = m_BaseLuminance;
        ptr->MaxLuminance  = m_MaxLuminance;
    }

    pCmd->SetGraphicsRootSignature(m_TonemapRootSig.GetPtr());
    pCmd->SetGraphicsRootDescriptorTable(0, m_TonemapCB[frameindex].GetHandleGPU());
    pCmd->SetGraphicsRootDescriptorTable(1, colortarget.GetHandleSRV()->HandleGPU);

    pCmd->SetPipelineState(m_pTonemapPSO.Get());
    pCmd->RSSetViewports(1,     viewport);
    pCmd->RSSetScissorRects(1,  scissor);

    pCmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pCmd->IASetVertexBuffers(0, 1, &vb.GetView());

    pCmd->DrawInstanced(3, 1, 0, 0);
}

void ToneMap::SetLuminance(float base, float max)
{
    m_BaseLuminance = base;
    m_MaxLuminance  = max;
}

void ToneMap::SetToneMapType(SampleAppSpace::TONEMAP_TYPE t)
{
    m_TonemapType = t;
}

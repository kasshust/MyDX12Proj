#include "ToneMap.h"
#include "DirectXHelpers.h"
#include "Logger.h"
#include "App.h"
#include <FileUtil.h>

#include <CommonStates.h>

bool ToneMap::Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)
{
	if (!CreateRootSig(pDevice))                                             return false;
	if (!CreatePipeLineState(pDevice, rtv_format, dsv_format))               return false;
	if (!CreateConstantBuffer(pDevice, pool, m_CB, sizeof(CbTonemap)))		 return false;

	m_TonemapType   = (TONEMAP_GT);
	m_ColorSpace    = (COLOR_SPACE_BT709);
	m_BaseLuminance = (100.0f);
	m_MaxLuminance  = (100.0f);

	return true;
}

bool ToneMap::CreateRootSig(ComPtr<ID3D12Device> pDevice) {
	RootSignature::Desc desc;
	desc.Begin(2)
		.SetCBV(ShaderStage::PS, 0, 0)
		.SetSRV(ShaderStage::PS, 1, 0)
		.AddStaticSmp(ShaderStage::PS, 0, SamplerState::LinearWrap)
		.AllowIL()
		.End();

	if (!InitRootSignature(pDevice, desc, m_RootSig)) return false;

	return true;
}

bool ToneMap::CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format) {
	std::wstring vsPath;
	std::wstring psPath;
	ComPtr<ID3DBlob> pVSBlob;
	ComPtr<ID3DBlob> pPSBlob;

	if (!SearchAndLoadShader(m_VSPath, pVSBlob)) return false;
	if (!SearchAndLoadShader(m_PSPath, pPSBlob)) return false;

	D3D12_INPUT_ELEMENT_DESC elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// グラフィックスパイプラインステートを設定.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.InputLayout                        = { elements, 2 };
	desc.pRootSignature                     = m_RootSig.GetPtr();
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

	// パイプラインステートを生成.
	if (!CreateGraphicsPipelineState(pDevice, desc, m_pPSO))return false;

	return true;
}


void ToneMap::Term()
{
	for (auto i = 0; i < App::FrameCount; ++i)
	{
		m_CB[i].Term();
	}
	m_pPSO.Reset();
	m_RootSig.Term();
}


void ToneMap::DrawTonemap(ID3D12GraphicsCommandList* pCmd, int frameindex, DrawSource& s)
{
	// 書き込み用リソースバリア設定.

	DirectX::TransitionResource(pCmd,
		s.ColorDest.GetResource(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	// ディスクリプタ取得.
	auto handleRTV = s.ColorDest.GetHandleRTV();
	auto handleDSV = s.DepthDest.GetHandleDSV();

	// レンダーターゲットを設定.
	pCmd->OMSetRenderTargets(1, &handleRTV->HandleCPU, FALSE, &handleDSV->HandleCPU);

	// レンダーターゲットをクリア.
	s.ColorDest.ClearView(pCmd);
	s.DepthDest.ClearView(pCmd);

	// 定数バッファ更新
	{
		auto ptr           = m_CB[frameindex].GetPtr<CbTonemap>();
		ptr->Type          = m_TonemapType;
		ptr->ColorSpace    = m_ColorSpace;
		ptr->BaseLuminance = m_BaseLuminance;
		ptr->MaxLuminance  = m_MaxLuminance;
	}

	pCmd->SetGraphicsRootSignature(m_RootSig.GetPtr());
	pCmd->SetGraphicsRootDescriptorTable(0, m_CB[frameindex].GetHandleGPU());
	pCmd->SetGraphicsRootDescriptorTable(1, s.ColorSource.GetHandleSRV()->HandleGPU);
	pCmd->SetPipelineState(m_pPSO.Get());

	pCmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCmd->IASetVertexBuffers(0, 1, &s.VertexBuffer.GetView());

	pCmd->DrawInstanced(3, 1, 0, 0);

	// 表示用リソースバリア設定.
	DirectX::TransitionResource(pCmd,
		s.ColorDest.GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
}

void ToneMap::SetLuminance(float base, float max)
{
	m_BaseLuminance = base;
	m_MaxLuminance = max;
}


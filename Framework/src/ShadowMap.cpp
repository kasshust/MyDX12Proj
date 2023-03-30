#include "DirectXHelpers.h"
#include <ShadowMap.h>
#include "Logger.h"
#include <FileUtil.h>
#include <CommonStates.h>
#include <Renderer.h>

bool ShadowMap::Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)
{
	if (!CreateRootSig(pDevice))                                             return false;
	if (!CreatePipeLineState(pDevice, rtv_format, dsv_format))               return false;
	if (!CreateConstantBuffer(pDevice, pool, m_CB, sizeof(CbDepthShadowMap)))return false;

	return true;
}

bool ShadowMap::CreateRootSig(ComPtr<ID3D12Device> pDevice) {
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

bool ShadowMap::CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format) {
	
	std::wstring vsPath;
	ComPtr<ID3DBlob> pVSBlob;

	if (!SearchAndLoadShader(m_VSPath, pVSBlob)) return false;

	D3D12_INPUT_ELEMENT_DESC elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// グラフィックスパイプラインステートを設定.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.InputLayout                        = { elements, 2 };
	desc.pRootSignature                     = m_RootSig.GetPtr();
	desc.VS                                 = { pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize() };
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


void ShadowMap::Term()
{
	for (auto i = 0; i < App::FrameCount; ++i)
	{
		m_CB[i].Term();
	}
	m_pPSO.Reset();
	m_RootSig.Term();
}


void ShadowMap::DrawShadowMap(
	ID3D12GraphicsCommandList*	pCmd, 
	DepthTarget&				depthDest, 
	D3D12_VIEWPORT*				viewport, 
	D3D12_RECT*					scissor, 
	VertexBuffer&				vb
)
{

	// ディスクリプタ取得.
	auto handleDSV = depthDest.GetHandleDSV();

	// レンダーターゲットを設定.
	pCmd->OMSetRenderTargets(0, 0, FALSE, &handleDSV->HandleCPU);

	// レンダーターゲットをクリア.
	depthDest.ClearView(pCmd);

	// ビューポート設定.
	pCmd->RSSetViewports(1, viewport);
	pCmd->RSSetScissorRects(1, scissor);


	//　ここにSetShadowMapShaderのセッティング

	// シーンの描画.
	/*
	for (size_t i = 0; i < m_GameObjects.size(); i++) {
		GameObject* g = m_GameObjects[i];
		g->m_Model.UpdateWorldMatrix(m_FrameIndex, g->Transform().GetTransform());
		g->m_Model.DrawModel(pCmd, m_FrameIndex, m_CommonBufferManager, m_SkyManager);
		// g->m_Model.DrawModelRaw(pCmd, m_FrameIndex);
	}
	*/
}
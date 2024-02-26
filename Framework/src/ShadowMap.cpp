#include "DirectXHelpers.h"
#include <ShadowMap.h>
#include "Logger.h"
#include <FileUtil.h>
#include <CommonStates.h>
#include <Renderer.h>
#include <CommonBufferManager.h>
#include <GameObject.h>

bool ShadowMap::Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)
{
	if (!CreateRootSig(pDevice))                                             return false;
	if (!CreatePipeLineState(pDevice, rtv_format, dsv_format))               return false;
	if (!CreateConstantBuffer(pDevice, pool, m_CB, sizeof(CbDepthShadowMap)))return false;

	return true;
}

bool ShadowMap::CreateRootSig(ComPtr<ID3D12Device> pDevice) {
	RootSignature::Desc desc;
	
	desc.Begin(3)
		.SetCBV(ShaderStage::VS, 0, 0)	//CbTransform
		.SetCBV(ShaderStage::VS, 1, 1)	//CbMesh
		.SetCBV(ShaderStage::VS, 2, 2)  //CbShadow
		.AllowIL()
		.End();

	if (!InitRootSignature(pDevice, desc, m_RootSig)) return false;
	
	return true;
}

bool ShadowMap::CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format) {
	
	std::wstring		vsPath;
	ComPtr<ID3DBlob>	pVSBlob;

	if (!SearchAndLoadShader(m_VSPath, pVSBlob)) return false;

	D3D12_INPUT_ELEMENT_DESC elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	// グラフィックスパイプラインステートを設定.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.InputLayout                        = { elements, _countof(elements) };
	desc.pRootSignature                     = m_RootSig.GetPtr();
	desc.VS                                 = { pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize() };
	desc.RasterizerState                    = DirectX::CommonStates::CullNone;						
	desc.BlendState                         = DirectX::CommonStates::Opaque;
	desc.DepthStencilState                  = DirectX::CommonStates::DepthDefault;
	desc.SampleMask                         = UINT_MAX;
	desc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.NumRenderTargets                   = 1;
	desc.DSVFormat                          = dsv_format;
	desc.SampleDesc.Count                   = 1;
	desc.SampleDesc.Quality                 = 0;

	//ステンシルを無効に
	desc.DepthStencilState.StencilEnable	= FALSE;


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
	int 						frameindex,
	DrawSource&					s
)
{
	
	DirectX::TransitionResource(pCmd,
		s.DepthDest.GetResource(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	// ディスクリプタ取得.
	auto handleDSV = s.DepthDest.GetHandleDSV();

	// レンダーターゲットを設定.
	pCmd->OMSetRenderTargets(0, nullptr, FALSE, &handleDSV->HandleCPU);

	// レンダーターゲットをクリア.
	s.DepthDest.ClearView(pCmd);

	pCmd->SetGraphicsRootSignature(m_RootSig.GetPtr());
	pCmd->SetPipelineState(m_pPSO.Get());

	pCmd->SetGraphicsRootDescriptorTable(0, s.Commonbufmanager.m_TransformCB[frameindex].GetHandleGPU());
	pCmd->SetGraphicsRootDescriptorTable(2, s.Commonbufmanager.m_LightCB[frameindex].GetHandleGPU());
	// シーンの描画.
	
	for (size_t i = 0; i < s.GameObjects.size(); i++) {
		GameObject* g = s.GameObjects[i];
		pCmd->SetGraphicsRootDescriptorTable(1, g->m_Model.m_MeshCB[frameindex].GetHandleGPU());
		g->m_Model.DrawModelRaw(pCmd, frameindex);
	}

	DirectX::TransitionResource(pCmd,
		s.DepthDest.GetResource(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);

}
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

/*
void ShadowMap::UpdateConstantBuffer(int frameindex, Vector3 lighrDir) {
	auto ptr = m_CB[frameindex].GetPtr<CbDepthShadowMap>();

	if (lighrDir.Length() == 0) {
		return;
	}
		
	// World→Lightに規定変換し、プロジェクションでぺちゃんこに
	// XMFLOAT3 lightPos = lighrDir*10.0f;
	XMFLOAT3 lightPos = { 2.0f, 6.5f, -1.0f };
	XMFLOAT3 lightDest = { 0.0f, 0.0f, 0.0f };
	XMMATRIX view = XMMatrixLookAtLH(
		{ lightPos.x, lightPos.y, lightPos.z },
		{ lightDest.x, lightDest.y, lightDest.z },
		{ 0.0f, 1.0f, 0.0f }
	);

	// XMMATRIX projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 1.0f, 1.0f, 15.0f);
	XMMATRIX projection = XMMatrixOrthographicLH(1.0f,1.0f,0.1,100.0f);

	XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, XMMatrixTranspose(view * projection));

	ptr->LightVP = mat;
}
*/

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
	desc.RasterizerState                    = DirectX::CommonStates::CullNone;						//表のみ
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
	DepthTarget&				depthDest, 
	D3D12_VIEWPORT*				viewport, 
	D3D12_RECT*					scissor, 
	int 						frameindex,
	const CommonBufferManager&	commonbufmanager,
	const std::vector<GameObject*> gameObjects,
	const Vector3				lightDirection
)
{
	
	DirectX::TransitionResource(pCmd,
		depthDest.GetResource(),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_STATE_DEPTH_WRITE
	);

	// ディスクリプタ取得.
	auto handleDSV = depthDest.GetHandleDSV();

	// レンダーターゲットを設定.
	pCmd->OMSetRenderTargets(0, nullptr, FALSE, &handleDSV->HandleCPU);

	// レンダーターゲットをクリア.
	depthDest.ClearView(pCmd);

	pCmd->SetGraphicsRootSignature(m_RootSig.GetPtr());
	pCmd->SetPipelineState(m_pPSO.Get());

	// ビューポート設定.
	pCmd->RSSetViewports(1, viewport);
	pCmd->RSSetScissorRects(1, scissor);

	pCmd->SetGraphicsRootDescriptorTable(0, commonbufmanager.m_TransformCB[frameindex].GetHandleGPU());
	pCmd->SetGraphicsRootDescriptorTable(2, commonbufmanager.m_LightCB[frameindex].GetHandleGPU());
	// シーンの描画.
	
	for (size_t i = 0; i < gameObjects.size(); i++) {
		GameObject* g = gameObjects[i];

		g->m_Model.UpdateWorldMatrix(frameindex, g->Transform().GetTransform());
		pCmd->SetGraphicsRootDescriptorTable(1, g->m_Model.m_MeshCB[frameindex].GetHandleGPU());

		g->m_Model.DrawModelRaw(pCmd, frameindex);
	}

	DirectX::TransitionResource(pCmd,
		depthDest.GetResource(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);

}
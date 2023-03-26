#include "ModelLoader.h"
#include <FileUtil.h>
#include <ResMesh.h>
#include <Logger.h>
#include <CommonBufferManager.h>
#include <App.h>
#include <ResourceManager.h>


bool Model::LoadModel(const wchar_t* filePath, ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue)
{
	m_FilePath = filePath;
	AppResourceManager& manager = AppResourceManager::GetInstance();

	

	// 定数バッファ作成
	if (!CreateMeshBuffer(pDevice, resPool))			return false;

	// MeshとMaterialの作成
	manager.LoadResModel(m_FilePath);
	if (!manager.CreateMesh(pDevice, m_FilePath,		manager.GetResMesh(m_FilePath)))				return false;
	if (!manager.CreateMaterial(pDevice, m_FilePath,	manager.GetResMaterial(m_FilePath), resPool))	return false;

	// リソースバッチを用意.
	DirectX::ResourceUploadBatch batch(pDevice.Get());
	batch.Begin(); // バッチ開始.
	
	const wchar_t* bc = L"../res/texture/rubber_bc.dds";
	const wchar_t* m  = L"../res/texture/rubber_m.dds";
	const wchar_t* r  = L"../res/texture/rubber_r.dds";
	const wchar_t* n  = L"../res/texture/rubber_n.dds";

	std::vector<Material*>& mat = manager.GetMaterial(m_FilePath);
	{
		mat[0]->SetTexture(0, Material::TEXTURE_USAGE_04, bc, manager.LoadGetTexture(bc, pDevice, resPool, true, batch));
		mat[0]->SetTexture(0, Material::TEXTURE_USAGE_05, m,  manager.LoadGetTexture(m, pDevice, resPool,  false, batch));
		mat[0]->SetTexture(0, Material::TEXTURE_USAGE_06, r,  manager.LoadGetTexture(r, pDevice, resPool,  false, batch));
		mat[0]->SetTexture(0, Material::TEXTURE_USAGE_03, n,  manager.LoadGetTexture(n, pDevice, resPool,  false, batch));
	}
	auto future = batch.End(commandQueue.Get()); // バッチ終了.
	future.wait();// バッチ完了を待機.

	return true;
}

void Model::UpdateWorldMatrix(int frameindex)
{
	auto ptr = m_MeshCB[frameindex].GetPtr<CommonCb::CbMesh>();
	ptr->World = Matrix::Identity;
}

bool Model::CreateMeshBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool) {
	for (auto i = 0; i < App::FrameCount; ++i)
	{
		if (!m_MeshCB[i].Init(pDevice.Get(), resPool, sizeof(CommonCb::CbMesh)))
		{
			ELOG("Error : ConstantBuffer::Init() Failed.");
			return false;
		}

		auto ptr = m_MeshCB[i].GetPtr<CommonCb::CbMesh>();
		ptr->World = Matrix::Identity;
	}
	return true;
}

void Model::DrawModel(ID3D12GraphicsCommandList* pCmd, int frameIndex, const CommonBufferManager& commonBufferManager,  const IBLBaker& baker, Shader& shader)
{
	// matrix更新
	UpdateWorldMatrix(frameIndex);

	AppResourceManager& manager = AppResourceManager::GetInstance();

	const std::vector<Mesh*>		meshs = manager.GetMesh(m_FilePath);
	const std::vector<Material*>	mat = manager.GetMaterial(m_FilePath);

	for (size_t i = 0; i < meshs.size(); ++i)
	{
		// マテリアルIDを取得.
		auto id = meshs[i]->GetMaterialId();

		// 使用するShaderをセット
		shader.SetShader(pCmd, frameIndex, *mat[0], i, commonBufferManager, baker);
		pCmd->SetGraphicsRootDescriptorTable(1, m_MeshCB[frameIndex].GetHandleGPU());

		// メッシュを描画.
		meshs[i]->Draw(pCmd);
	}
}

void Model::Release()
{

	for (auto i = 0; i < App::FrameCount; ++i)
	{
		m_MeshCB[i].Term();
	}
}
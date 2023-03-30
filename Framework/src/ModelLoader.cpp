#include "ModelLoader.h"

#include <FileUtil.h>
#include <ResMesh.h>
#include <Logger.h>
#include <CommonBufferManager.h>
#include <App.h>
#include <ResourceManager.h>

bool Model::LoadModel(std::wstring filePath, ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue)
{
	m_ModelPath = filePath;
	AppResourceManager& manager = AppResourceManager::GetInstance();

	if (!CreateMeshBuffer(pDevice, resPool)) return false;

	if (!manager.LoadResModel(m_ModelPath))															return false;
	if (!manager.CreateMesh(pDevice,	 m_ModelPath, manager.GetResMesh(m_ModelPath)))				return false;
	if (!manager.CreateMaterial(pDevice, m_ModelPath, manager.GetResMaterial(m_ModelPath), resPool))	return false;

	std::vector<Material*>&		mat = manager.GetMaterial(m_ModelPath);
	std::vector<ResMaterial>	res = manager.GetResMaterial(m_ModelPath);

	DirectX::ResourceUploadBatch batch(pDevice.Get());
	batch.Begin(); // バッチ開始.

	for (size_t i = 0; i < mat.size(); i++)
	{
		// マテリアルの定数バッファ値を読み取る
		// auto ptr = reinterpret_cast<CommonCb::CbMaterial*>(mat[i]->GetBufferPtr(0));
		// ptr->Param00 = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

		std::wstring shaderKey = res[i].ShaderKey;

		ModelShader* p = manager.GetShader(shaderKey.c_str());
		if (p == nullptr) {
			ELOG("Error : Shader Loading Failed = %ls ", shaderKey.c_str());
			// return false;
		}
		mat[i]->SetShaderPtr(p);

		SetTexture(mat[i], Material::TEXTURE_USAGE_03, res[i].NormalMap,		pDevice, resPool, true, batch, manager);
		SetTexture(mat[i], Material::TEXTURE_USAGE_04, res[i].DiffuseMap,		pDevice, resPool, false, batch, manager);
		SetTexture(mat[i], Material::TEXTURE_USAGE_05, res[i].SpecularMap,		pDevice, resPool, false, batch, manager);
		SetTexture(mat[i], Material::TEXTURE_USAGE_06, res[i].ShininessMap,		pDevice, resPool, false, batch, manager);
		SetTexture(mat[i], Material::TEXTURE_USAGE_07, res[i].AmbientMap,		pDevice, resPool, false, batch, manager);
		SetTexture(mat[i], Material::TEXTURE_USAGE_08, res[i].OpacityMap,		pDevice, resPool, false, batch, manager);
		SetTexture(mat[i], Material::TEXTURE_USAGE_09, res[i].EmissiveMap,		pDevice, resPool, false, batch, manager);
		SetTexture(mat[i], Material::TEXTURE_USAGE_10, res[i].DisplacementMap,	pDevice, resPool, false, batch, manager);
	}
	
	auto future = batch.End(commandQueue.Get()); // バッチ終了.
	future.wait();// バッチ完了を待機.

	return true;
}

std::vector<Material*> Model::GetMaterials() {
	return  AppResourceManager::GetInstance().GetMaterial(m_ModelPath);
}

void Model::SetTexture(
	Material* mat, 
	Material::TEXTURE_USAGE usage, 
	const std::wstring path,
	ComPtr<ID3D12Device> pDevice, 
	DescriptorPool* resPool, 
	bool isSRGB, 
	DirectX::ResourceUploadBatch& batch,
	AppResourceManager& manager
) {
	if (wcslen(path.c_str()) != 0) mat->SetTexture(0, usage, path, manager.LoadGetTexture(path, pDevice, resPool, isSRGB, batch));
}

void Model::DrawModelRaw(ID3D12GraphicsCommandList* pCmd, int frameIndex) {
	AppResourceManager&			manager     = AppResourceManager::GetInstance();
	const std::vector<Mesh*>	meshs		= manager.GetMesh(m_ModelPath);

	for (size_t i = 0; i < meshs.size(); ++i)
	{
		// メッシュを描画.
		meshs[i]->Draw(pCmd);
	}
}

void Model::DrawModel(ID3D12GraphicsCommandList* pCmd, int frameIndex, CommonBufferManager& commonBufferManager, const SkyManager& skyManager)
{
	AppResourceManager&				manager	= AppResourceManager::GetInstance();
	const std::vector<Mesh*>		meshs	= manager.GetMesh(m_ModelPath);
	const std::vector<Material*>	mat		= manager.GetMaterial(m_ModelPath);

	for (size_t i = 0; i < meshs.size(); ++i)
	{
		// マテリアルを設定
		auto id = meshs[i]->GetMaterialId();
		mat[i]->SetMaterial(pCmd, frameIndex, *mat[id], id, m_MeshCB, commonBufferManager, skyManager);

		// メッシュを描画.
		meshs[i]->Draw(pCmd);
	}
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

void Model::UpdateWorldMatrix(int frameindex, Matrix& modelMatrix)
{
	auto ptr = m_MeshCB[frameindex].GetPtr<CommonCb::CbMesh>();
	ptr->World = modelMatrix;
}

void Model::Release()
{
	for (auto i = 0; i < App::FrameCount; ++i)
	{
		m_MeshCB[i].Term();
	}
}
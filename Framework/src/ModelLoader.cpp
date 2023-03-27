#include "ModelLoader.h"

#include <FileUtil.h>
#include <ResMesh.h>
#include <Logger.h>
#include <CommonBufferManager.h>
#include <App.h>
#include <ResourceManager.h>

bool Model::LoadModel(const wchar_t* filePath, ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue)
{
	wcscpy_s(m_FilePath, OFS_MAXPATHNAME, filePath);
	AppResourceManager& manager = AppResourceManager::GetInstance();

	if (!CreateMeshBuffer(pDevice, resPool)) return false;

	if (!manager.LoadResModel(m_FilePath))															return false;
	if (!manager.CreateMesh(pDevice,	 m_FilePath, manager.GetResMesh(m_FilePath)))				return false;
	if (!manager.CreateMaterial(pDevice, m_FilePath, manager.GetResMaterial(m_FilePath), resPool))	return false;

	std::vector<Material*>&		mat = manager.GetMaterial(m_FilePath);
	std::vector<ResMaterial>	res = manager.GetResMaterial(m_FilePath);

	DirectX::ResourceUploadBatch batch(pDevice.Get());
	batch.Begin(); // バッチ開始.

	for (size_t i = 0; i < mat.size(); i++)
	{
		// マテリアルの定数バッファ値を読み取る
		// auto ptr = reinterpret_cast<CommonCb::CbMaterial*>(mat[i]->GetBufferPtr(0));
		// ptr->Param00 = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		// ptr->Param01 = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		// ptr->Param02 = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		// ptr->Param03 = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		// ptr->Param04 = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		// ptr->Param05 = Vector4(0.0f, 0.0f, 0.0f, 0.0f);

		// テクスチャの読み込み
		const wchar_t* normal		= res[i].NormalMap.c_str();
		const wchar_t* albedo		= res[i].DiffuseMap.c_str();
		const wchar_t* specular		= res[i].SpecularMap.c_str();
		const wchar_t* shininess	= res[i].ShininessMap.c_str();
		const wchar_t* ambient		= res[i].AmbientMap.c_str();
		const wchar_t* opacity		= res[i].OpacityMap.c_str();
		const wchar_t* emissive		= res[i].EmissiveMap.c_str();
		const wchar_t* displace		= res[i].DisplacementMap.c_str();

		SetTexture(mat[i], Material::TEXTURE_USAGE_03, normal,		pDevice, resPool, true, batch, manager);
		SetTexture(mat[i], Material::TEXTURE_USAGE_04, albedo,		pDevice, resPool, false, batch, manager);
		SetTexture(mat[i], Material::TEXTURE_USAGE_05, specular,	pDevice, resPool, false, batch, manager);
		SetTexture(mat[i], Material::TEXTURE_USAGE_06, shininess,	pDevice, resPool, false, batch, manager);
		SetTexture(mat[i], Material::TEXTURE_USAGE_07, ambient,		pDevice, resPool, false, batch, manager);
		SetTexture(mat[i], Material::TEXTURE_USAGE_08, opacity,		pDevice, resPool, false, batch, manager);
		SetTexture(mat[i], Material::TEXTURE_USAGE_09, emissive,	pDevice, resPool, false, batch, manager);
		SetTexture(mat[i], Material::TEXTURE_USAGE_10, displace,	pDevice, resPool, false, batch, manager);
	}
	
	auto future = batch.End(commandQueue.Get()); // バッチ終了.
	future.wait();// バッチ完了を待機.

	return true;
}

std::vector<Material*> Model::GetMaterials() {
	return  AppResourceManager::GetInstance().GetMaterial(m_FilePath);
}

void Model::SetTexture(
	Material* mat, 
	Material::TEXTURE_USAGE usage, 
	const wchar_t* path, 
	ComPtr<ID3D12Device> pDevice, 
	DescriptorPool* resPool, 
	bool isSRGB, 
	DirectX::ResourceUploadBatch& batch,
	AppResourceManager& manager
) {
	if (wcslen(path) != 0) mat->SetTexture(0, usage, path, manager.LoadGetTexture(path, pDevice, resPool, isSRGB, batch));
}

void Model::DrawModel(ID3D12GraphicsCommandList* pCmd, int frameIndex, CommonBufferManager& commonBufferManager, const IBLBaker& baker, Shader& shader)
{
	AppResourceManager& manager = AppResourceManager::GetInstance();
	const std::vector<Mesh*>		meshs = manager.GetMesh(m_FilePath);
	const std::vector<Material*>	mat = manager.GetMaterial(m_FilePath);

	for (size_t i = 0; i < meshs.size(); ++i)
	{
		// マテリアルIDを取得.
		auto id = meshs[i]->GetMaterialId();

		// mat[id]->SetProperty();
		shader.SetShader(pCmd, frameIndex, *mat[id], i, this, commonBufferManager, baker);

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
#include "ModelLoader.h"
#include <FileUtil.h>
#include <ResMesh.h>
#include <Logger.h>
#include <CommonBufferManager.h>
#include <App.h>
#include <ResourceManager.h>

bool ModelLoader::LoadModel(const wchar_t* filePath, ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue)
{
	std::wstring dir = GetDirectoryPath(filePath);

	AppResourceManager::GetInstance().LoadResModel(filePath);

	// �萔�o�b�t�@�쐬
	if (!CreateMeshBuffer(pDevice, resPool))			return false;

	// Mesh�쐬
	if (!CreateMesh(pDevice,	 *AppResourceManager::GetInstance().GetMesh(filePath)))					return false;
	if (!CreateMaterial(pDevice, *AppResourceManager::GetInstance().GetMaterial(filePath), resPool)) return false;

	// ���\�[�X�o�b�`��p��.
	DirectX::ResourceUploadBatch batch(pDevice.Get());
	batch.Begin(); // �o�b�`�J�n.
	
	const wchar_t* bc = L"../res/texture/gold_bc.dds";
	const wchar_t* m  = L"../res/texture/gold_m.dds";
	const wchar_t* r  = L"../res/texture/gold_r.dds";
	const wchar_t* n  = L"../res/texture/gold_n.dds";

	// AppResourceManager::GetInstance().LoadTexture(bc, pDevice, m_Material.GetPool(), true, batch);
	// AppResourceManager::GetInstance().LoadTexture(m , pDevice, resPool, false, batch);
	// AppResourceManager::GetInstance().LoadTexture(r , pDevice, resPool, false, batch);
	// AppResourceManager::GetInstance().LoadTexture(n , pDevice, resPool, false, batch);
	{
		// m_Material.SetTexture(0, Material::TEXTURE_USAGE_04, bc, AppResourceManager::GetInstance().GetTexture(bc));
		m_Material.SetTexture(0, Material::TEXTURE_USAGE_04, bc, batch, false);
		m_Material.SetTexture(0, Material::TEXTURE_USAGE_05, m, batch, false);
		m_Material.SetTexture(0, Material::TEXTURE_USAGE_06, r, batch, false);
		m_Material.SetTexture(0, Material::TEXTURE_USAGE_03, n, batch, false);
	}
	auto future = batch.End(commandQueue.Get()); // �o�b�`�I��.
	future.wait();// �o�b�`������ҋ@.

	return true;
}

void ModelLoader::UpdateWorldMatrix(int frameindex)
{
	auto ptr = m_MeshCB[frameindex].GetPtr<CommonCb::CbMesh>();
	ptr->World = Matrix::Identity;
}

bool ModelLoader::CreateMeshBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool) {
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

bool ModelLoader::CreateMesh(ComPtr<ID3D12Device> pDevice, std::vector<ResMesh> resMesh) {
	// ��������\��.
	m_pMesh.reserve(resMesh.size());

	// ���b�V����������.
	for (size_t i = 0; i < resMesh.size(); ++i)
	{
		// ���b�V������.
		auto mesh = new (std::nothrow) Mesh();

		// �`�F�b�N.
		if (mesh == nullptr)
		{
			ELOG("Error : Out of memory.");
			return false;
		}

		// ����������.
		if (!mesh->Init(pDevice.Get(), resMesh[i]))
		{
			ELOG("Error : Mesh Initialize Failed.");
			delete mesh;
			return false;
		}

		// ����������o�^.
		m_pMesh.push_back(mesh);
	}

	// �������œK��.
	m_pMesh.shrink_to_fit();

	return true;
}

bool ModelLoader::CreateMaterial(ComPtr<ID3D12Device> pDevice, std::vector<ResMaterial> resMaterial, DescriptorPool* resPool) {
	if (!m_Material.Init(
		pDevice.Get(),
		resPool,
		sizeof(CommonCb::CbMaterial),
		resMaterial.size()))
	{
		ELOG("Error : Material::Init() Failed.");
		return false;
	}

	return true;
}

void ModelLoader::Release()
{
	for (size_t i = 0; i < m_pMesh.size(); ++i)
	{
		SafeTerm(m_pMesh[i]);
	}
	m_pMesh.clear();
	m_pMesh.shrink_to_fit();
	m_Material.Term();

	for (auto i = 0; i < App::FrameCount; ++i)
	{
		m_MeshCB[i].Term();
	}
}
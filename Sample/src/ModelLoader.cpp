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

	

	// �萔�o�b�t�@�쐬
	if (!CreateMeshBuffer(pDevice, resPool))			return false;

	// Mesh��Material�̍쐬
	manager.LoadResModel(m_FilePath);
	if (!manager.CreateMesh(pDevice, m_FilePath,		manager.GetResMesh(m_FilePath)))				return false;
	if (!manager.CreateMaterial(pDevice, m_FilePath,	manager.GetResMaterial(m_FilePath), resPool))	return false;

	// ���\�[�X�o�b�`��p��.
	DirectX::ResourceUploadBatch batch(pDevice.Get());
	batch.Begin(); // �o�b�`�J�n.
	
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
	auto future = batch.End(commandQueue.Get()); // �o�b�`�I��.
	future.wait();// �o�b�`������ҋ@.

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
	// matrix�X�V
	UpdateWorldMatrix(frameIndex);

	AppResourceManager& manager = AppResourceManager::GetInstance();

	const std::vector<Mesh*>		meshs = manager.GetMesh(m_FilePath);
	const std::vector<Material*>	mat = manager.GetMaterial(m_FilePath);

	for (size_t i = 0; i < meshs.size(); ++i)
	{
		// �}�e���A��ID���擾.
		auto id = meshs[i]->GetMaterialId();

		// �g�p����Shader���Z�b�g
		shader.SetShader(pCmd, frameIndex, *mat[0], i, commonBufferManager, baker);
		pCmd->SetGraphicsRootDescriptorTable(1, m_MeshCB[frameIndex].GetHandleGPU());

		// ���b�V����`��.
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
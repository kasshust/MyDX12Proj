#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <Texture.h>
#include <Mesh.h>
#include <FileUtil.h>
#include <Logger.h>
#include <ResMesh.h>
#include <Material.h>
#include <CommonBufferManager.h>

// ResourceManager�N���X
class AppResourceManager {
public:

	static AppResourceManager& GetInstance()
	{
		static AppResourceManager instance;
		return instance;
	}

	void Init();

	void Release();

	bool CheckFilePath(const wchar_t* path);

	// Texture��ǂݍ����unordered_map�ɓo�^����
	bool LoadTexture(const wchar_t* path,
		ComPtr<ID3D12Device> pDevice,
		DescriptorPool* pPool,
		bool isSRGB,
		DirectX::ResourceUploadBatch& batch);

	// Model��ǂݍ����unordered_map�ɓo�^����
	bool LoadResModel(const wchar_t* path);

	// ResMesh����Mesh���쐬����
	bool CreateMesh(ComPtr<ID3D12Device> pDevice, const wchar_t* key, std::vector<ResMesh> resMesh);

	bool CreateMaterial(ComPtr<ID3D12Device> pDevice, const wchar_t* key, std::vector<ResMaterial> resMaterial, DescriptorPool* resPool);

	// �e�N�X�`�����擾����
	Texture* GetTexture(const wchar_t* path);

	// �e�N�X�`���ɑ΂��郍�[�h�ƃQ�b�g�𓯎��ɍs��
	Texture* LoadGetTexture(const wchar_t* path,
		ComPtr<ID3D12Device> pDevice,
		DescriptorPool* pPool,
		bool isSRGB,
		DirectX::ResourceUploadBatch& batch);

	// ���\�[�X���b�V�����擾����
	std::vector<ResMesh> GetResMesh(const wchar_t* path);

	// ���\�[�X�}�e���A�����擾����
	std::vector<ResMaterial> GetResMaterial(const wchar_t* path);

	// ���b�V�����擾����

	std::vector<Mesh*> GetMesh(const wchar_t* path);

	// �}�e���A�����擾����
	std::vector<Material*> GetMaterial(const wchar_t* path);

	const std::unordered_map<const wchar_t*, Texture*> GetTexturesMap();

	const std::unordered_map<const wchar_t*, std::vector<ResMesh>>     GetResMeshesMap();

	const std::unordered_map<const wchar_t*, std::vector<ResMaterial>> GetResMaterialsMap();

	void AddShader(
		std::wstring path,
		Shader* shader
	);

	Shader* GetShader(std::wstring key);
		

private:
	std::unordered_map<std::wstring, Shader*>                                    m_pShaders{};
	std::unordered_map<const wchar_t*, Texture*>                                 m_Textures{};

	std::unordered_map<const wchar_t*, std::vector<Mesh*>>                       m_pMeshs{};
	std::unordered_map<const wchar_t*, std::vector<Material*>>                   m_pMaterials{};

	// �O�̂��߂Ƀ��\�[�X���ێ����Ă���
	std::unordered_map<const wchar_t*, std::vector<ResMesh>>                    m_ResMeshes{};
	std::unordered_map<const wchar_t*, std::vector<ResMaterial>>                m_ResMaterials{};

	AppResourceManager() {}
	~AppResourceManager() {}
};
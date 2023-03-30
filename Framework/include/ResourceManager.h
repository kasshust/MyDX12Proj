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

// ResourceManagerƒNƒ‰ƒX
class AppResourceManager {
public:

	static AppResourceManager& GetInstance()
	{
		static AppResourceManager instance;
		return instance;
	}

	void Init();

	void Release();

	bool CheckFilePath(const std::wstring& path);
	bool LoadTexture(const std::wstring path,
		ComPtr<ID3D12Device> pDevice,
		DescriptorPool* pPool,
		bool isSRGB,
		DirectX::ResourceUploadBatch& batch);
	bool LoadResModel(const std::wstring path);
	bool CreateMesh(ComPtr<ID3D12Device> pDevice, const std::wstring key, std::vector<ResMesh> resMesh);
	bool CreateMaterial(ComPtr<ID3D12Device> pDevice, const std::wstring key, std::vector<ResMaterial> resMaterial, DescriptorPool* resPool);

	void AddShader(	const std::wstring path,Shader* shader	);




	Texture*					GetTexture(const std::wstring& path);
	std::vector<ResMesh>		GetResMesh(const std::wstring& path);
	std::vector<ResMaterial>	GetResMaterial(const std::wstring& path);
	std::vector<Mesh*>			GetMesh(const std::wstring& path);
	std::vector<Material*>		GetMaterial(const std::wstring& path);
	Shader*						GetShader(const std::wstring& key);


	const std::unordered_map<std::wstring, Texture*> GetTexturesMap();

	const std::unordered_map<std::wstring, std::vector<ResMesh>>     GetResMeshesMap();

	const std::unordered_map<std::wstring, std::vector<ResMaterial>> GetResMaterialsMap();

	Texture* LoadGetTexture(const std::wstring path,
	ComPtr<ID3D12Device> pDevice,
	DescriptorPool* pPool,
	bool isSRGB,
	DirectX::ResourceUploadBatch& batch);




private:
	std::unordered_map<std::wstring, Shader*>									 m_pShaders{};
	std::unordered_map<std::wstring, Texture*>                                 m_Textures{};

	std::unordered_map<std::wstring, std::vector<Mesh*>>                       m_pMeshs{};
	std::unordered_map<std::wstring, std::vector<Material*>>                   m_pMaterials{};

	std::unordered_map<std::wstring, std::vector<ResMesh>>                    m_ResMeshes{};
	std::unordered_map<std::wstring, std::vector<ResMaterial>>                m_ResMaterials{};
};
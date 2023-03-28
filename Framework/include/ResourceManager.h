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

// ResourceManagerクラス
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

	// Textureを読み込んでunordered_mapに登録する
	bool LoadTexture(const wchar_t* path,
		ComPtr<ID3D12Device> pDevice,
		DescriptorPool* pPool,
		bool isSRGB,
		DirectX::ResourceUploadBatch& batch);

	// Modelを読み込んでunordered_mapに登録する
	bool LoadResModel(const wchar_t* path);

	// ResMeshからMeshを作成する
	bool CreateMesh(ComPtr<ID3D12Device> pDevice, const wchar_t* key, std::vector<ResMesh> resMesh);

	bool CreateMaterial(ComPtr<ID3D12Device> pDevice, const wchar_t* key, std::vector<ResMaterial> resMaterial, DescriptorPool* resPool);

	// テクスチャを取得する
	Texture* GetTexture(const wchar_t* path);

	// テクスチャに対するロードとゲットを同時に行う
	Texture* LoadGetTexture(const wchar_t* path,
		ComPtr<ID3D12Device> pDevice,
		DescriptorPool* pPool,
		bool isSRGB,
		DirectX::ResourceUploadBatch& batch);

	// リソースメッシュを取得する
	std::vector<ResMesh> GetResMesh(const wchar_t* path);

	// リソースマテリアルを取得する
	std::vector<ResMaterial> GetResMaterial(const wchar_t* path);

	// メッシュを取得する

	std::vector<Mesh*> GetMesh(const wchar_t* path);

	// マテリアルを取得する
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

	// 念のためにリソースも保持しておく
	std::unordered_map<const wchar_t*, std::vector<ResMesh>>                    m_ResMeshes{};
	std::unordered_map<const wchar_t*, std::vector<ResMaterial>>                m_ResMaterials{};

	AppResourceManager() {}
	~AppResourceManager() {}
};
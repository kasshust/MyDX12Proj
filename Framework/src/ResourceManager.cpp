#include <ResourceManager.h>

void AppResourceManager::Init() {}

void AppResourceManager::Release() {
}

bool AppResourceManager::CheckFilePath(const wchar_t* path) {
	std::wstring findPath;
	if (!SearchFilePathW(path, findPath))
	{
		ELOG("Error : File Path is not exist");
		return false;
	}

	// ファイル名であることをチェック.
	if (PathIsDirectoryW(findPath.c_str()) != FALSE)
	{
		ELOG("Error : This is not FilePath");
		return false;
	}
}

// Textureを読み込んでunordered_mapに登録する
bool AppResourceManager::LoadTexture(const wchar_t* path,
	ComPtr<ID3D12Device> pDevice,
	DescriptorPool* pPool,
	bool isSRGB,
	DirectX::ResourceUploadBatch& batch) {
	// 既に登録されている場合は何もしない
	if (m_Textures.count(path) > 0) return true;

	// ファイルパスが存在するかチェックします.
	if (!CheckFilePath(path)) return false;

	// テクスチャを読み込んで登録
	Texture* pTexture = new (std::nothrow) Texture();
	// インスタンス生成.
	if (pTexture == nullptr)
	{
		ELOG("Error : Out of memory.");
		return false;
	}

	// 初期化.
	if (!pTexture->Init(pDevice.Get(), pPool, path, isSRGB, batch))
	{
		ELOG("Error : Texture::Init() Failed.");
		pTexture->Term();
		return false;
	}

	m_Textures[path] = pTexture;
}

// Modelを読み込んでunordered_mapに登録する
bool AppResourceManager::LoadResModel(const wchar_t* path) {
	if (m_ResMeshes.count(path) == 0 || m_ResMaterials.count(path) == 0) {
		std::vector<ResMesh>        resMesh = std::vector<ResMesh>();
		std::vector<ResMaterial>    resMaterial = std::vector<ResMaterial>();

		// メッシュリソースをロード.
		if (!Res::LoadMesh(path, resMesh, resMaterial))
		{
			ELOG("Error : Load Mesh Failed. filepath = %ls", path);
			return false;
		}

		if (m_ResMeshes.count(path) == 0)        m_ResMeshes[path] = resMesh;
		if (m_ResMaterials.count(path) == 0)     m_ResMaterials[path] = resMaterial;
	}
	else return false;
}

// ResMeshからMeshを作成する
bool AppResourceManager::CreateMesh(ComPtr<ID3D12Device> pDevice, const wchar_t* key, std::vector<ResMesh> resMesh) {

	if (m_pMeshs.count(key) > 0) return true;

	std::vector<Mesh*> pMesh = std::vector<Mesh*>();

	// メモリを予約.
	pMesh.reserve(resMesh.size());

	// メッシュを初期化.
	for (size_t i = 0; i < resMesh.size(); ++i)
	{
		// メッシュ生成.
		auto mesh = new (std::nothrow) Mesh();

		// チェック.
		if (mesh == nullptr)
		{
			ELOG("Error : Out of memory.");
			return false;
		}

		// 初期化処理.
		if (!mesh->Init(pDevice.Get(), resMesh[i]))
		{
			ELOG("Error : Mesh Initialize Failed.");
			delete mesh;
			return false;
		}

		// 成功したら登録.
		pMesh.push_back(mesh);
	}

	// メモリ最適化.
	pMesh.shrink_to_fit();

	m_pMeshs[key] = pMesh;

	return true;
}

bool AppResourceManager::CreateMaterial(ComPtr<ID3D12Device> pDevice, const wchar_t* key, std::vector<ResMaterial> resMaterial, DescriptorPool* resPool) {
	// メモリを予約.
	std::vector<Material*> pMaterial = std::vector<Material*>();
	pMaterial.reserve(resMaterial.size());

	// マテリアルを初期化.
	for (size_t i = 0; i < resMaterial.size(); ++i)
	{
		// マテリアル生成.
		auto material = new (std::nothrow) Material();

		// チェック.
		if (material == nullptr)
		{
			ELOG("Error : Out of memory.");
			return false;
		}

		if (!material->Init(
			pDevice.Get(),
			resPool,
			sizeof(CommonCb::CbMaterial),
			resMaterial.size()))
		{
			ELOG("Error : Material::Init() Failed.");
			return false;
		}

		// 成功したら登録.
		pMaterial.push_back(material);
	}

	// メモリ最適化.
	pMaterial.shrink_to_fit();

	m_pMaterials[key] = pMaterial;

	return true;
}

// テクスチャを取得する
Texture* AppResourceManager::GetTexture(const wchar_t* path) {
	auto it = m_Textures.find(path);
	if (it != m_Textures.end()) {
		return it->second;
	}
	return nullptr;
}

// テクスチャに対するロードとゲットを同時に行う
Texture* AppResourceManager::LoadGetTexture(const wchar_t* path,
	ComPtr<ID3D12Device> pDevice,
	DescriptorPool* pPool,
	bool isSRGB,
	DirectX::ResourceUploadBatch& batch) {
	if (LoadTexture(path, pDevice, pPool, isSRGB, batch)) {
		return GetTexture(path);
	}
	{
		ELOG("Error : Load Texture Failed. filepath = %ls", path);
		return nullptr;
	}
}

// リソースメッシュを取得する
std::vector<ResMesh> AppResourceManager::GetResMesh(const wchar_t* path) {
	auto it = m_ResMeshes.find(path);
	if (it != m_ResMeshes.end()) {
		return it->second;
	}
	return std::vector<ResMesh>();
}

// リソースマテリアルを取得する
std::vector<ResMaterial> AppResourceManager::GetResMaterial(const wchar_t* path) {
	auto it = m_ResMaterials.find(path);
	if (it != m_ResMaterials.end()) {
		return it->second;
	}
	return std::vector<ResMaterial>();
}

// メッシュを取得する

std::vector<Mesh*> AppResourceManager::GetMesh(const wchar_t* path) {
	auto it = m_pMeshs.find(path);
	if (it != m_pMeshs.end()) {
		return it->second;
	}
	return std::vector<Mesh*>();
}

// マテリアルを取得する
std::vector<Material*> AppResourceManager::GetMaterial(const wchar_t* path) {
	auto it = m_pMaterials.find(path);
	if (it != m_pMaterials.end()) {
		return it->second;
	}
	return std::vector<Material*>();
}

const std::unordered_map<const wchar_t*, Texture*> AppResourceManager::GetTexturesMap() {
	return m_Textures;
}

const std::unordered_map<const wchar_t*, std::vector<ResMesh>>     AppResourceManager::GetResMeshesMap() {
	return m_ResMeshes;
}

const std::unordered_map<const wchar_t*, std::vector<ResMaterial>> AppResourceManager::GetResMaterialsMap() {
	return m_ResMaterials;
}
#include <ResourceManager.h>

void AppResourceManager::Init() {

}

void AppResourceManager::Release() {
}

void AppResourceManager::AddShader(
	const std::wstring			path,
	Shader*					shader
)
{
	if (shader == nullptr) return ;
	m_pShaders[path] = shader;
}

Shader* AppResourceManager::GetShader(
	const std::wstring& path
)
{
	if (m_pShaders.empty()) return nullptr;
	auto it = m_pShaders.find(path);
	if (it != m_pShaders.end()) {
		return it->second;
	}
	return nullptr;
}

bool AppResourceManager::CheckFilePath(const std::wstring& path) {
	std::wstring findPath;
	if (!SearchFilePathW(path.c_str(), findPath))
	{
		ELOG("Error : File Path is not exist  = %ls", findPath.c_str());
		return false;
	}

	// ファイル名であることをチェック.
	if (PathIsDirectoryW(findPath.c_str()) != FALSE)
	{
		ELOG("Error : This is not FilePath  = %ls", findPath.c_str());
		return false;
	}
}

// Textureを読み込んでunordered_mapに登録する
bool AppResourceManager::LoadTexture(const std::wstring path,
	ComPtr<ID3D12Device> pDevice,
	DescriptorPool* pPool,
	bool isSRGB,
	DirectX::ResourceUploadBatch& batch) {
	// 既に登録されている場合は何もしない
	if (m_Textures.count(path) > 0) return true;

	// ファイルパスが存在するかチェックします.
	if (!CheckFilePath(path)) {
		ELOG("Error : Load Texture Failed. filepath = %ls", path);
		return false;
	}

	// テクスチャを読み込んで登録
	Texture* pTexture = new (std::nothrow) Texture();
	// インスタンス生成.
	if (pTexture == nullptr)
	{
		ELOG("Error : Out of memory.");
		return false;
	}

	// 初期化.
	if (!pTexture->Init(pDevice.Get(), pPool, path.c_str(), isSRGB, batch))
	{
		ELOG("Error : Texture::Init() Failed.");
		pTexture->Term();
		return false;
	}

	m_Textures.insert(std::make_pair(path, pTexture));
}

// Modelを読み込んでunordered_mapに登録する
bool AppResourceManager::LoadResModel(const std::wstring path) {
	if (m_ResMeshes.count(path) == 0 || m_ResMaterials.count(path) == 0) {
		std::vector<ResMesh>        resMesh = std::vector<ResMesh>();
		std::vector<ResMaterial>    resMaterial = std::vector<ResMaterial>();

		// メッシュリソースをロード.
		if (!Res::LoadMesh(path.c_str(), resMesh, resMaterial))
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
bool AppResourceManager::CreateMesh(ComPtr<ID3D12Device> pDevice, const std::wstring key, std::vector<ResMesh> resMesh) {

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

bool AppResourceManager::CreateMaterial(ComPtr<ID3D12Device> pDevice, const std::wstring key, std::vector<ResMaterial> resMaterial, DescriptorPool* resPool) {
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
Texture* AppResourceManager::GetTexture(const std::wstring& path) {
	auto it = m_Textures.find(path);
	if (it != m_Textures.end()) {
		return it->second;
	}
	return nullptr;
}

// テクスチャに対するロードとゲットを同時に行う
Texture* AppResourceManager::LoadGetTexture(const std::wstring path,
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
std::vector<ResMesh> AppResourceManager::GetResMesh(const std::wstring& path) {
	auto it = m_ResMeshes.find(path);
	if (it != m_ResMeshes.end()) {
		return it->second;
	}
	return std::vector<ResMesh>();
}

// リソースマテリアルを取得する
std::vector<ResMaterial> AppResourceManager::GetResMaterial(const std::wstring& path) {
	auto it = m_ResMaterials.find(path);
	if (it != m_ResMaterials.end()) {
		return it->second;
	}
	return std::vector<ResMaterial>();
}

// メッシュを取得する

std::vector<Mesh*> AppResourceManager::GetMesh(const std::wstring& path) {
	auto it = m_pMeshs.find(path);
	if (it != m_pMeshs.end()) {
		return it->second;
	}
	return std::vector<Mesh*>();
}

// マテリアルを取得する
std::vector<Material*> AppResourceManager::GetMaterial(const std::wstring& path) {
	auto it = m_pMaterials.find(path);
	if (it != m_pMaterials.end()) {
		return it->second;
	}
	return std::vector<Material*>();
}

const std::unordered_map<std::wstring, Texture*> AppResourceManager::GetTexturesMap() {
	return m_Textures;
}

const std::unordered_map<std::wstring, std::vector<ResMesh>>     AppResourceManager::GetResMeshesMap() {
	return m_ResMeshes;
}

const std::unordered_map<std::wstring, std::vector<ResMaterial>> AppResourceManager::GetResMaterialsMap() {
	return m_ResMaterials;
}
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

	// �t�@�C�����ł��邱�Ƃ��`�F�b�N.
	if (PathIsDirectoryW(findPath.c_str()) != FALSE)
	{
		ELOG("Error : This is not FilePath  = %ls", findPath.c_str());
		return false;
	}
}

// Texture��ǂݍ����unordered_map�ɓo�^����
bool AppResourceManager::LoadTexture(const std::wstring path,
	ComPtr<ID3D12Device> pDevice,
	DescriptorPool* pPool,
	bool isSRGB,
	DirectX::ResourceUploadBatch& batch) {
	// ���ɓo�^����Ă���ꍇ�͉������Ȃ�
	if (m_Textures.count(path) > 0) return true;

	// �t�@�C���p�X�����݂��邩�`�F�b�N���܂�.
	if (!CheckFilePath(path)) {
		ELOG("Error : Load Texture Failed. filepath = %ls", path);
		return false;
	}

	// �e�N�X�`����ǂݍ���œo�^
	Texture* pTexture = new (std::nothrow) Texture();
	// �C���X�^���X����.
	if (pTexture == nullptr)
	{
		ELOG("Error : Out of memory.");
		return false;
	}

	// ������.
	if (!pTexture->Init(pDevice.Get(), pPool, path.c_str(), isSRGB, batch))
	{
		ELOG("Error : Texture::Init() Failed.");
		pTexture->Term();
		return false;
	}

	m_Textures.insert(std::make_pair(path, pTexture));
}

// Model��ǂݍ����unordered_map�ɓo�^����
bool AppResourceManager::LoadResModel(const std::wstring path) {
	if (m_ResMeshes.count(path) == 0 || m_ResMaterials.count(path) == 0) {
		std::vector<ResMesh>        resMesh = std::vector<ResMesh>();
		std::vector<ResMaterial>    resMaterial = std::vector<ResMaterial>();

		// ���b�V�����\�[�X�����[�h.
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

// ResMesh����Mesh���쐬����
bool AppResourceManager::CreateMesh(ComPtr<ID3D12Device> pDevice, const std::wstring key, std::vector<ResMesh> resMesh) {

	if (m_pMeshs.count(key) > 0) return true;

	std::vector<Mesh*> pMesh = std::vector<Mesh*>();

	// ��������\��.
	pMesh.reserve(resMesh.size());

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
		pMesh.push_back(mesh);
	}

	// �������œK��.
	pMesh.shrink_to_fit();

	m_pMeshs[key] = pMesh;

	return true;
}

bool AppResourceManager::CreateMaterial(ComPtr<ID3D12Device> pDevice, const std::wstring key, std::vector<ResMaterial> resMaterial, DescriptorPool* resPool) {
	// ��������\��.
	std::vector<Material*> pMaterial = std::vector<Material*>();
	pMaterial.reserve(resMaterial.size());

	// �}�e���A����������.
	for (size_t i = 0; i < resMaterial.size(); ++i)
	{
		// �}�e���A������.
		auto material = new (std::nothrow) Material();

		// �`�F�b�N.
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

		// ����������o�^.
		pMaterial.push_back(material);
	}

	// �������œK��.
	pMaterial.shrink_to_fit();

	m_pMaterials[key] = pMaterial;

	return true;
}

// �e�N�X�`�����擾����
Texture* AppResourceManager::GetTexture(const std::wstring& path) {
	auto it = m_Textures.find(path);
	if (it != m_Textures.end()) {
		return it->second;
	}
	return nullptr;
}

// �e�N�X�`���ɑ΂��郍�[�h�ƃQ�b�g�𓯎��ɍs��
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

// ���\�[�X���b�V�����擾����
std::vector<ResMesh> AppResourceManager::GetResMesh(const std::wstring& path) {
	auto it = m_ResMeshes.find(path);
	if (it != m_ResMeshes.end()) {
		return it->second;
	}
	return std::vector<ResMesh>();
}

// ���\�[�X�}�e���A�����擾����
std::vector<ResMaterial> AppResourceManager::GetResMaterial(const std::wstring& path) {
	auto it = m_ResMaterials.find(path);
	if (it != m_ResMaterials.end()) {
		return it->second;
	}
	return std::vector<ResMaterial>();
}

// ���b�V�����擾����

std::vector<Mesh*> AppResourceManager::GetMesh(const std::wstring& path) {
	auto it = m_pMeshs.find(path);
	if (it != m_pMeshs.end()) {
		return it->second;
	}
	return std::vector<Mesh*>();
}

// �}�e���A�����擾����
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
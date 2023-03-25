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

// ResourceManager�N���X
class AppResourceManager {
public:

    static AppResourceManager& GetInstance()
    {
        static AppResourceManager instance;
        return instance;
    }


    void Init() {}

    void Release() {
        // m_Textures.clear();
        // m_ResMeshes.clear();
        // m_ResMaterials.clear();
    }

    bool CheckFilePath(const wchar_t* path) {
        std::wstring findPath;
        if (!SearchFilePathW(path, findPath))
        {
            ELOG("Error : File Path is not exist");
            return false;
        }

        // �t�@�C�����ł��邱�Ƃ��`�F�b�N.
        if (PathIsDirectoryW(findPath.c_str()) != FALSE)
        {
            ELOG("Error : This is not FilePath");
            return false;
        }
    }

    // Texture��ǂݍ����unordered_map�ɓo�^����
    bool LoadTexture(const wchar_t* path,
        ComPtr<ID3D12Device> pDevice,
        DescriptorPool* pPool,
        bool isSRGB,
        DirectX::ResourceUploadBatch& batch) {

        // ���ɓo�^����Ă���ꍇ�͉������Ȃ�
        if (m_Textures.count(path) > 0) return false;

        // �t�@�C���p�X�����݂��邩�`�F�b�N���܂�.
        if (!CheckFilePath(path)) return false;

        // �e�N�X�`����ǂݍ���œo�^
        auto tex = &Texture();
        // �C���X�^���X����.
        if (tex == nullptr)
        {
            ELOG("Error : Out of memory.");
            return false;
        }

        // ������.
        if (!tex->Init(pDevice.Get(), pPool, path, isSRGB, batch))
        {
            ELOG("Error : Texture::Init() Failed.");
            tex->Term();
            return false;
        }

        m_Textures[path] = tex;
    }

    // Model��ǂݍ����unordered_map�ɓo�^����
    bool LoadResModel(const wchar_t* path
        ) {

        if (m_ResMeshes.count(path) == 0 || m_ResMaterials.count(path) == 0) {
            std::shared_ptr <std::vector<ResMesh>>        resMesh       = std::make_shared<std::vector<ResMesh>>();;
            std::shared_ptr <std::vector<ResMaterial>>    resMaterial   = std::make_shared<std::vector<ResMaterial>>();;

            // ���b�V�����\�[�X�����[�h.
            if (!Res::LoadMesh(path, *resMesh, *resMaterial))
            {
                ELOG("Error : Load Mesh Failed. filepath = %ls", path);
                return false;
            }

            if (m_ResMeshes.count(path) == 0)        m_ResMeshes[path] = resMesh;
            if (m_ResMaterials.count(path) == 0)     m_ResMaterials[path] = resMaterial;
        }
        else return false;
    }

    // �e�N�X�`�����擾����
    Texture* GetTexture(const wchar_t* path) {
        auto it = m_Textures.find(path);
        if (it != m_Textures.end()) {
            return it->second;
        }
        return nullptr;
    }

    // ���b�V�����擾����
    std::shared_ptr<std::vector<ResMesh>> GetMesh(const wchar_t*  path) {
        auto it = m_ResMeshes.find(path);
        if (it != m_ResMeshes.end()) {
            return it->second;
        }
        return nullptr;
    }

    // �}�e���A�����擾����
    std::shared_ptr<std::vector<ResMaterial>> GetMaterial(const wchar_t* path) {
        auto it = m_ResMaterials.find(path);
        if (it != m_ResMaterials.end()) {
            return it->second;
        }
        return nullptr;
    }

    const std::unordered_map<const wchar_t*, Texture*> GetTexturesMap() {
        return m_Textures;
    }

    const std::unordered_map<const wchar_t*, std::shared_ptr<std::vector<ResMesh>>> GetResMeshesMap() {
        return m_ResMeshes;
    }

    const std::unordered_map<const wchar_t*, std::shared_ptr<std::vector<ResMaterial>>> GetResMaterialsMap() {
        return m_ResMaterials;
    }

private:
    std::unordered_map<const wchar_t*, Texture*>                                    m_Textures{};
    std::unordered_map<const wchar_t*, std::shared_ptr<std::vector<ResMesh>>>       m_ResMeshes{};
    std::unordered_map<const wchar_t*, std::shared_ptr<std::vector<ResMaterial>>>   m_ResMaterials{};

    AppResourceManager() {}
    ~AppResourceManager() {}
};
#pragma once
#include <App.h>
#include <vector>
#include <Material.h>
#include <Mesh.h>
#include <ConstantBuffer.h>
#include <Shader.h>
#include <ResourceManager.h>

class Shader;

class Model
{
public:
	Model() = default;
	~Model() = default;

	bool LoadModel(const wchar_t* filePath, ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue);
	std::vector<Material*> GetMaterials();
	void SetTexture(Material* mat, Material::TEXTURE_USAGE usage, const wchar_t* path, ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, bool isSRGB, DirectX::ResourceUploadBatch& batch, AppResourceManager& manager);
	void DrawModel(ID3D12GraphicsCommandList* pCmd, int frameIndex, CommonBufferManager& commonBufferManager, const IBLBaker& baker, Shader& shader);
	void Release();

	bool CreateMeshBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool);
	void UpdateWorldMatrix(int frameindex, Matrix& modelMatrix);
	ConstantBuffer		m_MeshCB[App::FrameCount];           //!< メッシュ用バッファです.

	wchar_t m_FilePath[OFS_MAXPATHNAME];

private:

};
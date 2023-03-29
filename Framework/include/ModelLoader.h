#pragma once
#include <App.h>
#include <vector>
#include <Material.h>
#include <Mesh.h>
#include <ConstantBuffer.h>
#include <Shader.h>
#include <ResourceManager.h>
#include <SkyTextureManager.h>

class Model
{
public:
	Model() = default;
	~Model() = default;
	void Release();

	ConstantBuffer		m_MeshCB[App::FrameCount];           //!< メッシュ用バッファです.
	wchar_t				m_ModelPath[OFS_MAXPATHNAME];


	bool LoadModel(const wchar_t* filePath, ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue);
	std::vector<Material*> GetMaterials();
	void SetTexture(Material* mat, Material::TEXTURE_USAGE usage, const wchar_t* path, ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, bool isSRGB, DirectX::ResourceUploadBatch& batch, AppResourceManager& manager);
	
	bool CreateMeshBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool);
	void UpdateWorldMatrix(int frameindex, Matrix& modelMatrix);
	
	void DrawModel(ID3D12GraphicsCommandList* pCmd, int frameIndex, CommonBufferManager& commonBufferManager, const SkyManager& manager);

private:

};
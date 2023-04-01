#pragma once
#include <App.h>
#include <vector>
#include <Material.h>
#include <Mesh.h>
#include <ConstantBuffer.h>
#include <ModelShader.h>
#include <ResourceManager.h>
#include <SkyTextureManager.h>

class Model
{
public:
	Model() = default;
	~Model() = default;
	void Release();

	ConstantBuffer		m_MeshCB[App::FrameCount];           //!< メッシュ用バッファです.
	std::wstring		m_ModelPath;


	bool LoadModel(std::wstring filePath, ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue);
	std::vector<Material*> GetMaterials();
	void SetTexture(Material* mat, Material::TEXTURE_USAGE usage, std::wstring path, ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, bool isSRGB, DirectX::ResourceUploadBatch& batch, AppResourceManager& manager);
	
	bool CreateMeshBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool);
	void UpdateMeshBuffer(int frameindex, CommonCb::CbMesh& cb);
	
	void DrawModel(ID3D12GraphicsCommandList* pCmd, int frameIndex, CommonBufferManager& commonBufferManager, const SkyManager& manager);
	void DrawModelRaw(ID3D12GraphicsCommandList* pCmd, int frameIndex);
private:

};
#pragma once
#include <App.h>
#include <vector>
#include <Material.h>
#include <Mesh.h>
#include <ConstantBuffer.h>

class ModelLoader
{
public:
	ModelLoader() = default;
	~ModelLoader() = default;

	bool LoadModel(const wchar_t* filePath, ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue);
	bool CreateMesh(ComPtr<ID3D12Device> pDevice, std::vector<ResMesh> resMesh);
	bool CreateMaterial(ComPtr<ID3D12Device> pDevice, std::vector<ResMaterial> resMaterial, DescriptorPool* resPool);
	void Release();

	const std::vector<Mesh*>& GetMeshes() const { return m_pMesh; }
	const Material& GetMaterial() const { return m_Material; }
	ConstantBuffer						 m_MeshCB[App::FrameCount];           //!< メッシュ用バッファです.

	void ModelLoader::UpdateWorldMatrix(int frameindex);

private:
	std::vector<Mesh*> m_pMesh;
	Material m_Material;
	bool ModelLoader::CreateMeshBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool);
};
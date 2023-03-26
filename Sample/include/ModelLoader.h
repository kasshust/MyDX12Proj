#pragma once
#include <App.h>
#include <vector>
#include <Material.h>
#include <Mesh.h>
#include <ConstantBuffer.h>
#include <Shader.h>

class Model
{
public:
	Model() = default;
	~Model() = default;

	bool LoadModel(const wchar_t* filePath, ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue);
	void DrawModel(ID3D12GraphicsCommandList* pCmd, int frameIndex, const CommonBufferManager& commonBufferManager, const IBLBaker& baker, Shader& shader);
	void Release();
	ConstantBuffer						 m_MeshCB[App::FrameCount];           //!< メッシュ用バッファです.

	void Model::UpdateWorldMatrix(int frameindex);

	const wchar_t* m_FilePath;

private:
	// std::vector<Mesh*> m_pMesh;
	// Material m_Material;
	bool Model::CreateMeshBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool);
};
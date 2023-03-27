#pragma once
#include <App.h>
#include <vector>
#include <Material.h>
#include <Mesh.h>
#include <ConstantBuffer.h>
#include <Shader.h>

class Shader;

class Model
{
public:
	Model() = default;
	~Model() = default;

	bool LoadModel(const wchar_t* filePath, ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue);
	void DrawModel(ID3D12GraphicsCommandList* pCmd, int frameIndex, CommonBufferManager& commonBufferManager, const IBLBaker& baker, Shader& shader);
	void Release();

	bool CreateMeshBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool);
	void UpdateWorldMatrix(int frameindex, Matrix& modelMatrix);
	ConstantBuffer		m_MeshCB[App::FrameCount];           //!< メッシュ用バッファです.

	const wchar_t* m_FilePath;

private:

};
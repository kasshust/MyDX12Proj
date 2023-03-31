#include "CommonBufferManager.h"
#include "FileUtil.h"
#include "Logger.h"
#include "CommonStates.h"
#include "DirectXHelpers.h"
#include "SimpleMath.h"
using namespace DirectX;

//-----------------------------------------------------------------------------
// Using Statements
//-----------------------------------------------------------------------------
using namespace DirectX::SimpleMath;

bool CommonBufferManager::Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, float width, float height)
{
	if (!CreateLightBuffer(	pDevice.Get(),	resPool))	return false;
	if (!CreateCameraBuffer(pDevice.Get(),	resPool))	return false;
	if (!CreateMeshBuffer(	pDevice.Get(),	resPool))   return false;
	if (!CreateVertexBuffer(pDevice.Get()))				return false;
	if (!CreateMatrixConstantBuffer(pDevice.Get(), resPool, width, height)) return false;

	return true;
}

bool CommonBufferManager::CreateLightBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool) {
	for (auto i = 0; i < App::FrameCount; ++i)
	{
		if (!m_LightCB[i].Init(pDevice.Get(), pool, sizeof(CommonCb::CbLight)))
		{
			ELOG("Error : ConstantBuffer::Init() Failed.");
			return false;
		}
	}
	return true;
}
bool CommonBufferManager::CreateCameraBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool) {
	for (auto i = 0; i < App::FrameCount; ++i)
	{
		if (!m_CameraCB[i].Init(pDevice.Get(), pool, sizeof(CommonCb::CbCamera)))
		{
			ELOG("Error : ConstantBuffer::Init() Failed.");
			return false;
		}
	}
	return true;
}

bool CommonBufferManager::CreateMeshBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool) {
	for (auto i = 0; i < App::FrameCount; ++i)
	{
		if (!m_MeshCB[i].Init(pDevice.Get(), resPool, sizeof(CommonCb::CbMesh)))
		{
			ELOG("Error : ConstantBuffer::Init() Failed.");
			return false;
		}

		auto ptr = m_MeshCB[i].GetPtr<CommonCb::CbMesh>();
		ptr->World = Matrix::Identity;
	}
	return true;
}

bool CommonBufferManager::CreateVertexBuffer(ComPtr<ID3D12Device> pDevice) {
	struct Vertex
	{
		float px;
		float py;

		float tx;
		float ty;
	};

	if (!m_QuadVB.Init<Vertex>(pDevice.Get(), 3))
	{
		ELOG("Error : VertexBuffer::Init() Failed.");
		return false;
	}

	auto ptr = m_QuadVB.Map<Vertex>();
	assert(ptr != nullptr);
	ptr[0].px = -1.0f;  ptr[0].py = 1.0f;  ptr[0].tx = 0.0f;   ptr[0].ty = -1.0f;
	ptr[1].px = 3.0f;  ptr[1].py = 1.0f;  ptr[1].tx = 2.0f;   ptr[1].ty = -1.0f;
	ptr[2].px = -1.0f;  ptr[2].py = -3.0f;  ptr[2].tx = 0.0f;   ptr[2].ty = 1.0f;
	m_QuadVB.Unmap();

	return true;
}

bool CommonBufferManager::CreateMatrixConstantBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, float width, float height) {
	for (auto i = 0u; i < App::FrameCount; ++i)
	{
		// 定数バッファ初期化.
		if (!m_TransformCB[i].Init(pDevice.Get(), pool, sizeof(CommonCb::CbTransform)))
		{
			ELOG("Error : ConstantBuffer::Init() Failed.");
			return false;
		}

		// カメラ設定.
		auto eyePos = Vector3(0.0f, 0.0f, 1.0f);
		auto targetPos = Vector3::Zero;
		auto upward = Vector3::UnitY;

		// 垂直画角とアスペクト比の設定.
		auto fovY = DirectX::XMConvertToRadians(37.5f);
		auto aspect = static_cast<float>(width) / static_cast<float>(height);

		// 変換行列を設定.
		auto ptr = m_TransformCB[i].GetPtr<CommonCb::CbTransform>();
		ptr->View = Matrix::CreateLookAt(eyePos, targetPos, upward);
		ptr->Proj = Matrix::CreatePerspectiveFieldOfView(fovY, aspect, 0.1f, 1000.0f);
	}

	return true;
}

CommonCb::CbLight* CommonBufferManager::GetLightProperty(int frameindex) {
	CommonCb::CbLight* ptr = m_LightCB[frameindex].GetPtr<CommonCb::CbLight>();
	return ptr;
}

void CommonBufferManager::SetLightProperty(int frameindex, CommonCb::CbLight& prop) {
	CommonCb::CbLight* ptr = m_LightCB[frameindex].GetPtr<CommonCb::CbLight>();
	ptr = &prop;
}

void CommonBufferManager::UpdateLightBuffer(int frameindex, float texSize, float mipCount, Vector3 direction, float intensity) {
	auto ptr = m_LightCB[frameindex].GetPtr<CommonCb::CbLight>();
	ptr->TextureSize	= texSize;
	ptr->MipCount		= mipCount;
	ptr->LightDirection = direction;
	ptr->LightIntensity = intensity;

}

void CommonBufferManager::UpdateLightBufferVP(int frameindex, Vector3 direction) {
	auto ptr = m_LightCB[frameindex].GetPtr<CommonCb::CbLight>();
	
	// LVPの更新
	if (direction.Length() == 0) {
		return;
	}

	XMFLOAT3 lightPos = -direction * 40.0f;
	XMFLOAT3 lightDest = { 0.0f, 0.0f, 0.0f };
	XMMATRIX view = XMMatrixLookAtLH(
		{ lightPos.x, lightPos.y, lightPos.z },
		{ lightDest.x, lightDest.y, lightDest.z },
		{ 0.0f, 1.0f, 0.0f }
	);

	XMMATRIX projection = XMMatrixOrthographicLH(3.0f, 3.0f, 0.1f, 100.0f);

	// XMMATRIX projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), 1.0f, 1.0f, 100.0f);
	XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, XMMatrixTranspose(view * projection));

	ptr->LightVP = mat;
}

void CommonBufferManager::UpdateCameraBuffer(int frameindex, Vector3 pos) {
	auto ptr = m_CameraCB[frameindex].GetPtr<CommonCb::CbCamera>();
	ptr->CameraPosition = pos;
}

void CommonBufferManager::UpdateViewProjMatrix(int frameindex, Matrix& view, Matrix& proj) {
	auto ptr = m_TransformCB[frameindex].GetPtr<CommonCb::CbTransform>();
	ptr->View = view;
	ptr->Proj = proj;
}

void CommonBufferManager::UpdateWorldMatrix(int frameindex, Matrix& modelMatrix)
{
	auto ptr = m_MeshCB[frameindex].GetPtr<CommonCb::CbMesh>();
	ptr->World = modelMatrix;
}

void CommonBufferManager::Term()
{
	m_QuadVB.Term();
	for (auto i = 0; i < App::FrameCount; ++i)
	{
		m_LightCB[i].Term();
		m_CameraCB[i].Term();
		m_MeshCB[i].Term();
		m_TransformCB[i].Term();
	}
}

void CommonBufferManager::SetRTManager(CommonRTManager* m) {
	m_RTManager = m;
};
CommonRTManager* CommonBufferManager::GetRTManager() {
	return m_RTManager;
};
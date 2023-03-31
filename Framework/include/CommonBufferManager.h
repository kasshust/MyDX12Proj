#pragma once
#include <App.h>
#include <Camera.h>
#include <ConstantBuffer.h>
#include <Material.h>
#include <Camera.h>
#include <RootSignature.h>
#include <RootSignature.h>
#include <CommonRTVManager.h>

using namespace DirectX::SimpleMath;

namespace CommonCb {
	///////////////////////////////////////////////////////////////////////////////
	// CbMesh structure
	///////////////////////////////////////////////////////////////////////////////
	struct alignas(256) CbMesh
	{
		Matrix   World;      //!< ワールド行列です.
	};

	///////////////////////////////////////////////////////////////////////////////
	// CbTransform structure
	///////////////////////////////////////////////////////////////////////////////
	struct alignas(256) CbTransform
	{
		Matrix   View;       //!< ビュー行列です.
		Matrix   Proj;       //!< 射影行列です.
	};

	///////////////////////////////////////////////////////////////////////////////
	// CbLight structure
	///////////////////////////////////////////////////////////////////////////////
	struct alignas(256) CbLight
	{
		float		TextureSize;		//!< キューブマップサイズです.
		float		MipCount;			//!< ミップ数です.
		float		LightIntensity;		//!< ライト強度です.
		float		Padding0;			//!< パディング.
		Vector3		LightDirection;		//!< ディレクショナルライトの方向.
		float		Padding1;			//!< パディング.
		Matrix		LightVP;			//!< ディレクショナルライトのViewProjection
	};

	///////////////////////////////////////////////////////////////////////////////
	// CbCamera structure
	///////////////////////////////////////////////////////////////////////////////
	struct alignas(256) CbCamera
	{
		Vector3  CameraPosition;    //!< カメラ位置です.
	};

	///////////////////////////////////////////////////////////////////////////////
	// CbMaterial structure
	///////////////////////////////////////////////////////////////////////////////
	struct alignas(256) CbMaterial
	{
		Vector4  Param00;
		Vector4  Param01;
		Vector4  Param02;
		Vector4  Param03;
		Vector4  Param04;
		Vector4  Param05;
		Vector4  Param06;
		Vector4  Param07;
		Vector4  Param08;
		Vector4  Param09;
		Vector4  Param10;
		Vector4  Param11;
		Vector4  Param12;
		Vector4  Param13;
		Vector4  Param14;
		Vector4  Param15;
	};
} // namespace

class CommonBufferManager {
public:
	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, float width, float height);

	void CommonBufferManager::UpdateLightBuffer(int frameindex, float texSize, float mipCount, Vector3 direction, float intensity);
	void CommonBufferManager::UpdateLightBufferVP(int frameindex, Vector3 direction);
	void CommonBufferManager::UpdateCameraBuffer(int frameindex, Vector3 pos);
	void CommonBufferManager::UpdateViewProjMatrix(int frameindex, Matrix& view, Matrix& proj);
	void CommonBufferManager::UpdateWorldMatrix(int frameindex, Matrix& modelMat);
	void Term();
	CommonCb::CbLight* GetLightProperty(int frameindex);
	void SetLightProperty(int frameindex, CommonCb::CbLight& prop);


	VertexBuffer        m_QuadVB;                            //!< 頂点バッファです.
	ConstantBuffer      m_LightCB[App::FrameCount];          //!< ライトバッファです.
	ConstantBuffer      m_CameraCB[App::FrameCount];         //!< カメラバッファです.
	ConstantBuffer      m_TransformCB[App::FrameCount];      //!< 変換用バッファです.
	ConstantBuffer		m_MeshCB[App::FrameCount];           //!< メッシュ用バッファです.
	
	CommonRTManager*	m_RTManager;

	void SetRTManager(CommonRTManager* m);
	CommonRTManager* GetRTManager();
private:
	//=========================================================================
	// private variables.
	//=========================================================================
	bool CommonBufferManager::CreateMeshBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool);
	bool CommonBufferManager::CreateLightBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool);
	bool CommonBufferManager::CreateCameraBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool);
	bool CommonBufferManager::CreateVertexBuffer(ComPtr<ID3D12Device> pDevice);
	bool CommonBufferManager::CreateMatrixConstantBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, float width, float height);



	
};
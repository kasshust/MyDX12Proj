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
		float		ShadowBias;			//!< Bias
		float		ShadowStrength;		//!< Strength
	};

	///////////////////////////////////////////////////////////////////////////////
	// CbCommon structure
	///////////////////////////////////////////////////////////////////////////////
	struct alignas(256) CbCommon
	{
		Vector3  CameraPosition;    //!< カメラ位置です.
		float	 Padding0;
		Vector2  FogArea;
		float	 Padding1;
		float	 Padding2;
		Vector3  FogColor;
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

	
	void UpdateShadowBuffer(int frameindex, Vector3 direction,  float shadowLightPosDist, Vector4& OrthographParam);
	void UpdateCommonBuffer(int frameindex, CommonCb::CbCommon& cb);
	void UpdateLightBuffer(int frameindex, CommonCb::CbLight& cb);
	void UpdateViewProjMatrix(int frameindex, CommonCb::CbTransform& cbt);
	void UpdateMeshBuffer(int frameindex, CommonCb::CbMesh& cb);
	
	void Term();

	CommonCb::CbLight* GetLightProperty(int frameindex);
	void SetLightProperty(int frameindex, CommonCb::CbLight& prop);
	CommonCb::CbCommon* GetCommonProperty(int frameindex);
	void SetCommonProperty(int frameindex, CommonCb::CbCommon& prop);

	VertexBuffer        m_QuadVB;                            //!< 頂点バッファです.
	ConstantBuffer      m_LightCB[App::FrameCount];          //!< ライトバッファです.
	ConstantBuffer      m_CommonCB[App::FrameCount];         //!< 一般バッファです.
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
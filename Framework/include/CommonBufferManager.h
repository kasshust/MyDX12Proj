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
		Matrix   World;      //!< ���[���h�s��ł�.
	};

	///////////////////////////////////////////////////////////////////////////////
	// CbTransform structure
	///////////////////////////////////////////////////////////////////////////////
	struct alignas(256) CbTransform
	{
		Matrix   View;       //!< �r���[�s��ł�.
		Matrix   Proj;       //!< �ˉe�s��ł�.
	};

	///////////////////////////////////////////////////////////////////////////////
	// CbLight structure
	///////////////////////////////////////////////////////////////////////////////
	struct alignas(256) CbLight
	{
		float		TextureSize;		//!< �L���[�u�}�b�v�T�C�Y�ł�.
		float		MipCount;			//!< �~�b�v���ł�.
		float		LightIntensity;		//!< ���C�g���x�ł�.
		float		Padding0;			//!< �p�f�B���O.
		Vector3		LightDirection;		//!< �f�B���N�V���i�����C�g�̕���.
		float		Padding1;			//!< �p�f�B���O.
		Matrix		LightVP;			//!< �f�B���N�V���i�����C�g��ViewProjection
		float		ShadowBias;			//!< Bias
		float		ShadowStrength;		//!< Strength
	};

	///////////////////////////////////////////////////////////////////////////////
	// CbCommon structure
	///////////////////////////////////////////////////////////////////////////////
	struct alignas(256) CbCommon
	{
		Vector3  CameraPosition;    //!< �J�����ʒu�ł�.
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

	VertexBuffer        m_QuadVB;                            //!< ���_�o�b�t�@�ł�.
	ConstantBuffer      m_LightCB[App::FrameCount];          //!< ���C�g�o�b�t�@�ł�.
	ConstantBuffer      m_CommonCB[App::FrameCount];         //!< ��ʃo�b�t�@�ł�.
	ConstantBuffer      m_TransformCB[App::FrameCount];      //!< �ϊ��p�o�b�t�@�ł�.
	ConstantBuffer		m_MeshCB[App::FrameCount];           //!< ���b�V���p�o�b�t�@�ł�.
	
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
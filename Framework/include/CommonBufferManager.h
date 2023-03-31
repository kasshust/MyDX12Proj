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
	};

	///////////////////////////////////////////////////////////////////////////////
	// CbCamera structure
	///////////////////////////////////////////////////////////////////////////////
	struct alignas(256) CbCamera
	{
		Vector3  CameraPosition;    //!< �J�����ʒu�ł�.
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


	VertexBuffer        m_QuadVB;                            //!< ���_�o�b�t�@�ł�.
	ConstantBuffer      m_LightCB[App::FrameCount];          //!< ���C�g�o�b�t�@�ł�.
	ConstantBuffer      m_CameraCB[App::FrameCount];         //!< �J�����o�b�t�@�ł�.
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
#pragma once
#include <App.h>
#include <Camera.h>
#include <ConstantBuffer.h>
#include <Material.h>
#include <SphereMapConverter.h>
#include <IBLBaker.h>
#include <SkyBox.h>
#include <Camera.h>
#include <RootSignature.h>

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
        float   TextureSize;      //!< �L���[�u�}�b�v�T�C�Y�ł�.
        float   MipCount;         //!< �~�b�v���ł�.
        float   LightIntensity;   //!< ���C�g���x�ł�.
        float   Padding0;         //!< �p�f�B���O.
        Vector3 LightDirection;   //!< �f�B���N�V���i�����C�g�̕���.
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
        Vector3 BaseColor;  //!< ��{�F.
        float   Alpha;      //!< ���ߓx.
        float   Roughness;  //!< �ʂ̑e���ł�(�͈͂�[0,1]).
        float   Metallic;   //!< �����x�ł�(�͈͂�[0,1]).
    };
} // namespace

class CommonBufferManager {
public:
    bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, float width, float height);



    void CommonBufferManager::UpdateLightBuffer(int frameindex, float texSize, float mipCount);
    void CommonBufferManager::UpdateCameraBuffer(int frameindex, Vector3 pos);
    void CommonBufferManager::UpdateViewProjMatrix(int frameindex, Matrix& view, Matrix& proj);
    void CommonBufferManager::UpdateWorldMatrix(int frameindex);
    void Term();

    VertexBuffer                    m_QuadVB;                            //!< ���_�o�b�t�@�ł�.
    ConstantBuffer                  m_LightCB[App::FrameCount];          //!< ���C�g�o�b�t�@�ł�.
    ConstantBuffer                  m_CameraCB[App::FrameCount];         //!< �J�����o�b�t�@�ł�.
    ConstantBuffer                  m_TransformCB[App::FrameCount];      //!< �ϊ��p�o�b�t�@�ł�.
    ConstantBuffer                  m_MeshCB[App::FrameCount];           //!< ���b�V���p�o�b�t�@�ł�.

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    bool CommonBufferManager::CreateLightBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool);
    bool CommonBufferManager::CreateCameraBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool);
    bool CommonBufferManager::CreateMeshBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool);
    bool CommonBufferManager::CreateVertexBuffer(ComPtr<ID3D12Device> pDevice);
    bool CommonBufferManager::CreateMatrixConstantBuffer(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, float width, float height);


};
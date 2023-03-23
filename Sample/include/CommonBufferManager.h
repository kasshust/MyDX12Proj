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
        float   TextureSize;      //!< キューブマップサイズです.
        float   MipCount;         //!< ミップ数です.
        float   LightIntensity;   //!< ライト強度です.
        float   Padding0;         //!< パディング.
        Vector3 LightDirection;   //!< ディレクショナルライトの方向.
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
        Vector3 BaseColor;  //!< 基本色.
        float   Alpha;      //!< 透過度.
        float   Roughness;  //!< 面の粗さです(範囲は[0,1]).
        float   Metallic;   //!< 金属度です(範囲は[0,1]).
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

    VertexBuffer                    m_QuadVB;                            //!< 頂点バッファです.
    ConstantBuffer                  m_LightCB[App::FrameCount];          //!< ライトバッファです.
    ConstantBuffer                  m_CameraCB[App::FrameCount];         //!< カメラバッファです.
    ConstantBuffer                  m_TransformCB[App::FrameCount];      //!< 変換用バッファです.
    ConstantBuffer                  m_MeshCB[App::FrameCount];           //!< メッシュ用バッファです.

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
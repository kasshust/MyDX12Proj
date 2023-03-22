﻿//-----------------------------------------------------------------------------
// File : SampleApp.h
// Desc : Sample Application.
// Copyright(c) Pocol. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <App.h>
#include <Camera.h>
#include <ConstantBuffer.h>
#include <Material.h>
#include <SphereMapConverter.h>
#include <IBLBaker.h>
#include <SkyBox.h>
#include <Camera.h>
#include <RootSignature.h>


///////////////////////////////////////////////////////////////////////////////
// SampleApp class
///////////////////////////////////////////////////////////////////////////////
class SampleApp : public App
{
    //=========================================================================
    // list of friend classes and methods.
    //=========================================================================
    /* NOTHING */

public:
    //=========================================================================
    // public variables.
    //=========================================================================
    /* NOTHING */

    //=========================================================================
    // public methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      コンストラクタです.
    //!
    //! @param[in]      width       ウィンドウの横幅です.
    //! @param[in]      height      ウィンドウの縦幅です.
    //-------------------------------------------------------------------------
    SampleApp(uint32_t width, uint32_t height);

    //-------------------------------------------------------------------------
    //! @brief      デストラクタです.
    //-------------------------------------------------------------------------
    virtual ~SampleApp();

    

private:
    //=========================================================================
    // private variables.
    //=========================================================================
    ComPtr<ID3D12PipelineState>     m_pScenePSO;                    //!< シーン用パイプラインステートです.
    RootSignature                   m_SceneRootSig;                 //!< シーン用ルートシグニチャです.
    ComPtr<ID3D12PipelineState>     m_pTonemapPSO;                  //!< トーンマップ用パイプラインステートです.
    RootSignature                   m_TonemapRootSig;               //!< トーンマップ用ルートシグニチャです.
    ColorTarget                     m_SceneColorTarget;             //!< シーン用レンダーターゲットです.
    DepthTarget                     m_SceneDepthTarget;             //!< シーン用深度ターゲットです.


    VertexBuffer                    m_QuadVB;                       //!< 頂点バッファです.
    ConstantBuffer                  m_TonemapCB[FrameCount];        //!< 定数バッファです.
    ConstantBuffer                  m_LightCB[FrameCount];          //!< ライトバッファです.
    ConstantBuffer                  m_CameraCB[FrameCount];         //!< カメラバッファです.
    ConstantBuffer                  m_TransformCB[FrameCount];      //!< 変換用バッファです.
    ConstantBuffer                  m_MeshCB[FrameCount];           //!< メッシュ用バッファです.

    //　オブジェクト依存にしたい
    std::vector<Mesh*>              m_pMesh;                        //!< メッシュです.
    Material                        m_Material;                     //!< マテリアルです.


    float                           m_RotateAngle;                  //!< ライトの回転角です.
    int                             m_TonemapType;                  //!< トーンマップタイプ.
    int                             m_ColorSpace;                   //!< 出力色空間
    float                           m_BaseLuminance;                //!< 基準輝度値.
    float                           m_MaxLuminance;                 //!< 最大輝度値.
    float                           m_Exposure;                     //!< 露光値.


    Texture                         m_SphereMap;                    //!< スフィアマップです.
    SphereMapConverter              m_SphereMapConverter;           //!< スフィアマップコンバータ.
    IBLBaker                        m_IBLBaker;                     //!< IBLベイク.
    SkyBox                          m_SkyBox;                       //!< スカイボックスです.
    DirectX::SimpleMath::Matrix     m_View;                         //!< ビュー行列.
    DirectX::SimpleMath::Matrix     m_Proj;                         //!< 射影行列.

    Camera                          m_Camera;                       //!< カメラ.
    int                             m_PrevCursorX;                  //!< 前回のカーソル位置X.
    int                             m_PrevCursorY;                  //!< 前回のカーソル位置Y.

    //=========================================================================
    // private methods.
    //=========================================================================

    //-------------------------------------------------------------------------
    //! @brief      初期化時の処理です.
    //!
    //! @retval true    初期化に成功.
    //! @retval false   初期化に失敗.
    //-------------------------------------------------------------------------
    bool OnInit() override;

    bool PrepareMesh();
    bool CreateLightBuffer();
    bool CreateCameraBuffer();
    
    bool CreateColorTarget();
    bool CreateDepthTarget();
    
    bool CreateSceneRootSig();
    bool CreateScenePipe();

    bool CreateToneMapConstantBuffer();
    bool CreateToneMapRootSig();
    bool CreateToneMapPipe();
    
    bool CreateVertexBuffer();

    bool CreateMatrixConstantBuffer();
    bool CreateMeshBuffer();
    bool CreateIBLBaker();
    bool LoadTexture();
    bool CreateSphereMapConverter();
    bool CreateSkyBox();
    bool IBLBake();

    //-------------------------------------------------------------------------
    //! @brief      終了時の処理です.
    //-------------------------------------------------------------------------
    void OnTerm() override;

    void OnRenderIMGUI() override;



    //-------------------------------------------------------------------------
    //! @brief      描画時の処理です.
    //-------------------------------------------------------------------------
    void OnRender() override;

    void UpdateCamera();



    //-------------------------------------------------------------------------
    //! @brief      メッセージプロシージャです.
    //-------------------------------------------------------------------------
    void OnMsgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) override;

    //-------------------------------------------------------------------------
    //! @brief      ディスプレイモードを変更します.
    //!
    //! @param[in]      hdr     trueであればHDRディスプレイ用の設定に変更します.
    //-------------------------------------------------------------------------
    void ChangeDisplayMode(bool hdr);

    //-------------------------------------------------------------------------
    //! @brief      シーンを描画します.
    //-------------------------------------------------------------------------
    void DrawScene(ID3D12GraphicsCommandList* pCmdList);

    void UpdateLightBuffer();
    void UpdateCameraBuffer();
    void UpdateViewProjMatrix();
    void UpdateWorldMatrix();

    //-------------------------------------------------------------------------
    //! @brief      トーンマップを適用します.
    //-------------------------------------------------------------------------
    void DrawTonemap(ID3D12GraphicsCommandList* pCmdList);

    //-------------------------------------------------------------------------
    //! @brief      メッシュを描画します.
    //-------------------------------------------------------------------------
    void DrawMesh(ID3D12GraphicsCommandList* pCmdList);


};

//-----------------------------------------------------------------------------
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
#include <ToneMap.h>
#include <CommonBufferManager.h>
#include <CommonRTVManager.h>

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
    
    CommonRTManager                 m_CommonRTManager;
    CommonBufferManager             m_CommonBufferManager;

    float                           m_RotateAngle;                  //!< ライトの回転角です.
    float                           m_Exposure;                     //!< 露光値.
    
    DirectX::SimpleMath::Matrix     m_View;                         //!< ビュー行列.
    DirectX::SimpleMath::Matrix     m_Proj;                         //!< 射影行列.
    Camera                          m_Camera;                       //!< カメラ.
    int                             m_PrevCursorX;                  //!< 前回のカーソル位置X.
    int                             m_PrevCursorY;                  //!< 前回のカーソル位置Y.

    SkyBox                          m_SkyBox;                       //!< スカイボックスです.
    ToneMap                         m_ToneMap;                      //!< トーンマップです.
    
                                                                    // 以下未整理
    //　オブジェクト依存にしたい
    std::vector<Mesh*>              m_pMesh;                        //!< メッシュです.
    Material                        m_Material;                     //!< マテリアルです.

    //  スフィアマップとIBLベイクがここにあることがおかしい
    Texture                         m_SphereMap;                    //!< スフィアマップです.
    SphereMapConverter              m_SphereMapConverter;           //!< スフィアマップコンバータ.
    IBLBaker                        m_IBLBaker;                     //!< IBLベイク.

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
    bool CreateSceneRootSig();
    bool CreateScenePipeLineState();
    bool LoadSphereMapTexture();
    bool CreateSphereMapConverter();
    bool CreateSkyBox();
    bool IBLBake();

    //-------------------------------------------------------------------------
    //! @brief      終了時の処理です.
    //-------------------------------------------------------------------------
    void OnTerm() override;

    //-------------------------------------------------------------------------
    //! @brief      描画時の処理です.
    //-------------------------------------------------------------------------
    void OnRender() override;
    void OpaqueProcess(ID3D12GraphicsCommandList* pCmd, ColorTarget& colorSource, DepthTarget& depthSource, SkyBox* skyBox);
    void OnRenderIMGUI() override;
    void PostProcess(ID3D12GraphicsCommandList* pCmd);
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

    //-------------------------------------------------------------------------
    //! @brief      メッシュを描画します.
    //-------------------------------------------------------------------------
    void DrawMesh(ID3D12GraphicsCommandList* pCmdList);


};

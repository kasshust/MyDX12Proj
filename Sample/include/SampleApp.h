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
#include <Camera.h>
#include <RootSignature.h>
#include <ToneMap.h>
#include <CommonBufferManager.h>
#include <CommonRTVManager.h>
#include <Shaders.h>
#include <SkyTextureManager.h>
#include <ModelLoader.h>

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
	CommonRTManager                 m_CommonRTManager;
	CommonBufferManager             m_CommonBufferManager;
	SkyTextureManager               m_SkyTextureManager;
	ToneMap                         m_ToneMap;
	BasicShader                     m_BasicShader;
	Camera                          m_Camera;
	ModelLoader						m_ModelLoader;

	float                           m_Exposure;                     //!< 露光値.

	DirectX::SimpleMath::Matrix     m_View;                         //!< ビュー行列.
	DirectX::SimpleMath::Matrix     m_Proj;                         //!< 射影行列.

	int                             m_PrevCursorX;                  //!< 前回のカーソル位置X.
	int                             m_PrevCursorY;                  //!< 前回のカーソル位置Y.

	ConstantBuffer                  m_MeshCB[2];

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

	//-------------------------------------------------------------------------
	//! @brief      終了時の処理です.
	//-------------------------------------------------------------------------
	void OnTerm() override;

	//-------------------------------------------------------------------------
	//! @brief      描画時の処理です.
	//-------------------------------------------------------------------------
	void OnRender() override;
	void OnRenderIMGUI() override;

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
	void RenderOpaque(ID3D12GraphicsCommandList* pCmd, ColorTarget& colorSource, DepthTarget& depthSource, SkyBox* skyBox);
	void RenderPostProcess(ID3D12GraphicsCommandList* pCmd);
	void UpdateCamera();
	//-------------------------------------------------------------------------
	//! @brief      モデルを描画します.
	//-------------------------------------------------------------------------
	void DrawModel(ID3D12GraphicsCommandList* pCmd, ModelLoader& loader, Shader& shader);
};

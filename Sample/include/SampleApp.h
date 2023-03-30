//-----------------------------------------------------------------------------
// File : SampleApp.h
// Desc : Sample Application.
// Copyright(c) Pocol. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <App.h>
#include <GameObject.h>
#include <Camera.h>
#include <ConstantBuffer.h>
#include <Material.h>
#include <SphereMapConverter.h>
#include <Camera.h>
#include <RootSignature.h>
#include <CommonBufferManager.h>
#include <CommonRTVManager.h>
#include <Shaders.h>
#include <SkyTextureManager.h>
#include <ModelLoader.h>

#include <ToneMap.h>
#include <ShadowMap.h>

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
	
	std::vector<GameObject*>		m_GameObjects;
	CommonRTManager                 m_CommonRTManager;
	CommonBufferManager             m_CommonBufferManager;
	SkyManager						m_SkyManager;

	ShadowMap						m_ShadowMap;
	ToneMap                         m_ToneMap;
	Camera                          m_Camera;

	float                           m_Exposure;                     //!< 露光値.

	DirectX::SimpleMath::Matrix     m_View;                         //!< ビュー行列.
	DirectX::SimpleMath::Matrix     m_Proj;                         //!< 射影行列.

	int                             m_PrevCursorX;                  //!< 前回のカーソル位置X.
	int                             m_PrevCursorY;                  //!< 前回のカーソル位置Y.

	Vector3							m_LightDirection = Vector3(1.0f,1.0f,0.0f);
	float							m_LightIntensity = 1.0f;

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
	bool CommonInit();

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
	void RenderShadowMap(ID3D12GraphicsCommandList* pCmd, DepthTarget& depthDest);
	void RenderOpaque(ID3D12GraphicsCommandList* pCmd, ColorTarget& colorSource, DepthTarget& depthSource, SkyManager& manager);
	void RenderPostProcess(ID3D12GraphicsCommandList* pCmd);
	void RenderImGui(ID3D12GraphicsCommandList* pCmd);
	void UpdateCamera();
	//-------------------------------------------------------------------------
	//! @brief      モデルを描画します.
	//-------------------------------------------------------------------------
	void DrawModel(ID3D12GraphicsCommandList* pCmd, Model& loader, ModelShader& shader);
};

﻿//-----------------------------------------------------------------------------
// File : SampleApp.cpp
// Desc : Sample Application Module.
// Copyright(c) Pocol. All right reserved.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "SampleApp.h"
#include "FileUtil.h"
#include "Logger.h"
#include "CommonStates.h"
#include "DirectXHelpers.h"
#include "SimpleMath.h"
#include "CommonBufferManager.h"

//-----------------------------------------------------------------------------
// Using Statements
//-----------------------------------------------------------------------------
using namespace DirectX::SimpleMath;

///////////////////////////////////////////////////////////////////////////////
// SampleApp class
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//      コンストラクタです.
//-----------------------------------------------------------------------------
SampleApp::SampleApp(uint32_t width, uint32_t height)
	: App(width, height, DXGI_FORMAT_R10G10B10A2_UNORM)
	, m_Exposure(1.0f)
	, m_PrevCursorX(0)
	, m_PrevCursorY(0)
{ /* DO_NOTHING */
}

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
SampleApp::~SampleApp()
{ /* DO_NOTHING */
}

//-----------------------------------------------------------------------------
//      初期化時の処理です.
//-----------------------------------------------------------------------------
bool SampleApp::OnInit()
{
	// テクスチャ/メッシュをロード.
	if (!m_ModelLoader.LoadModel(L"../res/matball/matball.obj", m_pDevice, m_pPool[POOL_TYPE_RES], m_pQueue)) return false;

	// 共通定数バッファ/レンダーターゲット
	if (!m_CommonBufferManager.Init(m_pDevice.Get(), m_pPool[POOL_TYPE_RES], m_Width, m_Height))                                                return false;
	if (!m_CommonRTManager.Init(m_pDevice.Get(), m_pPool[POOL_TYPE_RTV], m_pPool[POOL_TYPE_RES], m_pPool[POOL_TYPE_DSV], m_Width, m_Height))    return false;

	// メッシュ用シェーダー初期化.
	if (!m_BasicShader.Init(m_pDevice,
		m_CommonRTManager.m_SceneColorTarget.GetRTVDesc().Format,
		m_CommonRTManager.m_SceneDepthTarget.GetDSVDesc().Format))      return false;

	// スカイテクスチャーの初期化/IBLのベイク
	if (!m_SkyTextureManager.Init(m_pDevice, m_pPool[POOL_TYPE_RTV], m_pPool[POOL_TYPE_RES], m_pQueue)) return false;
	if (!m_SkyTextureManager.IBLBake(m_pDevice, m_pPool[POOL_TYPE_RTV], m_pPool[POOL_TYPE_RES], m_CommandList, m_pQueue, m_Fence)) return false;

	// ポストプロセス初期化
	if (!m_ToneMap.Init(m_pDevice, m_pPool[POOL_TYPE_RES], m_ColorTarget[0].GetRTVDesc().Format, m_DepthTarget.GetDSVDesc().Format))    return false;

	for (auto i = 0; i < 2; ++i)
	{
		if (!m_MeshCB[i].Init(m_pDevice.Get(), m_pPool[POOL_TYPE_RES], sizeof(CommonCb::CbMesh)))
		{
			ELOG("Error : ConstantBuffer::Init() Failed.");
			return false;
		}

		auto ptr = m_MeshCB[i].GetPtr<CommonCb::CbMesh>();
		ptr->World = Matrix::Identity;
	}

	return true;
}

//-----------------------------------------------------------------------------
//      終了時の処理です.
//-----------------------------------------------------------------------------
void SampleApp::OnTerm()
{
	// メッシュ破棄.
	m_ModelLoader.Release();
	m_BasicShader.Term();

	m_ToneMap.Term();

	m_CommonBufferManager.Term();
	m_CommonRTManager.Term();
	m_SkyTextureManager.Term();
}

void SampleApp::OnRenderIMGUI() {
	ImGui::Begin("Setting");

	// Camera Rest
	if (ImGui::Button("Camera Reset")) {
		m_Camera.Reset();
	}

	// HDR/SDRの選択
	if (ImGui::Button("HDR")) {
		ChangeDisplayMode(true);
	}
	ImGui::SameLine();
	if (ImGui::Button("SDR")) {
		ChangeDisplayMode(false);
	}

	if (ImGui::TreeNode("PostProcess")) {
		if (ImGui::TreeNode("ToneMap")) {
			ImGui::RadioButton("NONE", &m_ToneMap.m_TonemapType, ToneMap::TONEMAP_NONE);
			ImGui::RadioButton("REINHARD", &m_ToneMap.m_TonemapType, ToneMap::TONEMAP_REINHARD);
			ImGui::RadioButton("GT", &m_ToneMap.m_TonemapType, ToneMap::TONEMAP_GT);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	if (ImGui::Button("Close Window")) {
		PostQuitMessage(0);
	}

	ImGui::End();
}

//-----------------------------------------------------------------------------
//      描画時の処理です.
//-----------------------------------------------------------------------------
void SampleApp::OnRender()
{
	// カメラ更新.
	UpdateCamera();


	// Normal Draw
	auto pCmd = m_CommandList.Reset();
	ID3D12DescriptorHeap* const pHeaps[] = {
		m_pPool[POOL_TYPE_RES]->GetHeap(),
	};
	pCmd->SetDescriptorHeaps(1, pHeaps);
	{
		RenderOpaque(pCmd, m_CommonRTManager.m_SceneColorTarget, m_CommonRTManager.m_SceneDepthTarget, m_SkyTextureManager.GetSkyBox());
		RenderPostProcess(pCmd);	
	}
	pCmd->Close();

	// ImGui Draw
	OnRenderIMGUICommonProcess();

	auto pImGuiCmd = m_ImGuiCommandList.Reset();
	ID3D12DescriptorHeap* const pImGuiHeaps[] = {
		m_ImGuiDescriptorHeap,
	};

	auto handleRTV = m_ColorTarget[m_FrameIndex].GetHandleRTV();
	pImGuiCmd->OMSetRenderTargets(1, &handleRTV->HandleCPU, FALSE, NULL);

	pImGuiCmd->SetDescriptorHeaps(1, pImGuiHeaps);
	{
		DirectX::TransitionResource(pImGuiCmd,
			m_ColorTarget[m_FrameIndex].GetResource(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET);

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pImGuiCmd);
		
		DirectX::TransitionResource(pImGuiCmd,
			m_ColorTarget[m_FrameIndex].GetResource(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
	}
	pImGuiCmd->Close();



	// コマンドリストを実行.
	ID3D12CommandList* pLists[] = { pCmd ,pImGuiCmd};
	m_pQueue->ExecuteCommandLists(2, pLists);

	// 画面に表示.
	Present(1);
}

void SampleApp::UpdateCamera() {
	auto fovY = DirectX::XMConvertToRadians(37.5f);
	auto aspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);

	m_View = m_Camera.GetView();
	m_Proj = Matrix::CreatePerspectiveFieldOfView(fovY, aspect, 0.1f, 1000.0f);
}

void SampleApp::RenderOpaque(ID3D12GraphicsCommandList* pCmd, ColorTarget& colorDest, DepthTarget& depthDest, SkyBox* skyBox = nullptr)
{
	// 書き込み用リソースバリア設定.
	DirectX::TransitionResource(pCmd,
		colorDest.GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	// ディスクリプタ取得.
	auto handleRTV = colorDest.GetHandleRTV();
	auto handleDSV = depthDest.GetHandleDSV();

	// レンダーターゲットを設定.
	pCmd->OMSetRenderTargets(1, &handleRTV->HandleCPU, FALSE, &handleDSV->HandleCPU);

	// レンダーターゲットをクリア.
	colorDest.ClearView(pCmd);
	depthDest.ClearView(pCmd);

	// ビューポート設定.
	pCmd->RSSetViewports(1, &m_Viewport);
	pCmd->RSSetScissorRects(1, &m_Scissor);

	// 背景描画.
	if (skyBox != nullptr) skyBox->Draw(pCmd, m_SkyTextureManager.GetCubeMapHandleGPU(), m_View, m_Proj, 300.0f);

	// シーンの描画.
	DrawScene(pCmd);

	// 読み込み用リソースバリア設定.
	DirectX::TransitionResource(pCmd,
		colorDest.GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

//-----------------------------------------------------------------------------
//      シーンを描画します.
//-----------------------------------------------------------------------------
void SampleApp::DrawScene(ID3D12GraphicsCommandList* pCmd)
{
	// 定数バッファの更新.
	m_CommonBufferManager.UpdateLightBuffer(m_FrameIndex, m_SkyTextureManager.m_IBLBaker.LDTextureSize, m_SkyTextureManager.m_IBLBaker.MipCount);
	m_CommonBufferManager.UpdateCameraBuffer(m_FrameIndex, m_Camera.GetPosition());
	m_CommonBufferManager.UpdateViewProjMatrix(m_FrameIndex, m_View, m_Proj);
	m_ModelLoader.UpdateWorldMatrix(m_FrameIndex);

	DrawModel(pCmd, m_ModelLoader, m_BasicShader);
}

//-----------------------------------------------------------------------------
//      メッシュを描画します.
//-----------------------------------------------------------------------------
void SampleApp::DrawModel(ID3D12GraphicsCommandList* pCmd, ModelLoader& loader, Shader& shader)
{
	const std::vector<Mesh*> meshs = loader.GetMeshes();
	const Material& mat = loader.GetMaterial();

	for (size_t i = 0; i < meshs.size(); ++i)
	{
		// マテリアルIDを取得.
		auto id = meshs[i]->GetMaterialId();

		// 使用するShaderをセット
		// shader.SetShader(pCmd, m_FrameIndex, mat, i, m_CommonBufferManager, m_SkyTextureManager.m_IBLBaker, m_MeshCB[m_FrameIndex]);
		shader.SetShader(pCmd, m_FrameIndex, mat, i, m_CommonBufferManager, m_SkyTextureManager.m_IBLBaker, m_ModelLoader);

		// メッシュを描画.
		meshs[i]->Draw(pCmd);
	}
}

//-----------------------------------------------------------------------------
//      ポストプロセス
//-----------------------------------------------------------------------------
void SampleApp::RenderPostProcess(ID3D12GraphicsCommandList* pCmd)
{
	m_ToneMap.DrawTonemap(pCmd,
		m_FrameIndex,
		m_ColorTarget[m_FrameIndex],
		m_DepthTarget,
		m_CommonRTManager.m_SceneColorTarget,
		&m_Viewport,
		&m_Scissor,
		m_CommonBufferManager.m_QuadVB);
}

//-----------------------------------------------------------------------------
//      ディスプレイモードを変更します.
//-----------------------------------------------------------------------------
void SampleApp::ChangeDisplayMode(bool hdr)
{
	if (hdr)
	{
		if (!IsSupportHDR())
		{
			MessageBox(
				nullptr,
				TEXT("HDRがサポートされていないディスプレイです."),
				TEXT("HDR非サポート"),
				MB_OK | MB_ICONINFORMATION);
			ELOG("Error : Display not support HDR.");
			return;
		}

		auto hr = m_pSwapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
		if (FAILED(hr))
		{
			MessageBox(
				nullptr,
				TEXT("ITU-R BT.2100 PQ Systemの色域設定に失敗しました"),
				TEXT("色域設定失敗"),
				MB_OK | MB_ICONERROR);
			ELOG("Error : IDXGISwapChain::SetColorSpace1() Failed.");
			return;
		}

		DXGI_HDR_METADATA_HDR10 metaData = {};

		// ITU-R BT.2100の原刺激と白色点を設定.
		metaData.RedPrimary[0] = ToneMap::GetChromaticityCoord(0.708);
		metaData.RedPrimary[1] = ToneMap::GetChromaticityCoord(0.292);
		metaData.BluePrimary[0] = ToneMap::GetChromaticityCoord(0.170);
		metaData.BluePrimary[1] = ToneMap::GetChromaticityCoord(0.797);
		metaData.GreenPrimary[0] = ToneMap::GetChromaticityCoord(0.131);
		metaData.GreenPrimary[1] = ToneMap::GetChromaticityCoord(0.046);
		metaData.WhitePoint[0] = ToneMap::GetChromaticityCoord(0.3127);
		metaData.WhitePoint[1] = ToneMap::GetChromaticityCoord(0.3290);

		// ディスプレイがサポートすると最大輝度値と最小輝度値を設定.
		metaData.MaxMasteringLuminance = UINT(GetMaxLuminance() * 10000);
		metaData.MinMasteringLuminance = UINT(GetMinLuminance() * 0.001);

		// 最大値を 2000 [nit]に設定.
		metaData.MaxContentLightLevel = 2000;

		hr = m_pSwapChain->SetHDRMetaData(DXGI_HDR_METADATA_TYPE_HDR10, sizeof(DXGI_HDR_METADATA_HDR10), &metaData);
		if (FAILED(hr))
		{
			ELOG("Error : IDXGISwapChain::SetHDRMetaData() Failed.");
		}

		m_ToneMap.SetLuminance(100.0f, GetMaxLuminance());

		// 成功したことを知らせるダイアログを出す.
		std::string message;
		message += "HDRディスプレイ用に設定を変更しました\n\n";
		message += "色空間：ITU-R BT.2100 PQ\n";
		message += "最大輝度値：";
		message += std::to_string(GetMaxLuminance());
		message += " [nit]\n";
		message += "最小輝度値：";
		message += std::to_string(GetMinLuminance());
		message += " [nit]\n";

		MessageBoxA(nullptr,
			message.c_str(),
			"HDR設定成功",
			MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		auto hr = m_pSwapChain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709);
		if (FAILED(hr))
		{
			MessageBox(
				nullptr,
				TEXT("ITU-R BT.709の色域設定に失敗しました"),
				TEXT("色域設定失敗"),
				MB_OK | MB_ICONERROR);
			ELOG("Error : IDXGISwapChain::SetColorSpace1() Failed.");
			return;
		}

		DXGI_HDR_METADATA_HDR10 metaData = {};

		// ITU-R BT.709の原刺激と白色点を設定.
		metaData.RedPrimary[0] = ToneMap::GetChromaticityCoord(0.640);
		metaData.RedPrimary[1] = ToneMap::GetChromaticityCoord(0.330);
		metaData.BluePrimary[0] = ToneMap::GetChromaticityCoord(0.300);
		metaData.BluePrimary[1] = ToneMap::GetChromaticityCoord(0.600);
		metaData.GreenPrimary[0] = ToneMap::GetChromaticityCoord(0.150);
		metaData.GreenPrimary[1] = ToneMap::GetChromaticityCoord(0.060);
		metaData.WhitePoint[0] = ToneMap::GetChromaticityCoord(0.3127);
		metaData.WhitePoint[1] = ToneMap::GetChromaticityCoord(0.3290);

		// ディスプレイがサポートすると最大輝度値と最小輝度値を設定.
		metaData.MaxMasteringLuminance = UINT(GetMaxLuminance() * 10000);
		metaData.MinMasteringLuminance = UINT(GetMinLuminance() * 0.001);

		// 最大値を 100[nit] に設定.
		metaData.MaxContentLightLevel = 100;

		hr = m_pSwapChain->SetHDRMetaData(DXGI_HDR_METADATA_TYPE_HDR10, sizeof(DXGI_HDR_METADATA_HDR10), &metaData);
		if (FAILED(hr))
		{
			ELOG("Error : IDXGISwapChain::SetHDRMetaData() Failed.");
		}

		m_ToneMap.SetLuminance(100.0f, GetMaxLuminance());

		// 成功したことを知らせるダイアログを出す.
		std::string message;
		message += "SDRディスプレイ用に設定を変更しました\n\n";
		message += "色空間：ITU-R BT.709\n";
		message += "最大輝度値：";
		message += std::to_string(GetMaxLuminance());
		message += " [nit]\n";
		message += "最小輝度値：";
		message += std::to_string(GetMinLuminance());
		message += " [nit]\n";
		MessageBoxA(nullptr,
			message.c_str(),
			"SDR設定成功",
			MB_OK | MB_ICONINFORMATION);
	}
}

//-----------------------------------------------------------------------------
//      メッセージプロシージャです.
//-----------------------------------------------------------------------------
void SampleApp::OnMsgProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// 古いWM_MOUSEWHEELの定義.
	const UINT OLD_WM_MOUSEWHEEL = 0x020A;

	if ((msg == WM_LBUTTONDOWN)
		|| (msg == WM_LBUTTONUP)
		|| (msg == WM_LBUTTONDBLCLK)
		|| (msg == WM_MBUTTONDOWN)
		|| (msg == WM_MBUTTONUP)
		|| (msg == WM_MBUTTONDBLCLK)
		|| (msg == WM_RBUTTONDOWN)
		|| (msg == WM_RBUTTONUP)
		|| (msg == WM_RBUTTONDBLCLK)
		|| (msg == WM_XBUTTONDOWN)
		|| (msg == WM_XBUTTONUP)
		|| (msg == WM_XBUTTONDBLCLK)
		|| (msg == WM_MOUSEHWHEEL)             // このWM_MOUSEWHEELは0x020Eを想定.
		|| (msg == WM_MOUSEMOVE)
		|| (msg == OLD_WM_MOUSEWHEEL))
	{
		auto x = int(LOWORD(lp));
		auto y = int(HIWORD(lp));

		auto delta = 0;
		if (msg == WM_MOUSEHWHEEL || msg == OLD_WM_MOUSEWHEEL)
		{
			POINT pt;
			pt.x = x;
			pt.y = y;

			// クライアント座標系に変換.
			ScreenToClient(hWnd, &pt);
			x = pt.x;
			y = pt.y;

			delta += int(HIWORD(wp));
		}

		auto state = int(LOWORD(wp));
		auto left = ((state & MK_LBUTTON) != 0);
		auto right = ((state & MK_RBUTTON) != 0);
		auto middle = ((state & MK_MBUTTON) != 0);

		Camera::Event args = {};

		if (left)
		{
			args.Type = Camera::EventRotate;
			args.RotateH = DirectX::XMConvertToRadians(-0.5f * (x - m_PrevCursorX));
			args.RotateV = DirectX::XMConvertToRadians(0.5f * (y - m_PrevCursorY));
			m_Camera.UpdateByEvent(args);
		}
		else if (right)
		{
			args.Type = Camera::EventDolly;
			args.Dolly = DirectX::XMConvertToRadians(0.5f * (y - m_PrevCursorY));
			m_Camera.UpdateByEvent(args);
		}
		else if (middle)
		{
			args.Type = Camera::EventMove;
			if (GetAsyncKeyState(VK_MENU) != 0)
			{
				args.MoveX = DirectX::XMConvertToRadians(0.5f * (x - m_PrevCursorX));
				args.MoveZ = DirectX::XMConvertToRadians(0.5f * (y - m_PrevCursorY));
			}
			else
			{
				args.MoveX = DirectX::XMConvertToRadians(0.5f * (x - m_PrevCursorX));
				args.MoveY = DirectX::XMConvertToRadians(0.5f * (y - m_PrevCursorY));
			}
			m_Camera.UpdateByEvent(args);
		}

		m_PrevCursorX = x;
		m_PrevCursorY = y;
	}
}
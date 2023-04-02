//-----------------------------------------------------------------------------
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
#include <ResourceManager.h>
#include <algorithm>
#include <cmath>
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

	if (!CommonInit()) return false;

	if (!m_ShadowMap.Init(m_pDevice, m_pPool[POOL_TYPE_RES], m_ColorTarget[0].GetRTVDesc().Format, m_DepthTarget.GetDSVDesc().Format))	return false;
	if (!m_ToneMap.Init(m_pDevice, m_pPool[POOL_TYPE_RES], m_ColorTarget[0].GetRTVDesc().Format, m_DepthTarget.GetDSVDesc().Format))    return false;
	if (!m_SkyManager.IBLBake(m_pDevice, m_pPool[POOL_TYPE_RTV], m_pPool[POOL_TYPE_RES], m_CommandList, m_pQueue, m_Fence))				return false;

	

	// GameObject/Model
	AppResourceManager& manager = AppResourceManager::GetInstance();
	ModelShader* ptr                 = new BasicShader();
	ptr->Init(m_pDevice, m_CommonRTManager.m_SceneColorTarget.GetRTVDesc().Format, m_DepthTarget.GetDSVDesc().Format);
	manager.AddShader(L"basic", ptr);

	const std::vector<std::wstring> path= {
		L"../res/matball/matball.obj",
		L"../res/teapot/teapot.obj",
		L"../res/cube/cube.obj",
	};
	GameObject* g;
		
	// matball
	g = new GameObject();
	if (!g->m_Model.LoadModel(path[0] , m_pDevice, m_pPool[POOL_TYPE_RES], m_pQueue)) return false;
	m_GameObjects.push_back(g);
	
	// teapot
	g = new GameObject();
	if (!g->m_Model.LoadModel(path[1], m_pDevice, m_pPool[POOL_TYPE_RES], m_pQueue)) return false;
	g->Transform().SetPosition({ 1.0f,0.0f,0.0f });
	m_GameObjects.push_back(g);

	// cube
	g = new GameObject();
	if (!g->m_Model.LoadModel(path[2], m_pDevice, m_pPool[POOL_TYPE_RES], m_pQueue)) return false;
	g->Transform().SetPosition({0.0f,-0.4f,0.0f});
	g->Transform().SetScale({ 20.0f,0.1f,20.0f });
	m_GameObjects.push_back(g);


	return true;
}

bool SampleApp::CommonInit() {

	const std::wstring skyPath = L"../res/texture/hdr014.dds";
	
	if (!m_CommonBufferManager.Init(m_pDevice.Get(), m_pPool[POOL_TYPE_RES], m_Width, m_Height))                                                return false;
	if (!m_CommonRTManager.Init(m_pDevice.Get(), m_pPool[POOL_TYPE_RTV], m_pPool[POOL_TYPE_RES], m_pPool[POOL_TYPE_DSV], m_Width, m_Height))    return false;
	if (!m_SkyManager.Init(m_pDevice, m_pPool[POOL_TYPE_RTV], m_pPool[POOL_TYPE_RES], m_pQueue, skyPath)) return false;

	m_CommonBufferManager.SetRTManager(&m_CommonRTManager);
}

//-----------------------------------------------------------------------------
//      終了時の処理です.
//-----------------------------------------------------------------------------
void SampleApp::OnTerm()
{
	for (size_t i = 0; i < m_GameObjects.size(); i++) {
		GameObject* g = m_GameObjects[i];
		g->m_Model.Release();
	}
	m_GameObjects.clear();

	m_ToneMap.Term();
	m_ShadowMap.Term();
	m_CommonBufferManager.Term();
	m_CommonRTManager.Term();
	m_SkyManager.Term();
}

void SampleApp::OnRenderIMGUI() {
	ImGui::Begin("Setting");

	// HDR/SDRの選択
	if (ImGui::Button("HDR")) {
		ChangeDisplayMode(true);
	}
	ImGui::SameLine();
	if (ImGui::Button("SDR")) {
		ChangeDisplayMode(false);
	}

	ImGui::PushID("Camera");
	if (ImGui::TreeNode("Camera")) {

		Vector3 pos		= m_Camera.GetPosition();
		Vector3 target	= m_Camera.GetTarget();

		float* posArray = new float[]		{ pos.x, pos.y, pos.z};
		float* targetArray = new float[]	{ target.x, target.y, target.z};

		ImGui::InputFloat3("Camera Position", posArray);
		ImGui::InputFloat3("Target Position", targetArray);
	
		m_Camera.SetPosition(Vector3(posArray));
		m_Camera.SetTarget(Vector3(targetArray));

		if (ImGui::Button("Camera Reset")) {
			m_Camera.Reset();
		}
		ImGui::TreePop();
	}
	ImGui::PopID();

	if (ImGui::TreeNode("CommonBuffer")) {
		if (ImGui::TreeNode("Light")) {

			ImGui::InputFloat("LightIntensity",  &m_LightIntensity);

			float* dirArray = new float[] { m_LightDirection.x, m_LightDirection.y, m_LightDirection.z};
			ImGui::InputFloat3("LightDirection", dirArray);
			m_LightDirection = Vector3(dirArray);

			ImGui::InputFloat("ShadowBias",		&m_ShadowBias);
			ImGui::InputFloat("ShadowStrength", &m_ShadowStrength);
			ImGui::InputFloat("ShadowLightPosDistance", &m_ShadowLightPosDistance);

			float* orthoArray = new float[] { m_OrthoGraphParam.x, m_OrthoGraphParam.y, m_OrthoGraphParam.z, m_OrthoGraphParam.w};
			ImGui::InputFloat4("OrthoGraphParam", orthoArray);
			m_OrthoGraphParam = Vector4(orthoArray);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Fog")) {

			float* fogArray = new float[] { m_FogArea.x, m_FogArea.y };
			ImGui::InputFloat2("FogArea", fogArray);
			m_FogArea = Vector2(fogArray);

			float* fogColor = new float[] { m_FogColor.x, m_FogColor.y, m_FogColor.z};
			ImGui::InputFloat3("FogColor", fogColor);
			m_FogColor = Vector3(fogColor);


			ImGui::TreePop();
		}

		ImGui::TreePop();
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



	if (ImGui::TreeNode("GameObjects")) {


		for (size_t i = 0; i < m_GameObjects.size(); i++) {
			
			ImGui::PushID(i);
			GameObject* g = m_GameObjects[i];
			std::string s = "ID : " + std::to_string(g->GetId());

			if (ImGui::TreeNode(s.c_str())) {
				

				if (ImGui::TreeNode("Transform")) {
					Vector3		pos = g->Transform().GetPosition();
					Vector3		rot = g->Transform().GetYawPitchRoll();
					Vector3		scale = g->Transform().GetScale();

					Vector3 degrees = Vector3(DirectX::XMConvertToDegrees(rot.x), DirectX::XMConvertToDegrees(rot.y), DirectX::XMConvertToDegrees(rot.z));

					float* posArray = new float[] { pos.x, pos.y, pos.z };
					float* rotArray = new float[] { degrees.y, degrees.x, degrees.z};
					float* scaleArray = new float[] { scale.x, scale.y, scale.z };

					float* ZeroArray = new float[] { 0.0f, 0.0f, 0.0f};


					ImGui::InputFloat3("Position", posArray);
					ImGui::InputFloat3("Rotation", rotArray);
					ImGui::InputFloat3("Scale", scaleArray);
					ImGui::InputFloat3("Rotationzero", ZeroArray);

					rotArray[1] = std::min(std::max(rotArray[1], 0.f), 0.f);

					g->Transform().SetPosition(Vector3(posArray));
					g->Transform().SetRotation(Vector3(DirectX::XMConvertToRadians(rotArray[0]), DirectX::XMConvertToRadians(rotArray[1]), DirectX::XMConvertToRadians(rotArray[2])));
					g->Transform().SetScale(Vector3(scaleArray));
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Material")) {
					std::vector<Material*> mats = g->m_Model.GetMaterials();

					for (size_t i = 0; i < mats.size(); i++)
					{
						auto ptr = reinterpret_cast<CommonCb::CbMaterial*>(mats[i]->GetBufferPtr(0));
						
						float* p00 = new float[] { ptr->Param00.x, ptr->Param00.y, ptr->Param00.z, ptr->Param00.w };
						float* p01 = new float[] { ptr->Param01.x, ptr->Param01.y, ptr->Param01.z, ptr->Param01.w };
						float* p02 = new float[] { ptr->Param02.x, ptr->Param02.y, ptr->Param02.z, ptr->Param02.w };
						float* p03 = new float[] { ptr->Param03.x, ptr->Param03.y, ptr->Param03.z, ptr->Param03.w };
						float* p04 = new float[] { ptr->Param04.x, ptr->Param04.y, ptr->Param04.z, ptr->Param04.w };
						float* p05 = new float[] { ptr->Param05.x, ptr->Param05.y, ptr->Param05.z, ptr->Param05.w };
						float* p06 = new float[] { ptr->Param06.x, ptr->Param06.y, ptr->Param06.z, ptr->Param06.w };
						float* p07 = new float[] { ptr->Param07.x, ptr->Param07.y, ptr->Param07.z, ptr->Param07.w };
						float* p08 = new float[] { ptr->Param08.x, ptr->Param08.y, ptr->Param08.z, ptr->Param08.w };
						float* p09 = new float[] { ptr->Param09.x, ptr->Param09.y, ptr->Param09.z, ptr->Param09.w };
						
						ImGui::InputFloat4("Param00", p00);
						ImGui::InputFloat4("Param01", p01);
						ImGui::InputFloat4("Param02", p02);
						ImGui::InputFloat4("Param03", p03);
						ImGui::InputFloat4("Param04", p04);
						ImGui::InputFloat4("Param05", p05);
						ImGui::InputFloat4("Param06", p06);
						ImGui::InputFloat4("Param07", p07);
						ImGui::InputFloat4("Param08", p08);
						ImGui::InputFloat4("Param09", p09);

						ptr->Param00 = Vector4(p00);
						ptr->Param01 = Vector4(p01);
						ptr->Param02 = Vector4(p02);
						ptr->Param03 = Vector4(p03);
						ptr->Param04 = Vector4(p04);
						ptr->Param05 = Vector4(p05);
						ptr->Param06 = Vector4(p06);
						ptr->Param07 = Vector4(p07);
						ptr->Param08 = Vector4(p08);
						ptr->Param09 = Vector4(p09);
					}

					ImGui::TreePop();
				}

				
				ImGui::TreePop();
			}
			
			ImGui::PopID();

		}

		ImGui::TreePop();
	}

	if (ImGui::Button("Close Window")) {
		PostQuitMessage(0);
	}

	// ImGui::ShowMetricsWindow();

	ImGui::End();

	ImGui::Begin("Metrics");

	if (ImGui::TreeNode("Target")) {

		if (ImGui::TreeNode("ColorTarget")) {
			ImGui::Image((ImTextureID)m_CommonRTManager.m_SceneColorTarget.GetHandleSRV()->HandleGPU.ptr, ImVec2(160, 90));
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("DepthTarget")) {
			ImGui::Image((ImTextureID)m_CommonRTManager.m_SceneDepthTarget.GetHandleSRV()->HandleGPU.ptr, ImVec2(160, 90));
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("ShadowTarget")) {
			ImGui::Image((ImTextureID)m_CommonRTManager.m_SceneShadowTarget.GetHandleSRV()->HandleGPU.ptr, ImVec2(160, 90));
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Resource")) {
		if (ImGui::TreeNode("Texture")) {
			const auto m = AppResourceManager::GetInstance().GetTexturesMap();
			for (auto itr = m.begin(); itr != m.end(); ++itr) {
				ImGui::Text("%ls", itr->first.c_str());
				ImGui::Image((ImTextureID)itr->second->GetHandleGPU().ptr, ImVec2(64, 64));
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("ResMesh")) {
			const auto m = AppResourceManager::GetInstance().GetResMeshesMap();
			for (auto itr = m.begin(); itr != m.end(); ++itr) {
				ImGui::Text("%ls", itr->first.c_str());
			}
			ImGui::TreePop();
		}


		if (ImGui::TreeNode("ResMaterial")) {
			const auto m = AppResourceManager::GetInstance().GetResMaterialsMap();

			for (auto itr = m.begin(); itr != m.end(); ++itr) {
				ImGui::Text("%ls", itr->first.c_str());
			}
			ImGui::TreePop();
		}

		ImGui::TreePop();
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
	UpdateBuffer();

	// レンダリングエンジン描画
	auto pCmd = m_CommandList.Reset();
	if (pCmd == nullptr) { return; }

	ID3D12DescriptorHeap* const pHeaps[] = {
		m_pPool[POOL_TYPE_RES]->GetHeap(),
	};
	pCmd->SetDescriptorHeaps(1, pHeaps);
	{
		// ビューポート設定.
		pCmd->RSSetViewports(1,    &m_Viewport);
		pCmd->RSSetScissorRects(1, &m_Scissor);

		RenderShadowMap(pCmd, m_CommonRTManager.m_SceneShadowTarget);
		RenderOpaque(pCmd, m_CommonRTManager.m_SceneColorTarget, m_CommonRTManager.m_SceneDepthTarget, m_SkyManager);
		RenderPostProcess(pCmd);
	}
	pCmd->Close();



	// ImGui描画
	OnRenderIMGUICommonProcess();
	ID3D12DescriptorHeap* const pImGuiHeaps[] = {
		m_pPool[POOL_TYPE_RES]->GetHeap(),
		// m_pImGuiDescriptorHeap.Get(),
	};
	auto pImGuiCmd = m_ImGuiCommandList.Reset();
	pImGuiCmd->SetDescriptorHeaps(1, pImGuiHeaps);
	{
		RenderImGui(pImGuiCmd);
	}
	pImGuiCmd->Close();

	// コマンドリストを実行.
	ID3D12CommandList* pLists[] = { pCmd ,pImGuiCmd };
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

void SampleApp::UpdateBuffer() {
	CommonCb::CbCommon common;
	common.CameraPosition = m_Camera.GetPosition();
	common.FogArea        = m_FogArea;
	common.FogColor       = m_FogColor;

	// ライトシャドウバッファ
	CommonCb::CbLight cbl;
	cbl.TextureSize    = m_SkyManager.m_IBLBaker.LDTextureSize;
	cbl.MipCount       = m_SkyManager.m_IBLBaker.MipCount;
	cbl.LightDirection = m_LightDirection;
	cbl.LightIntensity = m_LightIntensity;
	cbl.ShadowBias     = m_ShadowBias;
	cbl.ShadowStrength = m_ShadowStrength;


	// ビュープロジェクションバッファ
	CommonCb::CbTransform cbt;
	cbt.Proj = m_Proj;
	cbt.View = m_View;

	m_CommonBufferManager.UpdateCommonBuffer(m_FrameIndex, common);
	m_CommonBufferManager.UpdateLightBuffer(m_FrameIndex, cbl);
	m_CommonBufferManager.UpdateShadowBuffer(m_FrameIndex, m_LightDirection, m_ShadowLightPosDistance, m_OrthoGraphParam);
	m_CommonBufferManager.UpdateViewProjMatrix(m_FrameIndex, cbt);
}

void SampleApp::RenderOpaque(ID3D12GraphicsCommandList* pCmd, ColorTarget& ColorDest, DepthTarget& DepthDest,SkyManager& skyManager)
{
	// 書き込み用リソースバリア設定.
	DirectX::TransitionResource(pCmd,
		ColorDest.GetResource(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	// ディスクリプタ取得.
	auto handleRTV = ColorDest.GetHandleRTV();
	auto handleDSV = DepthDest.GetHandleDSV();

	// レンダーターゲットを設定.
	pCmd->OMSetRenderTargets(1, &handleRTV->HandleCPU, FALSE, &handleDSV->HandleCPU);

	// レンダーターゲットをクリア.
	ColorDest.ClearView(pCmd);
	DepthDest.ClearView(pCmd);

	// 背景描画.
	SkyBox* ptr = skyManager.GetSkyBox();
	if (ptr != nullptr) ptr->Draw(pCmd, skyManager.GetCubeMapHandleGPU(), m_View, m_Proj, 300.0f);

	// シーンの描画.
	DrawScene(pCmd);

	// 読み込み用リソースバリア設定.
	DirectX::TransitionResource(pCmd,
		ColorDest.GetResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

//-----------------------------------------------------------------------------
//      シーンを描画します.
//-----------------------------------------------------------------------------
void SampleApp::DrawScene(ID3D12GraphicsCommandList* pCmd)
{

	// 非バッチ
	for (size_t i = 0; i < m_GameObjects.size(); i++) {
		GameObject* g = m_GameObjects[i];

		CommonCb::CbMesh cbm;
		cbm.World = g->Transform().GetTransform();

		g->m_Model.UpdateMeshBuffer(m_FrameIndex, cbm);
		g->m_Model.DrawModel(pCmd, m_FrameIndex, m_CommonBufferManager, m_SkyManager);
	}
}

//-----------------------------------------------------------------------------
//      プリ/ポストプロセス
//-----------------------------------------------------------------------------
void SampleApp::RenderShadowMap(ID3D12GraphicsCommandList* pCmd, DepthTarget& DepthDest) {
	ShadowMap::DrawSource s{
		DepthDest,
		m_CommonBufferManager,
		m_GameObjects,
		m_LightDirection
	};
	m_ShadowMap.DrawShadowMap(pCmd, m_FrameIndex, s);
}

void SampleApp::RenderPostProcess(ID3D12GraphicsCommandList* pCmd)
{
	ToneMap::DrawSource s{
		m_ColorTarget[m_FrameIndex],
		m_DepthTarget,
		m_CommonRTManager.m_SceneColorTarget,
		m_CommonBufferManager.m_QuadVB
	};
	m_ToneMap.DrawTonemap(pCmd, m_FrameIndex, s);
}

void SampleApp::RenderImGui(ID3D12GraphicsCommandList* pImGuiCmd)
{
	auto handleRTV = m_ColorTarget[m_FrameIndex].GetHandleRTV();
	pImGuiCmd->OMSetRenderTargets(1, &handleRTV->HandleCPU, FALSE, NULL);

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
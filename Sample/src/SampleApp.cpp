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
#include "SampleAppCb.h"

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
, m_Exposure        (1.0f)
, m_PrevCursorX(0)
, m_PrevCursorY(0)
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      デストラクタです.
//-----------------------------------------------------------------------------
SampleApp::~SampleApp()
{ /* DO_NOTHING */ }

//-----------------------------------------------------------------------------
//      初期化時の処理です.
//-----------------------------------------------------------------------------
bool SampleApp::OnInit()
{
    // テクスチャ/メッシュをロード.
    if (!PrepareMesh())              return false;

    // シーン用定数バッファ/レンダーターゲット
    if (!m_CommonBufferManager.CreateLightBuffer(m_pDevice.Get(), m_pPool[POOL_TYPE_RES]))                                              return false;
    if (!m_CommonBufferManager.CreateCameraBuffer(m_pDevice.Get(), m_pPool[POOL_TYPE_RES]))                                             return false;
    if (!m_CommonBufferManager.CreateVertexBuffer(m_pDevice.Get()))                                                                     return false;
    if (!m_CommonBufferManager.CreateMeshBuffer(m_pDevice.Get(), m_pPool[POOL_TYPE_RES]))                                               return false;
    if (!m_CommonBufferManager.CreateMatrixConstantBuffer(m_pDevice.Get(), m_pPool[POOL_TYPE_RES],m_Width, m_Height))                   return false;
    if (!m_CommonRTManager.CreateColorTarget(m_pDevice.Get(), m_pPool[POOL_TYPE_RTV], m_pPool[POOL_TYPE_RES], m_Width, m_Height))      return false;
    if (!m_CommonRTManager.CreateDepthTarget(m_pDevice.Get(), m_pPool[POOL_TYPE_DSV], m_Width, m_Height))                              return false;

    // シーン用ルートシグニチャ/パイプラインステートの生成.
    if (!CreateSceneRootSig())                  return false;
    if (!CreateScenePipeLineState())            return false;

    // スカイボックス
    if (!CreateSkyBox())                        return false;

    //依存しているのに関数が分かれている
    // スフィアマップ
    if (!LoadSphereMapTexture())                return false;
    if (!CreateSphereMapConverter())            return false;
    if (!IBLBake())                             return false;

    // ポストプロセス初期化
    if (!m_ToneMap.Init(m_pDevice, m_pPool[POOL_TYPE_RES], m_ColorTarget[0].GetRTVDesc().Format, m_DepthTarget.GetDSVDesc().Format))    return false;

    return true;
}


// オブジェクトに依存させたい
bool SampleApp::PrepareMesh() {
    std::wstring path;

    // ファイルパスを検索.
    if (!SearchFilePath(L"res/matball/matball.obj", path))
    {
        ELOG("Error : File Not Found.");
        return false;
    }

    std::wstring dir = GetDirectoryPath(path.c_str());

    std::vector<ResMesh>        resMesh;
    std::vector<ResMaterial>    resMaterial;

    // メッシュリソースをロード.
    if (!LoadMesh(path.c_str(), resMesh, resMaterial))
    {
        ELOG("Error : Load Mesh Failed. filepath = %ls", path.c_str());
        return false;
    }

    // メモリを予約.
    m_pMesh.reserve(resMesh.size());

    // メッシュを初期化.
    for (size_t i = 0; i < resMesh.size(); ++i)
    {
        // メッシュ生成.
        auto mesh = new (std::nothrow) Mesh();

        // チェック.
        if (mesh == nullptr)
        {
            ELOG("Error : Out of memory.");
            return false;
        }

        // 初期化処理.
        if (!mesh->Init(m_pDevice.Get(), resMesh[i]))
        {
            ELOG("Error : Mesh Initialize Failed.");
            delete mesh;
            return false;
        }

        // 成功したら登録.
        m_pMesh.push_back(mesh);
    }

    // メモリ最適化.
    m_pMesh.shrink_to_fit();

    // マテリアル初期化.
    if (!m_Material.Init(
        m_pDevice.Get(),
        m_pPool[POOL_TYPE_RES],
        sizeof(CommonCb::CbMaterial),
        resMaterial.size()))
    {
        ELOG("Error : Material::Init() Failed.");
        return false;
    }

    // リソースバッチを用意.
    DirectX::ResourceUploadBatch batch(m_pDevice.Get());

    // バッチ開始.
    batch.Begin();

    // テクスチャとマテリアルを設定.
    {
        /* ここではマテリアルが1個分かっているのでハードコーディングしています.*/
        m_Material.SetTexture(0, TU_BASE_COLOR, L"../res/texture/gold_bc.dds", batch);
        m_Material.SetTexture(0, TU_METALLIC, L"../res/texture/gold_m.dds", batch);
        m_Material.SetTexture(0, TU_ROUGHNESS, L"../res/texture/gold_r.dds", batch);
        m_Material.SetTexture(0, TU_NORMAL, L"../res/texture/gold_n.dds", batch);
    }

    // バッチ終了.
    auto future = batch.End(m_pQueue.Get());

    // バッチ完了を待機.
    future.wait();

    return true;
}

bool SampleApp::CreateSceneRootSig() {
    RootSignature::Desc desc;
    desc.Begin(11)
        .SetCBV(ShaderStage::VS, 0, 0)
        .SetCBV(ShaderStage::VS, 1, 1)
        .SetCBV(ShaderStage::PS, 2, 1)
        .SetCBV(ShaderStage::PS, 3, 2)
        .SetSRV(ShaderStage::PS, 4, 0)
        .SetSRV(ShaderStage::PS, 5, 1)
        .SetSRV(ShaderStage::PS, 6, 2)
        .SetSRV(ShaderStage::PS, 7, 3)
        .SetSRV(ShaderStage::PS, 8, 4)
        .SetSRV(ShaderStage::PS, 9, 5)
        .SetSRV(ShaderStage::PS, 10, 6)
        .AddStaticSmp(ShaderStage::PS, 0, SamplerState::LinearWrap)
        .AddStaticSmp(ShaderStage::PS, 1, SamplerState::LinearWrap)
        .AddStaticSmp(ShaderStage::PS, 2, SamplerState::LinearWrap)
        .AddStaticSmp(ShaderStage::PS, 3, SamplerState::LinearWrap)
        .AddStaticSmp(ShaderStage::PS, 4, SamplerState::LinearWrap)
        .AddStaticSmp(ShaderStage::PS, 5, SamplerState::LinearWrap)
        .AddStaticSmp(ShaderStage::PS, 6, SamplerState::LinearWrap)
        .AllowIL()
        .End();

    if (!m_SceneRootSig.Init(m_pDevice.Get(), desc.GetDesc()))
    {
        ELOG("Error : RootSignature::Init() Failed.");
        return false;
    }

    return true;
}
bool SampleApp::CreateScenePipeLineState() {
    std::wstring vsPath;
    std::wstring psPath;

    // 頂点シェーダを検索.
    if (!SearchFilePath(L"BasicVS.cso", vsPath))
    {
        ELOG("Error : Vertex Shader Not Found.");
        return false;
    }

    // ピクセルシェーダを検索.
    if (!SearchFilePath(L"BasicPS.cso", psPath))
    {
        ELOG("Error : Pixel Shader Node Found.");
        return false;
    }

    ComPtr<ID3DBlob> pVSBlob;
    ComPtr<ID3DBlob> pPSBlob;

    // 頂点シェーダを読み込む.
    auto hr = D3DReadFileToBlob(vsPath.c_str(), pVSBlob.GetAddressOf());
    if (FAILED(hr))
    {
        ELOG("Error : D3DReadFiledToBlob() Failed. path = %ls", vsPath.c_str());
        return false;
    }

    // ピクセルシェーダを読み込む.
    hr = D3DReadFileToBlob(psPath.c_str(), pPSBlob.GetAddressOf());
    if (FAILED(hr))
    {
        ELOG("Error : D3DReadFileToBlob() Failed. path = %ls", psPath.c_str());
        return false;
    }

    D3D12_INPUT_ELEMENT_DESC elements[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // グラフィックスパイプラインステートを設定.
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
    desc.InputLayout                        = { elements, 4 };
    desc.pRootSignature                     = m_SceneRootSig.GetPtr();
    desc.VS                                 = { pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize() };
    desc.PS                                 = { pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize() };
    desc.RasterizerState                    = DirectX::CommonStates::CullNone;
    desc.BlendState                         = DirectX::CommonStates::Opaque;
    desc.DepthStencilState                  = DirectX::CommonStates::DepthDefault;
    desc.SampleMask                         = UINT_MAX;
    desc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets                   = 1;
    desc.RTVFormats[0]                      = m_CommonRTManager.m_SceneColorTarget.GetRTVDesc().Format;
    desc.DSVFormat                          = m_CommonRTManager.m_SceneDepthTarget.GetDSVDesc().Format;
    desc.SampleDesc.Count                   = 1;
    desc.SampleDesc.Quality                 = 0;

    // パイプラインステートを生成.
    hr = m_pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pScenePSO.GetAddressOf()));
    if (FAILED(hr))
    {
        ELOG("Error : ID3D12Device::CreateGraphicsPipelineState() Failed. retcode = 0x%x", hr);
        return false;
    }
}

bool SampleApp::LoadSphereMapTexture() {
    DirectX::ResourceUploadBatch batch(m_pDevice.Get());

    // バッチ開始.
    batch.Begin();

    // スフィアマップ読み込み.
    {
        std::wstring sphereMapPath;
        if (!SearchFilePathW(L"../res/texture/hdr014.dds", sphereMapPath))
        {
            ELOG("Error : File Not Found.");
            return false;
        }

        // テクスチャ初期化.
        if (!m_SphereMap.Init(
            m_pDevice.Get(),
            m_pPool[POOL_TYPE_RES],
            sphereMapPath.c_str(),
            batch))
        {
            ELOG("Error : Texture::Init() Failed.");
            return false;
        }
    }

    // バッチ終了.
    auto future = batch.End(m_pQueue.Get());

    // 完了を待機.
    future.wait();
}
bool SampleApp::CreateSphereMapConverter() {
    if (!m_SphereMapConverter.Init(
        m_pDevice.Get(),
        m_pPool[POOL_TYPE_RTV],
        m_pPool[POOL_TYPE_RES],
        m_SphereMap.GetResource()->GetDesc()))
    {
        ELOG("Error : SphereMapConverter::Init() Failed.");
        return false;
    }
}
bool SampleApp::CreateSkyBox() {
    if (!m_SkyBox.Init(
        m_pDevice.Get(),
        m_pPool[POOL_TYPE_RES],
        DXGI_FORMAT_R10G10B10A2_UNORM,
        DXGI_FORMAT_D32_FLOAT))
    {
        ELOG("Error : SkyBox::Init() Failed.");
        return false;
    }
}
bool SampleApp::IBLBake() {

    if (!m_IBLBaker.Init(m_pDevice.Get(), m_pPool[POOL_TYPE_RES], m_pPool[POOL_TYPE_RTV]))
    {
        ELOG("Error : IBLBaker::Init() Failed.");
        return false;
    }

    // コマンドリストの記録を開始.
    auto pCmd = m_CommandList.Reset();

    ID3D12DescriptorHeap* const pHeaps[] = {
        m_pPool[POOL_TYPE_RES]->GetHeap(),
    };

    pCmd->SetDescriptorHeaps(1, pHeaps);

    // キューブマップに変換.
    m_SphereMapConverter.DrawToCube(pCmd, m_SphereMap.GetHandleGPU());

    auto desc = m_SphereMapConverter.GetCubeMapDesc();
    auto handle = m_SphereMapConverter.GetCubeMapHandleGPU();

    // DFG項を積分.
    m_IBLBaker.IntegrateDFG(pCmd);

    // LD項を積分.
    m_IBLBaker.IntegrateLD(pCmd, uint32_t(desc.Width), desc.MipLevels, handle);

    // コマンドリストの記録を終了.
    pCmd->Close();

    // コマンドリストを実行.
    ID3D12CommandList* pLists[] = { pCmd };
    m_pQueue->ExecuteCommandLists(1, pLists);

    // 完了を待機.
    m_Fence.Sync(m_pQueue.Get());

    return true;
}


//-----------------------------------------------------------------------------
//      終了時の処理です.
//-----------------------------------------------------------------------------
void SampleApp::OnTerm()
{
    m_ToneMap.Term();

    // メッシュ破棄.
    for (size_t i = 0; i<m_pMesh.size(); ++i)
    {
        SafeTerm(m_pMesh[i]);
    }
    m_pMesh.clear();
    m_pMesh.shrink_to_fit();
    m_Material.Term();

    m_CommonBufferManager.Term();
    m_CommonRTManager.Term();

    m_pScenePSO   .Reset();
    m_SceneRootSig.Term();

    m_IBLBaker.Term();
    m_SphereMapConverter.Term();
    m_SphereMap.Term();
    m_SkyBox.Term();
}

void SampleApp::OnRenderIMGUI() {
    ImGui::Begin("Setting");

    // Camera Rest
    if (ImGui::Button("Camera Reset")) {
        m_Camera.Reset();
    }

    // HDR SDR
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
    // IMGUI 初期設定
    OnRenderIMGUICommonProcess();

    // カメラ更新.
    UpdateCamera();

    // コマンドリストの記録を開始.
    auto pCmd = m_CommandList.Reset();

    ID3D12DescriptorHeap* const pHeaps[] = {
        m_pPool[POOL_TYPE_RES]->GetHeap(),
    };
    pCmd->SetDescriptorHeaps(1, pHeaps);

    //　生出力
    {
        // OpaqueProcess(pCmd, m_ColorTarget[m_FrameIndex], m_DepthTarget, &m_SkyBox);
    }

    {
        OpaqueProcess(pCmd, m_CommonRTManager.m_SceneColorTarget, m_CommonRTManager.m_SceneDepthTarget, &m_SkyBox);
        PostProcess(pCmd);  
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCmd);
    }

    // コマンドリストの記録を終了.
    pCmd->Close();

    // コマンドリストを実行.
    ID3D12CommandList* pLists[] = { pCmd };
    m_pQueue->ExecuteCommandLists( 1, pLists );

    // 画面に表示.
    Present(1);
}

void SampleApp::OpaqueProcess(ID3D12GraphicsCommandList* pCmd, ColorTarget& colorDest, DepthTarget& depthDest , SkyBox* skyBox = nullptr) 
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
    if (skyBox != nullptr) skyBox->Draw(pCmd, m_SphereMapConverter.GetCubeMapHandleGPU(), m_View, m_Proj, 300.0f);

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
    m_CommonBufferManager.UpdateLightBuffer(m_FrameIndex, m_IBLBaker.LDTextureSize, m_IBLBaker.MipCount);
    m_CommonBufferManager.UpdateCameraBuffer(m_FrameIndex, m_Camera.GetPosition());
    m_CommonBufferManager.UpdateViewProjMatrix(m_FrameIndex, m_View, m_Proj);
    m_CommonBufferManager.UpdateWorldMatrix(m_FrameIndex);

    pCmd->SetGraphicsRootSignature(m_SceneRootSig.GetPtr());
    pCmd->SetGraphicsRootDescriptorTable(0, m_CommonBufferManager.m_TransformCB[m_FrameIndex].GetHandleGPU());
    pCmd->SetGraphicsRootDescriptorTable(2, m_CommonBufferManager.m_LightCB[m_FrameIndex].GetHandleGPU());
    pCmd->SetGraphicsRootDescriptorTable(3, m_CommonBufferManager.m_CameraCB[m_FrameIndex].GetHandleGPU());
    pCmd->SetGraphicsRootDescriptorTable(4, m_IBLBaker.GetHandleGPU_DFG());
    pCmd->SetGraphicsRootDescriptorTable(5, m_IBLBaker.GetHandleGPU_DiffuseLD());
    pCmd->SetGraphicsRootDescriptorTable(6, m_IBLBaker.GetHandleGPU_SpecularLD());
    pCmd->SetPipelineState(m_pScenePSO.Get());

    // オブジェクトを描く.
    pCmd->SetGraphicsRootDescriptorTable(1, m_CommonBufferManager.m_MeshCB[m_FrameIndex].GetHandleGPU());
    DrawMesh(pCmd);
}

//-----------------------------------------------------------------------------
//      メッシュを描画します.
//-----------------------------------------------------------------------------
void SampleApp::DrawMesh(ID3D12GraphicsCommandList* pCmd)
{
    for (size_t i = 0; i < m_pMesh.size(); ++i)
    {
        // マテリアルIDを取得.
        auto id = m_pMesh[i]->GetMaterialId();

        // テクスチャを設定.
        pCmd->SetGraphicsRootDescriptorTable(7, m_Material.GetTextureHandle(id, TU_BASE_COLOR));
        pCmd->SetGraphicsRootDescriptorTable(8, m_Material.GetTextureHandle(id, TU_METALLIC));
        pCmd->SetGraphicsRootDescriptorTable(9, m_Material.GetTextureHandle(id, TU_ROUGHNESS));
        pCmd->SetGraphicsRootDescriptorTable(10, m_Material.GetTextureHandle(id, TU_NORMAL));

        // メッシュを描画.
        m_pMesh[i]->Draw(pCmd);
    }
}


void SampleApp::UpdateCamera() {
    auto fovY = DirectX::XMConvertToRadians(37.5f);
    auto aspect = static_cast<float>(m_Width) / static_cast<float>(m_Height);

    m_View = m_Camera.GetView();
    m_Proj = Matrix::CreatePerspectiveFieldOfView(fovY, aspect, 0.1f, 1000.0f);
}




//-----------------------------------------------------------------------------
//      ポストプロセス
//-----------------------------------------------------------------------------
void SampleApp::PostProcess(ID3D12GraphicsCommandList* pCmd)
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
        metaData.RedPrimary[0]      = ToneMap::GetChromaticityCoord(0.708);
        metaData.RedPrimary[1]      = ToneMap::GetChromaticityCoord(0.292);
        metaData.BluePrimary[0]     = ToneMap::GetChromaticityCoord(0.170);
        metaData.BluePrimary[1]     = ToneMap::GetChromaticityCoord(0.797);
        metaData.GreenPrimary[0]    = ToneMap::GetChromaticityCoord(0.131);
        metaData.GreenPrimary[1]    = ToneMap::GetChromaticityCoord(0.046);
        metaData.WhitePoint[0]      = ToneMap::GetChromaticityCoord(0.3127);
        metaData.WhitePoint[1]      = ToneMap::GetChromaticityCoord(0.3290);

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
        metaData.RedPrimary[0]      = ToneMap::GetChromaticityCoord(0.640);
        metaData.RedPrimary[1]      = ToneMap::GetChromaticityCoord(0.330);
        metaData.BluePrimary[0]     = ToneMap::GetChromaticityCoord(0.300);
        metaData.BluePrimary[1]     = ToneMap::GetChromaticityCoord(0.600);
        metaData.GreenPrimary[0]    = ToneMap::GetChromaticityCoord(0.150);
        metaData.GreenPrimary[1]    = ToneMap::GetChromaticityCoord(0.060);
        metaData.WhitePoint[0]      = ToneMap::GetChromaticityCoord(0.3127);
        metaData.WhitePoint[1]      = ToneMap::GetChromaticityCoord(0.3290);

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

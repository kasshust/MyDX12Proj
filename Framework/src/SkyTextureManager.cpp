#include "SkyTextureManager.h"
#include "CommonBufferManager.h"
#include "FileUtil.h"
#include "Logger.h"
#include "CommonStates.h"
#include "DirectXHelpers.h"
#include "SimpleMath.h"

bool SkyManager::Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvPool, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue, std::wstring path) {
	if (!InitSphereMapTexture(pDevice, resPool, commandQueue, path)) return false;
	if (!InitSphereMapConverter(pDevice, rtvPool, resPool)) return false;
	if (!InitSkyBox(pDevice, resPool)) return false;

	return true;
}

bool SkyManager::InitSphereMapTexture(ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue, std::wstring path)
{

	HRESULT result = pDevice->GetDeviceRemovedReason();
	

	DirectX::ResourceUploadBatch batch(pDevice.Get());

	// バッチ開始.
	batch.Begin();
	
	// スフィアマップ読み込み.
	{
		std::wstring sphereMapPath;
		if (!SearchFilePathW(path.c_str(), sphereMapPath))
		{
			ELOG("Error : File Not Found.");
			return false;
		}

		m_SkyTexturePath = path;

		// テクスチャ初期化.
		if (!m_SphereMap.Init(
			pDevice.Get(),
			resPool,
			sphereMapPath.c_str(),
			batch))
		{
			ELOG("Error : Texture::Init() Failed.");
			return false;
		}
	}

	// バッチ終了.
	auto future = batch.End(commandQueue.Get());

	// 完了を待機.
	future.wait();

	return true;
}
bool SkyManager::InitSphereMapConverter(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvPool, DescriptorPool* resPool) {
	if (!m_SphereMapConverter.Init(
		pDevice.Get(),
		rtvPool,
		resPool,
		m_SphereMap.GetResource()->GetDesc()))
	{
		ELOG("Error : SphereMapConverter::Init() Failed.");
		return false;
	}

	return true;
}
bool SkyManager::InitSkyBox(ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool) {
	if (!m_SkyBox.Init(
		pDevice.Get(),
		resPool,
		DXGI_FORMAT_R10G10B10A2_UNORM,
		DXGI_FORMAT_D32_FLOAT))
	{
		ELOG("Error : SkyBox::Init() Failed.");
		return false;
	}

	return true;
}

// 引数が多すぎる
bool SkyManager::IBLBake(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvPool, DescriptorPool* resPool, CommandList& commandList, ComPtr<ID3D12CommandQueue> commandQueue, Fence& fence) {
	if (!m_IBLBaker.Init(pDevice.Get(), resPool, rtvPool))
	{
		ELOG("Error : IBLBaker::Init() Failed.");
		return false;
	}

	// コマンドリストの記録を開始.
	auto pCmd = commandList.Reset();

	ID3D12DescriptorHeap* const pHeaps[] = {
		resPool->GetHeap(),
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
	commandQueue->ExecuteCommandLists(1, pLists);

	// 完了を待機.
	fence.Sync(commandQueue.Get());

	return true;
}

void SkyManager::Term()
{
	m_IBLBaker.Term();
	m_SphereMapConverter.Term();
	m_SphereMap.Term();
	m_SkyBox.Term();
}
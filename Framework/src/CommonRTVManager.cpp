#include "CommonRTVManager.h"
#include "Logger.h"
#include <DirectXHelpers.h>

bool CommonRTManager::CreateColorTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, float width, float height) {
	float clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

	if (!m_SceneColorTarget.Init(
		pDevice.Get(),
		rtvpool,
		respool,
		width,
		height,
		DXGI_FORMAT_R16G16B16A16_UNORM,
		clearColor))
	{
		ELOG("Error : ColorTarget::Init() Failed.");
		return false;
	}

	return true;
}
bool CommonRTManager::CreateDepthTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* dsvpool, DescriptorPool* respool, float width, float height) {
	if (!m_SceneDepthTarget.Init(
		pDevice.Get(),
		dsvpool,
		respool,
		width,
		height,
		DXGI_FORMAT_D32_FLOAT,
		1.0f,
		0))
	{
		ELOG("Error : DepthTarget::Init() Failed.");
		return false;
	}
	return true;
}

bool CommonRTManager::CreateShadowTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* dsvpool, DescriptorPool* respool, float width, float height) {
	if (!m_SceneShadowTarget.Init(
		pDevice.Get(),
		dsvpool,
		respool,
		width,
		height,
		DXGI_FORMAT_D32_FLOAT,
		1.0f,
		0))
	{
		ELOG("Error : DepthTarget::Init() Failed.");
		return false;
	}
	return true;
}

bool CommonRTManager::Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, DescriptorPool* dsvpool, float width, float height)
{
	if (!CreateColorTarget(pDevice, rtvpool, respool, width, height))   return false;
	if (!CreateDepthTarget(pDevice, dsvpool, respool, width, height))            return false;
	if (!CreateShadowTarget(pDevice, dsvpool, respool, width, height))			return false;

	return true;
}

void CommonRTManager::Term()
{
	m_SceneColorTarget.Term();
	m_SceneDepthTarget.Term();
	m_SceneShadowTarget.Term();
}
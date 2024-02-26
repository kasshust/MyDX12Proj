#include "CommonRTVManager.h"
#include "Logger.h"
#include <DirectXHelpers.h>

bool CommonRTManager::CreateColorTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, uint32_t width, uint32_t height) {
	float clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

	if (!m_SceneColorTarget.Init(
		pDevice.Get(),
		rtvpool,
		respool,
		width,
		height,
		DXGI_FORMAT_R10G10B10A2_UNORM,
		clearColor,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	))
	{
		ELOG("Error : ColorTarget::Init() Failed.");
		return false;
	}

	return true;
}

bool CommonRTManager::CreateTempTarget(ColorTarget& temp, ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, uint32_t width, uint32_t height) {
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	if (!temp.Init(
		pDevice.Get(),
		rtvpool,
		respool,
		width,
		height,
		DXGI_FORMAT_R10G10B10A2_UNORM,
		clearColor,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	))
	{
		ELOG("Error : TempColorTarget::Init() Failed.");
		return false;
	}

	return true;
}

bool CommonRTManager::CreateDepthTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* dsvpool, DescriptorPool* respool, uint32_t width, uint32_t height) {
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

	if (!m_PreDepthTarget.Init(
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

bool CommonRTManager::CreateShadowTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* dsvpool, DescriptorPool* respool, uint32_t width, uint32_t height) {
	if (!m_SceneShadowTarget.Init(
		pDevice.Get(),
		dsvpool,
		respool,
		width,
		height,
		DXGI_FORMAT_D32_FLOAT,
		1.0f,
		0,
		D3D12_RESOURCE_STATE_GENERIC_READ
	))
	{
		ELOG("Error : DepthTarget::Init() Failed.");
		return false;
	}
	return true;
}

bool CommonRTManager::Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, DescriptorPool* dsvpool, uint32_t width, uint32_t height)
{
	if (!CreateColorTarget(pDevice, rtvpool, respool, width, height))				return false;
	if (!CreateDepthTarget(pDevice, dsvpool, respool, width, height))				return false;
	if (!CreateShadowTarget(pDevice, dsvpool, respool, width, height))				return false;

	if (!CreateTempTarget(m_TempColorTarget, pDevice, rtvpool, respool, width, height)) return false;
	if (!CreateTempTarget(m_NormalTarget, pDevice, rtvpool, respool, width, height)) return false;


	int size = sizeof(m_BloomColorTarget) / sizeof(m_BloomColorTarget[0]);
	for (size_t i = 0; i < size; i++)
	{
		if (!CreateTempTarget(m_BloomColorTarget[i], pDevice, rtvpool, respool, width / (pow(2,i + 1)), height / (pow(2, i + 1)))) return false;
	}

	return true;
}

void CommonRTManager::Term()
{
	m_SceneColorTarget.Term();
	m_SceneDepthTarget.Term();
	m_SceneShadowTarget.Term();
}
#pragma once
#include <ColorTarget.h>
#include <DepthTarget.h>

class CommonRTManager {
public:

	ColorTarget                     m_SceneColorTarget;             //!< シーン用レンダーターゲット
	ColorTarget                     m_TempColorTarget;				//!<  一時的に使用する場合のターゲット
	ColorTarget                     m_NormalTarget;					//!<  NormalMap
	


	DepthTarget                     m_SceneDepthTarget;				//!< シーン用深度ターゲット
	DepthTarget                     m_PreDepthTarget;				//!< PreDepth

	DepthTarget                     m_SceneShadowTarget;			//!< シャドウ用深度ターゲット

	ColorTarget                     m_BloomColorTarget[4];             //!<  Bloom用


	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, DescriptorPool* dsvpool, uint32_t width, uint32_t height);

	void Term();
private:
	bool CreateColorTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, uint32_t width, uint32_t height);
	bool CreateTempTarget(ColorTarget& temp, ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, uint32_t width, uint32_t height);
	bool CreateDepthTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* dsvpool, DescriptorPool* respool, uint32_t width, uint32_t height);
	bool CreateShadowTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* dsvpool, DescriptorPool* respool, uint32_t width, uint32_t height);
};
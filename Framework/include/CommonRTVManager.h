#pragma once
#include <ColorTarget.h>
#include <DepthTarget.h>

class CommonRTManager {
public:

	ColorTarget                     m_SceneColorTarget;             //!< シーン用レンダーターゲット
	DepthTarget                     m_SceneDepthTarget;				//!< シーン用深度ターゲット
	DepthTarget                     m_SceneShadowTarget;				//!< シャドウ用深度ターゲット
	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, DescriptorPool* dsvpool, float width, float height);

	void Term();
private:
	bool CreateColorTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, float width, float height);
	bool CreateDepthTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* dsvpool, DescriptorPool* respool, float width, float height);
	bool CreateShadowTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* dsvpool, DescriptorPool* respool, float width, float height);
};
#pragma once
#include <ColorTarget.h>
#include <DepthTarget.h>

class CommonRTManager {
public:

	ColorTarget                     m_SceneColorTarget;             //!< �V�[���p�����_�[�^�[�Q�b�g
	DepthTarget                     m_SceneDepthTarget;				//!< �V�[���p�[�x�^�[�Q�b�g
	DepthTarget                     m_SceneShadowTarget;				//!< �V���h�E�p�[�x�^�[�Q�b�g
	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, DescriptorPool* dsvpool, float width, float height);

	void Term();
private:
	bool CreateColorTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, float width, float height);
	bool CreateDepthTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* dsvpool, DescriptorPool* respool, float width, float height);
	bool CreateShadowTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* dsvpool, DescriptorPool* respool, float width, float height);
};
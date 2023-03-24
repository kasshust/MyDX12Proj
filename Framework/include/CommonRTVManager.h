#pragma once
#include <ColorTarget.h>
#include <DepthTarget.h>

class CommonRTManager {
public:

	ColorTarget                     m_SceneColorTarget;             //!< �V�[���p�����_�[�^�[�Q�b�g�ł�.
	DepthTarget                     m_SceneDepthTarget;
	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, DescriptorPool* dsvpool, float width, float height);

	void Term();
	//!< �V�[���p�[�x�^�[�Q�b�g�ł�.
private:
	bool CreateColorTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, float width, float height);
	bool CreateDepthTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* dsvpool, float width, float height);
};
#pragma once
#include <ColorTarget.h>
#include <DepthTarget.h>

class CommonRTManager {
public:

	ColorTarget                     m_SceneColorTarget;             //!< �V�[���p�����_�[�^�[�Q�b�g
	ColorTarget                     m_TempColorTarget;				//!<  �ꎞ�I�Ɏg�p����ꍇ�̃^�[�Q�b�g
	ColorTarget                     m_NormalTarget;					//!<  NormalMap
	


	DepthTarget                     m_SceneDepthTarget;				//!< �V�[���p�[�x�^�[�Q�b�g
	DepthTarget                     m_PreDepthTarget;				//!< PreDepth

	DepthTarget                     m_SceneShadowTarget;			//!< �V���h�E�p�[�x�^�[�Q�b�g

	ColorTarget                     m_BloomColorTarget[4];             //!<  Bloom�p


	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, DescriptorPool* dsvpool, uint32_t width, uint32_t height);

	void Term();
private:
	bool CreateColorTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, uint32_t width, uint32_t height);
	bool CreateTempTarget(ColorTarget& temp, ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvpool, DescriptorPool* respool, uint32_t width, uint32_t height);
	bool CreateDepthTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* dsvpool, DescriptorPool* respool, uint32_t width, uint32_t height);
	bool CreateShadowTarget(ComPtr<ID3D12Device> pDevice, DescriptorPool* dsvpool, DescriptorPool* respool, uint32_t width, uint32_t height);
};
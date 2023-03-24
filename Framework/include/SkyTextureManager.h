#pragma once
#include <IBLBaker.h>
#include <SkyBox.h>
#include <Texture.h>
#include <SphereMapConverter.h>
#include <IBLBaker.h>
#include <Fence.h>
#include <CommandList.h>

class SkyTextureManager {
	SkyBox                          m_SkyBox;                       //!< �X�J�C�{�b�N�X�ł�.
	Texture                         m_SphereMap;                    //!< �X�t�B�A�}�b�v�ł�.
	SphereMapConverter              m_SphereMapConverter;           //!< �X�t�B�A�}�b�v�R���o�[�^.

	bool InitSphereMapTexture(ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue);
	bool InitSphereMapConverter(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvPool, DescriptorPool* resPool);
	bool InitSkyBox(ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool);

public:
	IBLBaker                        m_IBLBaker;                     //!< IBL�x�C�N.

	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvPool, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue);
	bool IBLBake(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvPool, DescriptorPool* resPool, CommandList& commandList, ComPtr<ID3D12CommandQueue> commandQueue, Fence& fence);

	SkyBox* GetSkyBox() { return &m_SkyBox; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCubeMapHandleGPU() { return m_SphereMapConverter.GetCubeMapHandleGPU(); };

	void Term();
};
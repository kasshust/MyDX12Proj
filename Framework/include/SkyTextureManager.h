#pragma once
#include <IBLBaker.h>
#include <SkyBox.h>
#include <Texture.h>
#include <SphereMapConverter.h>
#include <IBLBaker.h>
#include <Fence.h>
#include <CommandList.h>

class SkyManager {

public:
	IBLBaker                        m_IBLBaker;                     //!< IBLベイク.

	bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvPool, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue, std::wstring path);
	bool IBLBake(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvPool, DescriptorPool* resPool, CommandList& commandList, ComPtr<ID3D12CommandQueue> commandQueue, Fence& fence);

	SkyBox* GetSkyBox() { return &m_SkyBox; }
	D3D12_GPU_DESCRIPTOR_HANDLE GetCubeMapHandleGPU() { return m_SphereMapConverter.GetCubeMapHandleGPU(); };

	void Term();

private:
	SkyBox                          m_SkyBox;                       //!< スカイボックスです.
	Texture                         m_SphereMap;                    //!< スフィアマップです.
	SphereMapConverter              m_SphereMapConverter;           //!< スフィアマップコンバータ.

	std::wstring m_SkyTexturePath;

	bool InitSphereMapTexture(ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool, ComPtr<ID3D12CommandQueue> commandQueue, std::wstring path);
	bool InitSphereMapConverter(ComPtr<ID3D12Device> pDevice, DescriptorPool* rtvPool, DescriptorPool* resPool);
	bool InitSkyBox(ComPtr<ID3D12Device> pDevice, DescriptorPool* resPool);
};
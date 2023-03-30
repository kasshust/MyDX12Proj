#pragma once
#include <Renderer.h>
#include "DirectXHelpers.h"
#include "Logger.h"
#include <App.h>
#include <ConstantBuffer.h>
#include <RootSignature.h>


class PostEffect : public Renderer {
public:
	virtual bool Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format) = 0;
	virtual void Term() = 0;

protected:
	ConstantBuffer                  m_CB[App::FrameCount]; 
};
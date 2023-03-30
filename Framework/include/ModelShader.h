#pragma once

#include "DirectXHelpers.h"
#include "Logger.h"
#include <App.h>
#include <ConstantBuffer.h>
#include <RootSignature.h>
#include <IBLBaker.h>
#include <CommonBufferManager.h>
#include <Material.h>
#include <SkyTextureManager.h>
#include <Renderer.h>

class Material;
class CommonBufferManager;

class ModelShader : public Renderer {
public:
	bool Init(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);
	void Term();
	virtual void SetShader(
		ID3D12GraphicsCommandList* pCmd,
		int frameindex,
		Material& mat,
		int id,
		const ConstantBuffer* meshCB,
		const CommonBufferManager& commonbufmanager,
		const SkyManager& manager
	)  = 0;
};
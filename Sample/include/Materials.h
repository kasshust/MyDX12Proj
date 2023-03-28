#pragma once

#include <ShaderedMaterial.h>

class BasicMaterial : public ShaderedMaterial {
public:
	virtual void SetShaderedMaterial(ID3D12GraphicsCommandList* pCmd, int frameindex, int id, const CommonBufferManager& commonbufmanager, const IBLBaker& baker) override;
};

void BasicMaterial::SetShaderedMaterial(ID3D12GraphicsCommandList* pCmd, int frameindex, int id, const CommonBufferManager& commonbufmanager, const IBLBaker& baker) {
	// m_Shader.SetShader(pCmd, frameindex, m_Material, id, commonbufmanager, baker);
}
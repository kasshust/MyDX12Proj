#pragma once

#include <Material.h>
#include <ModelShader.h>

class ShaderedMaterial {
public:

	virtual void SetShaderedMaterial(
		ID3D12GraphicsCommandList* pCmd,
		int frameindex,
		int id,
		const CommonBufferManager& commonbufmanager,
		const IBLBaker& baker
	);

	bool Init(const Material* mat, const ModelShader* shader);

protected:

	const Material* m_Material;
	const ModelShader* m_Shader;
};
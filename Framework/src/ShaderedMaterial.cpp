#include <ShaderedMaterial.h>
#include <CommonBufferManager.h>

bool ShaderedMaterial::Init(const Material* mat, const ModelShader* shader)
{
	m_Material = mat;
	m_Shader = shader;

	return true;
}
#include "Logger.h"
#include "DirectXHelpers.h"
#include <CommonStates.h>
#include <FileUtil.h>
#include <ModelLoader.h>
#include "Shaders.h"

// BasicShader
bool BasicShader::CreateRootSig(ComPtr<ID3D12Device> pDevice) {
	RootSignature::Desc desc;
	desc.Begin(11)
		.SetCBV(ShaderStage::VS, 0, 0)
		.SetCBV(ShaderStage::VS, 1, 1)
		.SetCBV(ShaderStage::PS, 2, 1)
		.SetCBV(ShaderStage::PS, 3, 2)
		.SetSRV(ShaderStage::PS, 4, 0)
		.SetSRV(ShaderStage::PS, 5, 1)
		.SetSRV(ShaderStage::PS, 6, 2)
		.SetSRV(ShaderStage::PS, 7, 3)
		.SetSRV(ShaderStage::PS, 8, 4)
		.SetSRV(ShaderStage::PS, 9, 5)
		.SetSRV(ShaderStage::PS, 10, 6)
		.AddStaticSmp(ShaderStage::PS, 0, SamplerState::LinearWrap)
		.AddStaticSmp(ShaderStage::PS, 1, SamplerState::LinearWrap)
		.AddStaticSmp(ShaderStage::PS, 2, SamplerState::LinearWrap)
		.AddStaticSmp(ShaderStage::PS, 3, SamplerState::LinearWrap)
		.AddStaticSmp(ShaderStage::PS, 4, SamplerState::LinearWrap)
		.AddStaticSmp(ShaderStage::PS, 5, SamplerState::LinearWrap)
		.AddStaticSmp(ShaderStage::PS, 6, SamplerState::LinearWrap)
		.AllowIL()
		.End();

	if (!m_RootSig.Init(pDevice.Get(), desc.GetDesc()))
	{
		ELOG("Error : RootSignature::Init() Failed.");
		return false;
	}

	return true;
}
bool BasicShader::CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat) {
	std::wstring vsPath;
	std::wstring psPath;

	// ���_�V�F�[�_������.
	if (!SearchFilePath(m_VSPath, vsPath))
	{
		ELOG("Error : Vertex Shader Not Found.");
		return false;
	}

	// �s�N�Z���V�F�[�_������.
	if (!SearchFilePath(m_PSPath, psPath))
	{
		ELOG("Error : Pixel Shader Node Found.");
		return false;
	}

	ComPtr<ID3DBlob> pVSBlob;
	ComPtr<ID3DBlob> pPSBlob;

	// ���_�V�F�[�_��ǂݍ���.
	auto hr = D3DReadFileToBlob(vsPath.c_str(), pVSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		ELOG("Error : D3DReadFiledToBlob() Failed. path = %ls", vsPath.c_str());
		return false;
	}

	// �s�N�Z���V�F�[�_��ǂݍ���.
	hr = D3DReadFileToBlob(psPath.c_str(), pPSBlob.GetAddressOf());
	if (FAILED(hr))
	{
		ELOG("Error : D3DReadFileToBlob() Failed. path = %ls", psPath.c_str());
		return false;
	}

	D3D12_INPUT_ELEMENT_DESC elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g��ݒ�.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.InputLayout = { elements, 4 };
	desc.pRootSignature = m_RootSig.GetPtr();
	desc.VS = { pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize() };
	desc.PS = { pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize() };
	desc.RasterizerState = DirectX::CommonStates::CullNone;
	desc.BlendState = DirectX::CommonStates::Opaque;
	desc.DepthStencilState = DirectX::CommonStates::DepthDefault;
	desc.SampleMask = UINT_MAX;
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.NumRenderTargets = 1;
	desc.RTVFormats[0] = rtvFormat;
	desc.DSVFormat = dsvFormat;

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	// �p�C�v���C���X�e�[�g�𐶐�.
	hr = pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pPSO.GetAddressOf()));
	if (FAILED(hr))
	{
		ELOG("Error : ID3D12Device::CreateGraphicsPipelineState() Failed. retcode = 0x%x", hr);
		return false;
	}

	return true;
}
void BasicShader::SetShader(ID3D12GraphicsCommandList* pCmd, int frameindex, const Material& mat, int id, const CommonBufferManager& commonbufmanager, const IBLBaker& baker)
{
	//�@�}�e���A�����ʂ̃o�b�t�@
	{
		pCmd->SetGraphicsRootSignature(m_RootSig.GetPtr());
		pCmd->SetGraphicsRootDescriptorTable(0, commonbufmanager.m_TransformCB[frameindex].GetHandleGPU());


		pCmd->SetGraphicsRootDescriptorTable(2, commonbufmanager.m_LightCB[frameindex].GetHandleGPU());
		pCmd->SetGraphicsRootDescriptorTable(3, commonbufmanager.m_CameraCB[frameindex].GetHandleGPU());
		pCmd->SetGraphicsRootDescriptorTable(4, baker.GetHandleGPU_DFG());
		pCmd->SetGraphicsRootDescriptorTable(5, baker.GetHandleGPU_DiffuseLD());
		pCmd->SetGraphicsRootDescriptorTable(6, baker.GetHandleGPU_SpecularLD());
	}
	

	//�@�}�e���A�����ƂɈقȂ�o�b�t�@
	{
		pCmd->SetGraphicsRootDescriptorTable(7,		mat.GetTextureHandle(id, Material::TEXTURE_USAGE_04));
		pCmd->SetGraphicsRootDescriptorTable(8,		mat.GetTextureHandle(id, Material::TEXTURE_USAGE_05));
		pCmd->SetGraphicsRootDescriptorTable(9,		mat.GetTextureHandle(id, Material::TEXTURE_USAGE_06));
		pCmd->SetGraphicsRootDescriptorTable(10,	mat.GetTextureHandle(id, Material::TEXTURE_USAGE_03));
	}
	pCmd->SetPipelineState(m_pPSO.Get());
}

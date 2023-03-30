#include "Logger.h"
#include "DirectXHelpers.h"
#include <CommonStates.h>
#include <FileUtil.h>
#include <ModelLoader.h>
#include <Renderer.h>
#include "Shaders.h"


// BasicShader
bool BasicShader::CreateRootSig(ComPtr<ID3D12Device> pDevice) {
	RootSignature::Desc desc;
	desc.Begin(12)
		
		//VSの定数バッファ
		.SetCBV(ShaderStage::VS, 0, 0)
		.SetCBV(ShaderStage::VS, 1, 1)

		//PSの定数バッファ
		.SetCBV(ShaderStage::PS, 2, 1)
		.SetCBV(ShaderStage::PS, 3, 2)
		.SetCBV(ShaderStage::PS, 4,3)

		// テクスチャ
		.SetSRV(ShaderStage::PS, 5, 0)
		.SetSRV(ShaderStage::PS, 6, 1)
		.SetSRV(ShaderStage::PS, 7, 2)
		.SetSRV(ShaderStage::PS, 8, 3)
		.SetSRV(ShaderStage::PS, 9, 4)
		.SetSRV(ShaderStage::PS, 10, 5)
		.SetSRV(ShaderStage::PS, 11, 6)

		.AddStaticSmp(ShaderStage::PS, 0, SamplerState::LinearWrap)
		.AddStaticSmp(ShaderStage::PS, 1, SamplerState::LinearWrap)
		.AddStaticSmp(ShaderStage::PS, 2, SamplerState::LinearWrap)
		.AddStaticSmp(ShaderStage::PS, 3, SamplerState::LinearWrap)
		.AddStaticSmp(ShaderStage::PS, 4, SamplerState::LinearWrap)
		.AddStaticSmp(ShaderStage::PS, 5, SamplerState::LinearWrap)
		.AddStaticSmp(ShaderStage::PS, 6, SamplerState::LinearWrap)
		.AllowIL()
		.End();

	if (!InitRootSignature(pDevice, desc, m_RootSig)) return false;

	return true;
}
bool BasicShader::CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat) {
	std::wstring vsPath;
	std::wstring psPath;
	ComPtr<ID3DBlob> pVSBlob;
	ComPtr<ID3DBlob> pPSBlob;

	if (!SearchAndLoadShader(m_VSPath, pVSBlob)) return false;
	if (!SearchAndLoadShader(m_PSPath, pPSBlob)) return false;

	D3D12_INPUT_ELEMENT_DESC elements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// グラフィックスパイプラインステートを設定.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	desc.InputLayout                        = { elements, 4 };
	desc.pRootSignature                     = m_RootSig.GetPtr();
	desc.VS                                 = { pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize() };
	desc.PS                                 = { pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize() };
	desc.RasterizerState                    = DirectX::CommonStates::CullNone;
	desc.BlendState                         = DirectX::CommonStates::Opaque;
	desc.DepthStencilState                  = DirectX::CommonStates::DepthDefault;
	desc.SampleMask                         = UINT_MAX;
	desc.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc.NumRenderTargets                   = 1;
	desc.RTVFormats[0]                      = rtvFormat;
	desc.DSVFormat                          = dsvFormat;

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	// パイプラインステートを生成.
	if (!CreateGraphicsPipelineState(pDevice, desc, m_pPSO))return false;

	return true;
}
void BasicShader::SetShader(ID3D12GraphicsCommandList* pCmd, int frameindex, Material& mat, int id, const ConstantBuffer* meshCB, const CommonBufferManager& commonbufmanager, const SkyManager& manager)
{
	//　マテリアル共通のバッファ
	{
		pCmd->SetGraphicsRootSignature(m_RootSig.GetPtr());

		// ココを外部化したい
		{
			pCmd->SetGraphicsRootDescriptorTable(0, commonbufmanager.m_TransformCB[frameindex].GetHandleGPU());
			pCmd->SetGraphicsRootDescriptorTable(2, commonbufmanager.m_LightCB[frameindex].GetHandleGPU());
			pCmd->SetGraphicsRootDescriptorTable(3, commonbufmanager.m_CameraCB[frameindex].GetHandleGPU());
		}

		pCmd->SetGraphicsRootDescriptorTable(5, manager.m_IBLBaker.GetHandleGPU_DFG());
		pCmd->SetGraphicsRootDescriptorTable(6, manager.m_IBLBaker.GetHandleGPU_DiffuseLD());
		pCmd->SetGraphicsRootDescriptorTable(7, manager.m_IBLBaker.GetHandleGPU_SpecularLD());
	}

	pCmd->SetGraphicsRootDescriptorTable(1, meshCB[frameindex].GetHandleGPU());

	//  マテリアルから定数バッファを取り出す
	pCmd->SetGraphicsRootDescriptorTable(4, mat.GetBufferHandle(id));

	//　マテリアルごとに異なるバッファ
	{
		pCmd->SetGraphicsRootDescriptorTable(8,		mat.GetTextureHandle(id, Material::TEXTURE_USAGE_04));
		pCmd->SetGraphicsRootDescriptorTable(9,		mat.GetTextureHandle(id, Material::TEXTURE_USAGE_05));
		pCmd->SetGraphicsRootDescriptorTable(10,	mat.GetTextureHandle(id, Material::TEXTURE_USAGE_06));
		pCmd->SetGraphicsRootDescriptorTable(11,	mat.GetTextureHandle(id, Material::TEXTURE_USAGE_03));
	}
	pCmd->SetPipelineState(m_pPSO.Get());
}
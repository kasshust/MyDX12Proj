#include "ExtractHighIntensity.h"

bool ExtractHightIntensity::Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)
{
	return true;
}

void ExtractHightIntensity::Term()
{
}

void ExtractHightIntensity::Draw(ID3D12GraphicsCommandList* pCmd, int frameindex, DrawSource& s)
{
}

bool ExtractHightIntensity::CreateRootSig(ComPtr<ID3D12Device> pDevice)
{
	return false;
}

bool ExtractHightIntensity::CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)
{
	return false;
}

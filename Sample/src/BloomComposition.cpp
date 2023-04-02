#include "BloomComPosition.h"

bool BloomComposition::Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)
{
	return true;
}

bool BloomComposition::CreateRootSig(ComPtr<ID3D12Device> pDevice)
{
	return false;
}

bool BloomComposition::CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)
{
	return false;
}


void BloomComposition::Term()
{
}

void BloomComposition::Draw(ID3D12GraphicsCommandList* pCmd, int frameindex, DrawSource& s)
{
}


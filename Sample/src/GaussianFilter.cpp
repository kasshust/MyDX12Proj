#include "GaussianFilter.h"

bool GaussianFilter::Init(ComPtr<ID3D12Device> pDevice, DescriptorPool* pool, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)
{
	return true;
}

void GaussianFilter::Term()
{
}

void GaussianFilter::Draw(ID3D12GraphicsCommandList* pCmd, int frameindex, DrawSource& s)
{
}

bool GaussianFilter::CreateRootSig(ComPtr<ID3D12Device> pDevice)
{
	return false;
}

bool GaussianFilter::CreatePipeLineState(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtv_format, DXGI_FORMAT dsv_format)
{
	return false;
}

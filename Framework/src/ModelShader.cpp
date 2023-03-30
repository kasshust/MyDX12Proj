#include "ModelShader.h"
#include "Logger.h"
#include "DirectXHelpers.h"
#include <CommonStates.h>
#include <FileUtil.h>

bool ModelShader::Init(ComPtr<ID3D12Device> pDevice, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat)
{
	if (!CreateRootSig(pDevice.Get()))                  return false;
	if (!CreatePipeLineState(pDevice.Get(),
		rtvFormat,
		dsvFormat))                                          return false;

	return true;
}

void ModelShader::Term()
{
	m_pPSO.Reset();
	m_RootSig.Term();
}
﻿//-----------------------------------------------------------------------------
// File : App.h
// Desc : Application Module.
// Copyright(c) Pocol. All right reserved.
//-----------------------------------------------------------------------------
#pragma once

#define NOMINMAX

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <ComPtr.h>
#include <DescriptorPool.h>
#include <ColorTarget.h>
#include <DepthTarget.h>
#include <CommandList.h>
#include <Fence.h>
#include <Mesh.h>
#include <Texture.h>
#include <InlineUtil.h>

//-----------------------------------------------------------------------------
// Linker
//-----------------------------------------------------------------------------
#pragma comment( lib, "d3d12.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

///////////////////////////////////////////////////////////////////////////////
// Global Instance
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// App class
///////////////////////////////////////////////////////////////////////////////
class App
{
	//=========================================================================
	// list of friend classes and methods.
	//=========================================================================
	/* NOTHING */

public:
	//=========================================================================
	// public variables.
	//=========================================================================
	static const uint32_t FrameCount = 2;   // フレームバッファ数です.

	//=========================================================================
	// public methods.
	//=========================================================================

	//-------------------------------------------------------------------------
	//! @brief      コンストラクタです.
	//-------------------------------------------------------------------------
	App(uint32_t width, uint32_t height, DXGI_FORMAT format);

	//-------------------------------------------------------------------------
	//! @brief      デストラクタです.
	//-------------------------------------------------------------------------
	virtual ~App();

	//-------------------------------------------------------------------------
	//! @brief      アプリケーションを実行します.
	//-------------------------------------------------------------------------
	void Run();

protected:
	///////////////////////////////////////////////////////////////////////////
	// POOL_TYPE enum
	///////////////////////////////////////////////////////////////////////////
	enum POOL_TYPE
	{
		POOL_TYPE_RES = 0,     // CBV / SRV / UAV
		POOL_TYPE_SMP = 1,     // Sampler
		POOL_TYPE_RTV = 2,     // RTV
		POOL_TYPE_DSV = 3,     // DSV
		POOL_COUNT = 4,
	};

	//=========================================================================
	// private variables.
	//=========================================================================

	HINSTANCE   m_hInst;        // インスタンスハンドルです.
	HWND        m_hWnd;         // ウィンドウハンドルです.
	uint32_t    m_Width;        // ウィンドウの横幅です.
	uint32_t    m_Height;       // ウィンドウの縦幅です.

	ComPtr<IDXGIFactory4>       m_pFactory;                  // DXGIファクトリーです.
	ComPtr<ID3D12Device>        m_pDevice;                   // デバイスです.
	ComPtr<ID3D12CommandQueue>  m_pQueue;                    // コマンドキューです.
	ComPtr<IDXGISwapChain4>     m_pSwapChain;                // スワップチェインです.
	ColorTarget                 m_ColorTarget[FrameCount];   // カラーターゲットです.
	DepthTarget                 m_DepthTarget;               // 深度ターゲットです.
	DescriptorPool* m_pPool[POOL_COUNT];         // ディスクリプタプールです.
	CommandList                 m_CommandList;               // コマンドリストです.
	Fence                       m_Fence;                     // フェンスです.
	uint32_t                    m_FrameIndex;                // フレーム番号です.
	D3D12_VIEWPORT              m_Viewport;                  // ビューポートです.
	D3D12_RECT                  m_Scissor;                   // シザー矩形です.
	DXGI_FORMAT                 m_BackBufferFormat;          // バックバッファフォーマットです.

	ID3D12DescriptorHeap*		m_ImGuiDescriptorHeap;		 // ImGui用のディスクリプタヒープ
	CommandList                 m_ImGuiCommandList;          // ImGui用のコマンドリストです.

	//=========================================================================
	// protected methods.
	//=========================================================================
	void Present(uint32_t interval);
	bool  IsSupportHDR() const;
	float GetMaxLuminance() const;
	float GetMinLuminance() const;

	virtual bool OnInit()
	{
		return true;
	}

	virtual void OnTerm()
	{ /* DO_NOTHING */
	}

	virtual void OnRender()
	{ /* DO_NOTHING */
	}

	virtual void OnMsgProc(HWND, UINT, WPARAM, LPARAM)
	{ /* DO_NOTHING */
	}

	virtual void OnRenderIMGUI()
	{ /* DO_NOTHING */
	}

private:
	//=========================================================================
	// private variables.
	//=========================================================================
	bool    m_SupportHDR;   // HDRディスプレイをサポートしているかどうか?
	float   m_MaxLuminance; // ディスプレイの最大輝度値.
	float   m_MinLuminance; // ディスプレイの裁量輝度値.

	//=========================================================================
	// private methods.
	//=========================================================================
	bool InitGlobal();
	void TermGlobal();
	bool InitApp();
	void TermApp();
	bool InitIMGUI();
	void TermIMGUI();
	bool InitWnd();
	void TermWnd();
	bool InitD3D();
	void TermD3D();
	void MainLoop();
	void CheckSupportHDR();

protected:
	void OnRenderIMGUICommonProcess();

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
};

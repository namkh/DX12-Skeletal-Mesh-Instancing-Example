#pragma once

#include "Defaults.h"

#include "Singleton.h"
#include "GlobalSystemValues.h"
#include "CommandQueue.h"
#include "SwapChain.h"
#include "CoreEventManager.h"

#include <functional>

class DX12DeviceResource
{
public:
	bool Initialize(HINSTANCE hAppInstance, HWND hMainWnd, uint32_t width, uint32_t height);
	void Destroy();

	bool CreateDevice();
	bool CreateCommandObjects();

public:
	void RefreshDeviceResIfDirtied();
	void RefreshViewportAndScissorRect();

public:
	void WaitForCommandQueue();

public:
	void OnScreenSizeChanged(ScreenSizeChangedEvent* screenSizeChangedEvent);
	LambdaCommandList<std::function<void()> > OnRenderTargetSizeChanged;

public:

	ID3D12Device2* GetLogicalDevice() { return m_logicalDevice;}
	IDXGIFactory* GetGIFactory()	{ return m_GIFactory; }
	CommandQueue& GetDefaultCommandQueue()	{ return m_commandQueue; }

	SwapChain& GetSwapChain() { return m_swapChain; }

	bool GetMsaaState()			{ return m_msaa4XState; }
	uint32_t GetMsaaQuality()	{ return m_msaa4XQuality; }

	uint32_t GetRtvDiscriptorSize()			{ return m_rtvDiscriptorSize; }
	uint32_t GetDsvDiscriptorSize()			{ return m_dsvDiscriptorSize; }
	uint32_t GetCbvSrvUavDiscriptorSize()	{ return m_cbvSrvUavDiscriptorSize; }
	uint32_t GetSamplerDiscriptorSize()		{ return m_samplerDiscriptorSize; }

	D3D12_VIEWPORT& GetViewport()	{ return m_viewPort; }
	D3D12_RECT&	GetScissorRect()	{ return m_scissorRect; }

	uint32_t GetWidth() { return m_width; }
	uint32_t GetHeight() { return m_height; }

public:
	bool IsInitialized() { return m_isInitilized; }	

private:

	HINSTANCE	m_hAppInstance = nullptr;
	HWND		m_hMainWnd	= nullptr;

	IDXGIFactory4*	m_GIFactory = nullptr;
	ID3D12Device2*	m_logicalDevice = nullptr;

	CommandQueue m_commandQueue = {};
	SwapChain m_swapChain = {};

	uint32_t m_rtvDiscriptorSize = 0;
	uint32_t m_dsvDiscriptorSize = 0;
	uint32_t m_cbvSrvUavDiscriptorSize = 0;
	uint32_t m_samplerDiscriptorSize = 0;

	uint32_t m_width = 800;
	uint32_t m_height = 600;

	D3D12_VIEWPORT m_viewPort = {0, 0, 0, 0, 0, 0};
	D3D12_RECT	m_scissorRect = { 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };

	DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_depthStencilBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	
	bool m_msaa4XState = false;
	uint32_t m_msaa4XQuality = 0;

	bool m_isSupportMeshShader = false;
	uint32_t m_waveLaneMin = 0;
	uint32_t m_waveLaneMax = 0;

	bool m_isWindowMode = true;
	bool m_isInitilized = false;

	CoreEventHandle m_screenSizeChangedEventHandle = {};
};




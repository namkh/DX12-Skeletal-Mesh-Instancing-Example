#pragma once

#include "Defaults.h"
#include "CommandQueue.h"

class SwapChain
{
public:
	SwapChain() {};

public:
	SwapChain(const SwapChain& rhs) = delete;
	SwapChain& operator=(const SwapChain& rhs) = delete;

public:

	bool Initialize(IDXGIFactory* giFactroy, CommandQueue* commandQueue, HWND hwnd, uint32_t width, uint32_t height, bool isWindowMode);
	void Destroy();

	bool CreateRtvDsvDescriptorHeap();
	void CreateRenderTargetView();
	bool CreateDepthStencilView();

	void Present();

public:

	void OnScreenSizeChanged(uint32_t width, uint32_t height);
	void Refresh();

public:
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRtvHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE GetRtvHandle(uint32_t index);
	D3D12_CPU_DESCRIPTOR_HANDLE GetDsvHandle();
	ID3D12Resource* GetCurrentBackBuffer();
	ID3D12Resource* GetBackBuffer(uint32_t index);

	DXGI_FORMAT GetBackBufferFormat() { return m_backBufferFormat; }
	DXGI_FORMAT GetDepthStencilBufferFormat() { return m_depthStencilBufferFormat; }

	bool IsScreenSizeChanged() { return m_screenSizeChanged; }
	

private:
	static const int SWAP_CHAIN_BUFFER_COUNT = 2;

	IDXGISwapChain*	m_swapChain = nullptr;

	ID3D12DescriptorHeap* m_rtvDescHeap = nullptr;
	ID3D12DescriptorHeap* m_dsvDescHeap = nullptr;

	ID3D12Resource* m_swapChainBuffers[SWAP_CHAIN_BUFFER_COUNT] = 
	{
		nullptr, nullptr
	};
	ID3D12Resource*	m_depthStencilBuffer = nullptr;

	HWND m_hwnd = 0;
	bool m_isWindowMode = false;

	uint32_t m_width = 800;
	uint32_t m_height = 600;

	DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_depthStencilBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	uint8_t m_currentBackBuffer = 0;
	bool m_screenSizeChanged = false;
};
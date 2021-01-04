#include "SwapChain.h"
#include "DX12DeviceResourceAccess.h"
#include "CommandBuffers.h"

bool SwapChain::Initialize(IDXGIFactory* giFactroy, CommandQueue* commandQueue, HWND hwnd, uint32_t width, uint32_t height, bool isWindowMode)
{
	m_width = width;
	m_height = height;

	m_hwnd = 0;
	m_isWindowMode = isWindowMode;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = m_backBufferFormat;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = gDX12DeviceRes->GetMsaaState() ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = gDX12DeviceRes->GetMsaaState() ? (gDX12DeviceRes->GetMsaaQuality() - 1) : 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = isWindowMode;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT result = giFactroy->CreateSwapChain(commandQueue->GetCommandQueue(), &swapChainDesc, &m_swapChain);
	if (FAILED(result))
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Swap chain create failed.");
		return false;
	}

	if (!CreateRtvDsvDescriptorHeap())
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Swap chain rtv/dsv descriptor create failed.");
		return false;
	}

	CreateRenderTargetView();

	if (!CreateDepthStencilView())
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Swap chain dsv create failed.");
		return false;
	}

	return true;
}

void SwapChain::Destroy()
{
	for (int swapChainIndex = 0; swapChainIndex < SWAP_CHAIN_BUFFER_COUNT; swapChainIndex++)
	{
		m_swapChainBuffers[swapChainIndex]->Release();
	}

	if (m_depthStencilBuffer != nullptr)
	{
		m_depthStencilBuffer->Release();
	}

	if (m_rtvDescHeap != nullptr)
	{
		m_rtvDescHeap->Release();
	}

	if (m_dsvDescHeap != nullptr)
	{
		m_dsvDescHeap->Release();
	}

	if (m_swapChain != nullptr)
	{
		m_swapChain->Release();
	}
}

bool SwapChain::CreateRtvDsvDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SWAP_CHAIN_BUFFER_COUNT;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	HRESULT res = gLogicalDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvDescHeap));
	if (FAILED(res))
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Render target view descriptor heap create failed.");
		return false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	res = gLogicalDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvDescHeap));
	if (FAILED(res))
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Render target view descriptor heap create failed.");
		return false;
	}
	return true;
}

void SwapChain::CreateRenderTargetView()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvDescHeap->GetCPUDescriptorHandleForHeapStart());

	for (int swapChainIndex = 0; swapChainIndex < SWAP_CHAIN_BUFFER_COUNT; swapChainIndex++)
	{
		m_swapChain->GetBuffer(swapChainIndex, IID_PPV_ARGS(&m_swapChainBuffers[swapChainIndex]));
		gLogicalDevice->CreateRenderTargetView(m_swapChainBuffers[swapChainIndex], nullptr, rtvHeapHandle);
		m_swapChainBuffers[swapChainIndex]->SetName(L"RenderTarget");
		rtvHeapHandle.Offset(1, gDX12DeviceRes->GetRtvDiscriptorSize());
	}
}

bool SwapChain::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC depthStencilResourceDesc;
	depthStencilResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilResourceDesc.Alignment = 0;
	depthStencilResourceDesc.Width = m_width;
	depthStencilResourceDesc.Height = m_height;
	depthStencilResourceDesc.DepthOrArraySize = 1;
	depthStencilResourceDesc.MipLevels = 1;
	depthStencilResourceDesc.Format = m_depthStencilBufferFormat;
	depthStencilResourceDesc.SampleDesc.Count = gDX12DeviceRes->GetMsaaState() ? 4 : 1;
	depthStencilResourceDesc.SampleDesc.Quality = gDX12DeviceRes->GetMsaaState() ? (gDX12DeviceRes->GetMsaaQuality() - 1) : 0;
	depthStencilResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthStencilClearValue;
	depthStencilClearValue.Format = m_depthStencilBufferFormat;
	depthStencilClearValue.DepthStencil.Depth = 1;
	depthStencilClearValue.DepthStencil.Stencil = 0;
	
	HRESULT res = gLogicalDevice->CreateCommittedResource
	(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilResourceDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&depthStencilClearValue,
		IID_PPV_ARGS(&m_depthStencilBuffer)
	);
	if (FAILED(res))
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Depth stencil resource create failed.");
	}

	gLogicalDevice->CreateDepthStencilView(m_depthStencilBuffer, nullptr, GetDsvHandle());
	if (m_depthStencilBuffer == nullptr)
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Depth stencil resource create failed.");
		return false;
	}

	SingleTimeCommandBuffer singleTimeCmdBuffer;
	if (singleTimeCmdBuffer.Begin(nullptr))
	{
		singleTimeCmdBuffer.GetCommandBuffer()->ResourceBarrier
		(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer,
												  D3D12_RESOURCE_STATE_COMMON,
												  D3D12_RESOURCE_STATE_DEPTH_WRITE)
		);
		singleTimeCmdBuffer.End();
	}

	return true;
}

void SwapChain::Present()
{
	m_swapChain->Present(0, 0);
	m_currentBackBuffer = (m_currentBackBuffer + 1) % SWAP_CHAIN_BUFFER_COUNT;
}

void SwapChain::OnScreenSizeChanged(uint32_t width, uint32_t height)
{
	m_screenSizeChanged = true;
	m_width = width;
	m_height = height;
}

void SwapChain::Refresh()
{
	for (int swapChainIndex = 0; swapChainIndex < SWAP_CHAIN_BUFFER_COUNT; swapChainIndex++)
	{
		m_swapChainBuffers[swapChainIndex]->Release();
	}

	if (m_depthStencilBuffer != nullptr)
	{
		m_depthStencilBuffer->Release();
	}

	if (m_rtvDescHeap != nullptr)
	{
		m_rtvDescHeap->Release();
	}

	if (m_dsvDescHeap != nullptr)
	{
		m_dsvDescHeap->Release();
	}

	m_swapChain->ResizeBuffers(SWAP_CHAIN_BUFFER_COUNT,
							   m_width,
							   m_height,
							   m_backBufferFormat,
							   DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	m_currentBackBuffer = 0;

	if (!CreateRtvDsvDescriptorHeap())
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Swap chain rtv/dsv descriptor create failed.");
	}

	CreateRenderTargetView();

	if (!CreateDepthStencilView())
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Swap chain dsv create failed.");
	}

	m_screenSizeChanged = false;
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCurrentRtvHandle()
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvDescHeap->GetCPUDescriptorHandleForHeapStart(),
										 m_currentBackBuffer,
										 gDX12DeviceRes->GetRtvDiscriptorSize());
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetRtvHandle(uint32_t index)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvDescHeap->GetCPUDescriptorHandleForHeapStart(),
										 index,
										 gDX12DeviceRes->GetRtvDiscriptorSize());
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetDsvHandle()
{
	return m_dsvDescHeap->GetCPUDescriptorHandleForHeapStart();
}

ID3D12Resource* SwapChain::GetCurrentBackBuffer()
{
	return m_swapChainBuffers[m_currentBackBuffer];
}

ID3D12Resource* SwapChain::GetBackBuffer(uint32_t index)
{
	if (index < SWAP_CHAIN_BUFFER_COUNT)
	{
		return m_swapChainBuffers[index];
	}
	return nullptr;
}
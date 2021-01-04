#include <iostream>

#include "DX12DeviceResourceAccess.h"
#include "d3dx12.h"
#include "GlobalTimer.h"
#if defined(DEBUG) || defined(_DEBUG)
#include <dxgidebug.h>
#endif

#include "Utils.h"
#include "CommandBuffers.h"

bool DX12DeviceResource::Initialize(HINSTANCE hAppInstance, HWND hMainWnd, uint32_t width, uint32_t height)
{
	m_hAppInstance = hAppInstance;
	m_hMainWnd = hMainWnd;
	m_width = width;
	m_height = height;

	//디바이스 관련 생성
	if(!CreateDevice())
	{
		return false;
	}
	if (!CreateCommandObjects())
	{
		return false;
	}
	if (!m_swapChain.Initialize(m_GIFactory, &m_commandQueue, m_hMainWnd, m_width, m_height, m_isWindowMode))
	{
		return false;
	}

	RefreshViewportAndScissorRect();

	m_isInitilized = true;

	m_screenSizeChangedEventHandle = CoreEventManager::Instance().RegisterScreenSizeChangedEventCallback(this, &DX12DeviceResource::OnScreenSizeChanged);

	return true;
}

void DX12DeviceResource::Destroy()
{
	CoreEventManager::Instance().UnregisterEventCallback(m_screenSizeChangedEventHandle);

	m_commandQueue.WaitForAllFence();

	m_swapChain.Destroy();
	m_commandQueue.Destroy();

	if (m_logicalDevice != nullptr)
	{
		m_logicalDevice->Release();
	}
	
	if (m_GIFactory != nullptr)
	{
		m_GIFactory->Release();
	}
}

bool DX12DeviceResource::CreateDevice()
{
	HRESULT res = E_FAIL;
	
#if defined(DEBUG) || defined(_DEBUG)
	ID3D12Debug* d3d12DebugPtr;
	res = D3D12GetDebugInterface(IID_PPV_ARGS(&d3d12DebugPtr));
	if (!res)
	{
		REPORT(EReportType::REPORT_TYPE_WARN, "Dx12 debug layer enable failed.");
	}
	d3d12DebugPtr->EnableDebugLayer();
	d3d12DebugPtr->Release();
#endif
	
	res = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_GIFactory));
	if (FAILED(res))
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "DXGIFactory create failed.");
		return false;
	}

	res = D3D12CreateDevice(nullptr,
								D3D_FEATURE_LEVEL_12_1,
								IID_PPV_ARGS(&m_logicalDevice));

	D3D12_FEATURE_DATA_D3D12_OPTIONS1 waveIntrinsicsFeatureData;
	m_logicalDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS1, &waveIntrinsicsFeatureData, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS1));
	if (waveIntrinsicsFeatureData.WaveOps)
	{
		m_waveLaneMin = waveIntrinsicsFeatureData.WaveLaneCountMin;
		m_waveLaneMax = waveIntrinsicsFeatureData.WaveLaneCountMax;
	}
	
	D3D12_FEATURE_DATA_D3D12_OPTIONS7 meshShaderFeatureData = {};
	m_logicalDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &meshShaderFeatureData, sizeof(D3D12_FEATURE_DATA_D3D12_OPTIONS7));
	if (meshShaderFeatureData.MeshShaderTier == D3D12_MESH_SHADER_TIER_NOT_SUPPORTED)
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Mesh shader is not supported.");
	}

	if (FAILED(res))
	{
		IDXGIAdapter1* GIAdapterPtr;
		HRESULT hInlineResult = m_GIFactory->EnumAdapters1(0, &GIAdapterPtr);
		if (FAILED(res))
		{
			REPORT(EReportType::REPORT_TYPE_ERROR, "Cannot found DXGIAdepter.");
			return false;
		}

		D3D12CreateDevice(GIAdapterPtr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_logicalDevice));
	}

	m_rtvDiscriptorSize = m_logicalDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsvDiscriptorSize = m_logicalDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cbvSrvUavDiscriptorSize = m_logicalDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_samplerDiscriptorSize = m_logicalDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels;
	qualityLevels.Format = m_backBufferFormat;
	qualityLevels.SampleCount = 4;
	qualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	res = m_logicalDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
												   &qualityLevels,
												   sizeof(qualityLevels));
											   
	m_msaa4XQuality = qualityLevels.NumQualityLevels;

	return true;
}

bool DX12DeviceResource::CreateCommandObjects()
{	
	D3D12_COMMAND_QUEUE_DESC commandQueneDesc = {};
	commandQueneDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueneDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	if (!m_commandQueue.Initialize(commandQueneDesc))
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Command queue create failed.");
		return false;
	}

	return true;
}

void DX12DeviceResource::RefreshDeviceResIfDirtied()
{
	if (m_swapChain.IsScreenSizeChanged())
	{
		WaitForCommandQueue();
		m_swapChain.Refresh();
		RefreshViewportAndScissorRect();
		OnRenderTargetSizeChanged.Exec();
	}
}

void DX12DeviceResource::RefreshViewportAndScissorRect()
{
	m_viewPort.TopLeftX = 0.0f;
	m_viewPort.TopLeftY = 0.0f;
	m_viewPort.Width = static_cast<float>(m_width);
	m_viewPort.Height = static_cast<float>(m_height);
	m_viewPort.MinDepth = 0.0f;
	m_viewPort.MaxDepth = 1.0f;

	m_scissorRect = { 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
}

void DX12DeviceResource::WaitForCommandQueue()
{
	m_commandQueue.WaitForAllFence();
}

void DX12DeviceResource::OnScreenSizeChanged(ScreenSizeChangedEvent* screenSizeChangedEvent)
{
	m_width = screenSizeChangedEvent->Width;
	m_height = screenSizeChangedEvent->Height;

	m_swapChain.OnScreenSizeChanged(m_width, m_height);
}
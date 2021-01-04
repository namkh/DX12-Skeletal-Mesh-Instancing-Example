#pragma once

#include "DX12DeviceResource.h"
#include "Singleton.h"

class DX12DeviceResourceAccess : public TSingleton<DX12DeviceResourceAccess>
{
public:
	DX12DeviceResourceAccess(token) {}

	bool Initialize(HINSTANCE hAppInstance, HWND hMainWnd, uint32_t width, uint32_t height)
	{
		if (m_deviceResource == nullptr)
		{
			m_deviceResource = new DX12DeviceResource();
			return m_deviceResource->Initialize(hAppInstance, hMainWnd, width, height);
		}
		return false;
	}
	void Destroy()
	{
		if (m_deviceResource != nullptr)
		{
#if defined(DEBUG) || defined(_DEBUG)
			ID3D12DebugDevice* pDebugDevice = NULL;
			m_deviceResource->GetLogicalDevice()->QueryInterface(&pDebugDevice);
#endif
			m_deviceResource->Destroy();
			delete m_deviceResource;
			m_deviceResource = nullptr;

#if defined(DEBUG) || defined(_DEBUG)
			pDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
			pDebugDevice->Release();
			pDebugDevice = nullptr;
#endif
		}
	}
	DX12DeviceResource* GetDX12DeviceResource() { return m_deviceResource; }
private:
	DX12DeviceResource* m_deviceResource = nullptr;
};

#define gDX12DeviceRes DX12DeviceResourceAccess::Instance().GetDX12DeviceResource()
#define gLogicalDevice DX12DeviceResourceAccess::Instance().GetDX12DeviceResource()->GetLogicalDevice()
#define gGIFactory DX12DeviceResourceAccess::Instance().GetDX12DeviceResource()->GetGIFactory()

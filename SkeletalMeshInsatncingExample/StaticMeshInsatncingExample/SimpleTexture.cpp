
#include "SimpleTexture.h"
#include "CommandBuffers.h"
#include "DX12DeviceResourceAccess.h"
#include "DDSTextureLoader.h"

SimpleTexture2D::SimpleTexture2D()
{

}

bool SimpleTexture2D::Load(CommandBufferBase* cmdBuffer, const wchar_t* filePath)
{
	m_srcFilePath = filePath;

	int texChannels = 0;

	DirectX::CreateDDSTextureFromFile12
	(
		gLogicalDevice,
		cmdBuffer->GetCommandBuffer(),
		filePath,
		&m_resourceBuffer,
		&m_uploadBuffer
	);

	m_resourceBuffer->SetName(L"texture resource buffer");
	m_uploadBuffer->SetName(L"texture upload buffer");
		
	return true;
}

void SimpleTexture2D::Destroy()
{
	DecRef();
}

void SimpleTexture2D::Unload()
{
	if (m_resourceBuffer != nullptr)
	{
		m_resourceBuffer->Release();
	}
	
	if (m_uploadBuffer != nullptr)
	{
		m_uploadBuffer->Release();
	}
}
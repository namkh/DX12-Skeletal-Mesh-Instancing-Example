
#include "Fence.h"
#include "DX12DeviceResourceAccess.h"
#include "CommandBuffers.h"

bool Fence::Initialize(const wchar_t* name)
{
	HRESULT res = gLogicalDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
	m_fence->SetName(name);
	return SUCCEEDED(res);
}

bool Fence::WaitForEnd(bool notifyEnd)
{
	if (m_fence->GetCompletedValue() < m_currentFenceIndex)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		m_fence->SetEventOnCompletion(m_currentFenceIndex, eventHandle);
		if (eventHandle != 0)
		{
			WaitForSingleObject(eventHandle, UINT32_MAX);
			CloseHandle(eventHandle);
		}
	}

	for (uint32_t i = 0; i < m_submittedCommandBuffer.size(); i++)
	{
		CommandBuffer* commandBuffer = dynamic_cast<CommandBuffer*>(m_submittedCommandBuffer[i]);
		if (commandBuffer != nullptr)
		{
			commandBuffer->OnExecutionEnded();
		}
	}
	m_submittedCommandBuffer.clear();

	if (notifyEnd && NotifyWaitEnd.IsBinded())
	{
		NotifyWaitEnd.Exec(GetUID());
		NotifyWaitEnd.SetBinded(false);
	}
	
	return true;
}

void Fence::AddSubmittedCommandBuffer(CommandBufferBase* cmdBuffer)
{
	m_submittedCommandBuffer.push_back(cmdBuffer);
}

void Fence::AddSubmittedCommandBuffer(std::vector<CommandBufferBase*>& cmdBuffer)
{
	m_submittedCommandBuffer.insert(m_submittedCommandBuffer.end(), cmdBuffer.begin(), cmdBuffer.end());
}

void Fence::Destory()
{
	if (m_fence != nullptr)
	{
		m_fence->Release();
	}
}
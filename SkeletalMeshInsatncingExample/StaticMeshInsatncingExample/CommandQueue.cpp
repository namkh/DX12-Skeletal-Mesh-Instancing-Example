#include "CommandQueue.h"
#include "DX12DeviceResourceAccess.h"
#include "Utils.h"
#include "Fence.h"
#include "CommandBuffers.h"

bool CommandQueue::Initialize(D3D12_COMMAND_QUEUE_DESC& queueDesc)
{
	HRESULT res = gLogicalDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
	m_commandQueue->SetName(L"Main Queue");
	return SUCCEEDED(res);
}

void CommandQueue::Submit(CommandBufferBase* cmdBuffer, Fence* fence, bool present)
{
	if (fence == nullptr)
	{
		REPORT(EReportType::REPORT_TYPE_WARN, "Submit failed : fence is nullptr");
		return;
	}
	if (cmdBuffer == nullptr)
	{
		REPORT(EReportType::REPORT_TYPE_WARN, "Submit failed : command buffer is nullptr");
		return;
	}

	ID3D12CommandList* cmdLists[] = { cmdBuffer->GetCommandBuffer() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
	
	m_fenceValue++;
	fence->AddSubmittedCommandBuffer(cmdBuffer);
	fence->SetCurrentFenceIndex(m_fenceValue);
	fence->NotifyWaitEnd.Bind(this, &CommandQueue::OnFenceWaitEnded);
	m_waitingFences.insert(std::make_pair(fence->GetUID(), fence));
	if (present)
	{
		gDX12DeviceRes->GetSwapChain().Present();
	}
	m_commandQueue->Signal(fence->GetFence(), m_fenceValue);
}

void CommandQueue::Submit(std::vector<CommandBufferBase*>& cmdBuffers, Fence* fence, bool present)
{
	if (fence == nullptr)
	{
		REPORT(EReportType::REPORT_TYPE_WARN, "Command queue submit failed : fence is nullptr");
		return;
	}
	if (cmdBuffers.size() == 0)
	{
		REPORT(EReportType::REPORT_TYPE_WARN, "Command queue submit failed : count of command buffer list is zero");
		return;
	}
	
	std::vector<ID3D12CommandList*> commands(cmdBuffers.size());
	for (uint32_t i = 0; i < cmdBuffers.size(); i++)
	{
		commands[i] = cmdBuffers[i]->GetCommandBuffer();
	}
	ID3D12CommandList* const* submitCmdLists = commands.data();
	m_commandQueue->ExecuteCommandLists(static_cast<uint32_t>(cmdBuffers.size()), submitCmdLists);

	m_fenceValue++;
	fence->AddSubmittedCommandBuffer(cmdBuffers);
	fence->NotifyWaitEnd.Bind(this, &CommandQueue::OnFenceWaitEnded);
	fence->SetCurrentFenceIndex(m_fenceValue);
	m_waitingFences.insert(std::make_pair(fence->GetUID(), fence));
	if (present)
	{
		gDX12DeviceRes->GetSwapChain().Present();
	}
	m_commandQueue->Signal(fence->GetFence(), m_fenceValue);
}

void CommandQueue::WaitForAllFence()
{
	for (auto& cur : m_waitingFences)
	{
		if (cur.second != nullptr)
		{
			cur.second->WaitForEnd(false);
		}
	}
	m_waitingFences.clear();
}

void CommandQueue::Destroy()
{
	if (m_commandQueue != nullptr)
	{
		m_commandQueue->AddRef();
		int cnt = m_commandQueue->Release();
		m_commandQueue->Release();
	}
}

void CommandQueue::OnFenceWaitEnded(UID uid)
{
	auto finded = m_waitingFences.find(uid);
	if (finded != m_waitingFences.end())
	{
		m_waitingFences.erase(finded);
	}
}
#include "CommandBuffers.h"
#include "DX12DeviceResourceAccess.h"	
#include "Fence.h"

CommandAllocator::CommandAllocator() : UniqueIdentifier()
{
	HRESULT res = gLogicalDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_allocator));
	if (FAILED(res))
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Command allocator create failed.");
	}
	m_allocator->Reset();
}

CommandAllocator::~CommandAllocator()
{
	m_allocator->Release();
}

ID3D12CommandAllocator* CommandAllocator::GetAllocator()
{
	return m_allocator;
}

void CommandAllocator::Reset()
{
	m_allocator->Reset();
}

CommandAllocator* CommandAllocatorManager::GetCommandAllocator()
{
	int idleAllocatorIndex = -1;
	if (m_idleAllocatorIndexQueue.size() > 0)
	{
		idleAllocatorIndex = m_idleAllocatorIndexQueue.front();
		m_idleAllocatorIndexQueue.pop();
	}

	CommandAllocator* allocator = nullptr;
	if (idleAllocatorIndex != -1)
	{
		if (idleAllocatorIndex < m_commandAllocatorList.size())
		{
			allocator = m_commandAllocatorList[idleAllocatorIndex];
		}
	}

	if (allocator == nullptr)
	{
		allocator = new CommandAllocator();
		m_commandAllocatorList.push_back(allocator);
		uint32_t newAllocatorIndex = static_cast<uint32_t>(m_commandAllocatorList.size() - 1);
		m_bindTable.insert(std::make_pair(allocator->GetUID(), newAllocatorIndex));
	}

	return allocator;
}

void CommandAllocatorManager::ReleaseCommandAllocator(CommandAllocator* allocator)
{
	auto iterFind = m_bindTable.find(allocator->GetUID());
	if (iterFind != m_bindTable.end())
	{
		uint32_t index = iterFind->second;
		m_commandAllocatorList[index]->Reset();
		m_idleAllocatorIndexQueue.push(index);
	}
}

void CommandAllocatorManager::Destory()
{
	for (auto& cur : m_commandAllocatorList)
	{
		if (cur != nullptr)
		{
			delete cur;
			cur = nullptr;
		}
	}
}

void CommandBufferBase::FreeCommandBuffers()
{
	if (m_isInitialized)
	{
		m_commandBuffer->Release();
	}
}

bool CommandBufferBase::AllocateCommandBuffer()
{
	m_commandAllocator = CommandAllocatorManager::Instance().GetCommandAllocator();
	HRESULT res = gLogicalDevice->CreateCommandList
	(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_commandAllocator->GetAllocator(),
		nullptr,
		IID_PPV_ARGS(&m_commandBuffer)
	);
	m_commandBuffer->Close();

	return SUCCEEDED(res);
}

void CommandBufferBase::Reset()
{
	if (m_commandAllocator != nullptr)
	{
		CommandAllocatorManager::Instance().ReleaseCommandAllocator(m_commandAllocator);		
		m_commandAllocator = nullptr;
	}
}

bool CommandBufferBase::Begin()
{
	if (m_commandAllocator == nullptr)
	{
		m_commandAllocator = CommandAllocatorManager::Instance().GetCommandAllocator();
	}
	else
	{
		m_commandAllocator->Reset();
	}
	return SUCCEEDED(m_commandBuffer->Reset(m_commandAllocator->GetAllocator(), nullptr));
}

bool CommandBufferBase::Begin(ID3D12PipelineState* pso)
{
	if (m_commandAllocator == nullptr)
	{
		m_commandAllocator = CommandAllocatorManager::Instance().GetCommandAllocator();
	}
	else
	{
		m_commandAllocator->Reset();
	}
	return SUCCEEDED(m_commandBuffer->Reset(m_commandAllocator->GetAllocator(), pso));
}

bool CommandBufferBase::End()
{
	if (SUCCEEDED(m_commandBuffer->Close()))
	{
		return true;
	}
	return false;
}

bool CommandBuffer::Initialize()
{
	if (AllocateCommandBuffer())
	{
		m_isInitialized = true;
		return true;
	}
	return false;
}

bool CommandBuffer::Begin(ID3D12PipelineState* pso)
{
	return CommandBufferBase::Begin(pso);
}

bool CommandBuffer::End()
{
	m_state = ECommandBufferState::COMMAND_BUFFER_STATE_RECORDED;
	return CommandBufferBase::End();
}

bool StaticCommandBufferContainer::Initialize(uint32_t allocCount)
{
	m_commandList.resize(allocCount);
	for (auto& cur : m_commandList)
	{
		cur.Initialize();
	}

	return true;
}

void StaticCommandBufferContainer::Destory()
{
	Clear();
}

CommandBuffer* StaticCommandBufferContainer::GetCommandBuffer(uint32_t index)
{
	if (index < m_commandList.size())
	{
		return &m_commandList[index];
	}
	return nullptr;
}

void StaticCommandBufferContainer::Clear()
{
	for (auto& cur : m_commandList)
	{
		cur.FreeCommandBuffers();
	}
	m_commandList.clear();
}

bool DynamicCommandBufferContainer::Initialize()
{
	return true;
}

void DynamicCommandBufferContainer::Destory()
{
	Clear();
}

CommandBuffer* DynamicCommandBufferContainer::GetCommandBuffer()
{
	for (int i = 0; i < m_commandList.size(); i++)
	{
		if (m_commandList[i]->GetState() == CommandBuffer::ECommandBufferState::COMMAND_BUFFER_STATE_READY)
		{
			m_commandList[i]->Reset();
			return m_commandList[i];
		}
	}

	CommandBuffer* newCmdBuf = new CommandBuffer();
	m_commandList.push_back(newCmdBuf);
	m_commandList[m_commandList.size() - 1]->Initialize();
	return m_commandList[m_commandList.size() - 1];

	return nullptr;
}

CommandBuffer* DynamicCommandBufferContainer::GetCommandBuffer(uint32_t index)
{
	if (index < m_commandList.size())
	{
		if (m_commandList[index]->GetState() == CommandBuffer::ECommandBufferState::COMMAND_BUFFER_STATE_READY)
		{
			return m_commandList[index];
		}
		else
		{
			//인덱스에 해당하는 커맨드 버퍼가 준비되지 않았음을 로깅
		}
	}
	return nullptr;
}

void DynamicCommandBufferContainer::Clear()
{
	for (auto& cur : m_commandList)
	{
		cur->FreeCommandBuffers();
		delete cur;
	}
	m_commandList.clear();
}

bool SingleTimeCommandBuffer::Begin()
{
	if (AllocateCommandBuffer())
	{
		if (CommandBufferBase::Begin())
		{
			m_isInitialized = true;
		}
		else
		{
			REPORT(EReportType::REPORT_TYPE_ERROR, "Single time command buffer begin failed");
			return false;
		}
	}
	else
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Single time command buffer allocate failed");
		return false;
	}

	return true;
}

bool SingleTimeCommandBuffer::Begin(ID3D12PipelineState* pso)
{
	if (AllocateCommandBuffer())
	{
		if (CommandBufferBase::Begin(pso))
		{
			m_commandBuffer->SetName(L"single time command buffer");
			m_isInitialized = true;
		}
		else
		{
			REPORT(EReportType::REPORT_TYPE_ERROR, "Single time command buffer begin failed");
			return false;
		}
	}
	else
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Single time command buffer allocate failed");
		return false;
	}

	return true;
}

bool SingleTimeCommandBuffer::End()
{
	if (CommandBufferBase::End())
	{
		m_fence.Initialize(L"SCmdBufFence");
		gDX12DeviceRes->GetDefaultCommandQueue().Submit(this, &m_fence, false);
		m_fence.WaitForEnd();
		m_fence.Destory();
		
		FreeCommandBuffers();

		m_commandAllocator->Reset();
	}
	else
	{
		REPORT(EReportType::REPORT_TYPE_ERROR, "Single time command buffer end failed");
		return false;
	}
	return true;
}
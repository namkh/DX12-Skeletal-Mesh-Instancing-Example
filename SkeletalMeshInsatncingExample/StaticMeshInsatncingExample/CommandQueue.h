#pragma once

#include "Defaults.h"
#include "Utils.h"

class Fence;
class CommandBufferBase;

class CommandQueue
{
public:
	CommandQueue() {};

public:
	CommandQueue(const CommandQueue& rhs) = delete;
	CommandQueue& operator=(const CommandQueue& rhs) = delete;

public:
	bool Initialize(D3D12_COMMAND_QUEUE_DESC& queueDesc);
	void Submit(CommandBufferBase* cmdBuffer, Fence* fence, bool present);
	void Submit(std::vector<CommandBufferBase*>& cmdBuffers, Fence* fence, bool present);
	void Destroy();

	void WaitForAllFence();

	ID3D12CommandQueue* GetCommandQueue() { return m_commandQueue; }

	void OnFenceWaitEnded(UID uid);

private:
	ID3D12CommandQueue* m_commandQueue;
	std::map<UID, Fence*> m_waitingFences;
	uint64_t m_fenceValue = 0;
};


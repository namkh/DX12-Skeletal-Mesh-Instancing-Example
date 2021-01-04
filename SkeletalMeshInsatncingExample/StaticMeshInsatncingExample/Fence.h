#pragma once

#include <vector>
#include "Commands.h"
#include "Utils.h"
#include "CommandQueue.h"

class CommandBufferBase;
class CommandQueue;

class Fence : public UniqueIdentifier
{
public:
	Fence() {};

public:
	bool Initialize(const wchar_t* name);
	bool WaitForEnd(bool notifyEnd = true);
	void AddSubmittedCommandBuffer(CommandBufferBase* cmdBuffer);
	void AddSubmittedCommandBuffer(std::vector<CommandBufferBase*>& cmdBuffer);
	void Destory();

	void SetCurrentFenceIndex(uint64_t fenceIndex) { m_currentFenceIndex = fenceIndex; }
	ID3D12Fence* GetFence() { return m_fence; }

	CommandWithOneParam<CommandQueue, UID> NotifyWaitEnd;

public:
	
	uint64_t m_currentFenceIndex = 0;
	ID3D12Fence* m_fence = nullptr;
	std::vector<CommandBufferBase*> m_submittedCommandBuffer;
};


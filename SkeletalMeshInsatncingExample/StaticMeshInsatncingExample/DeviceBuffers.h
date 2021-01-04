#pragma once

#include "Defaults.h"
#include "DX12DeviceResourceAccess.h"
#include "CommandBuffers.h"

template <typename DataType>
class StructuredBuffer
{
public:
	StructuredBuffer();

public:
	StructuredBuffer(const StructuredBuffer& rhs) = delete;
	StructuredBuffer& operator=(const StructuredBuffer& rhs) = delete;

public:
	bool Initialize(UINT elementCount, bool isConstantBuffer, D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATES resourceState = D3D12_RESOURCE_STATE_COMMON);
	void Destroy();
	void UploadResource(CommandBufferBase* cmdBuffer, uint32_t elementIndex, const void* data);
	void UploadResource(CommandBufferBase* cmdBuffer, uint32_t pos, uint32_t uploadCount, const void* data);
	void UploadResource(CommandBufferBase* cmdBuffer, const void* data);

	void Begin(CommandBufferBase* cmdBuffer);
	void End(CommandBufferBase* cmdBuffer);

	uint32_t GetStride() { return m_stride; }
	ID3D12Resource* GetResource() { return m_resourceBuffer; }

private:

	ID3D12Resource* m_resourceBuffer = nullptr;
	ID3D12Resource* m_stagingBuffer = nullptr;

	D3D12_RESOURCE_FLAGS m_resourceFlags = D3D12_RESOURCE_FLAG_NONE;
	D3D12_RESOURCE_STATES m_resourceState = D3D12_RESOURCE_STATE_COMMON;

	uint32_t m_stride = 0;
	uint32_t m_count = 0;
	uint32_t m_byteSize = 0;
	bool m_isConstantBuffer = false;
};

template <typename DataType>
StructuredBuffer<DataType>::StructuredBuffer()
{

}


template <typename DataType>
bool StructuredBuffer<DataType>::Initialize(UINT count, bool isConstantBuffer, D3D12_RESOURCE_FLAGS resourceFlags, D3D12_RESOURCE_STATES resourceState)
{
	m_isConstantBuffer = isConstantBuffer;
	m_count = count;
	m_stride = sizeof(DataType);
	m_byteSize = m_stride * m_count;

	m_resourceFlags = resourceFlags;
	m_resourceState = resourceState;

	if (m_isConstantBuffer)
	{
		m_stride = (m_stride + 255) & ~255;
	}
		
	HRESULT res = gLogicalDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
														  D3D12_HEAP_FLAG_NONE,
														  &CD3DX12_RESOURCE_DESC::Buffer(m_stride * count, m_resourceFlags),
														  m_resourceState,
														  nullptr,
														  IID_PPV_ARGS(&m_resourceBuffer));
	if (FAILED(res))
	{
		return false;
	}
	m_resourceBuffer->SetName(L"sb resource buffer");
	

	res = gLogicalDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
												  D3D12_HEAP_FLAG_NONE,
												  &CD3DX12_RESOURCE_DESC::Buffer(m_stride * count),
												  D3D12_RESOURCE_STATE_GENERIC_READ,
												  nullptr,
												  IID_PPV_ARGS(&m_stagingBuffer));
	if (FAILED(res))
	{
		return false;
	}
	m_stagingBuffer->SetName(L"sb staging buffer");

	return true;
}

template <typename DataType>
void StructuredBuffer<DataType>::Destroy()
{
	if (m_resourceBuffer != nullptr)
	{
		m_resourceBuffer->Release();
	}
	
	if (m_resourceBuffer != nullptr)
	{
		m_stagingBuffer->Release();
	}
}

template <typename DataType>
void StructuredBuffer<DataType>::UploadResource(CommandBufferBase* cmdBuffer, uint32_t pos, uint32_t uploadCount, const void* data)
{
	D3D12_SUBRESOURCE_DATA subResourceData;
	subResourceData.pData = data;
	subResourceData.RowPitch = m_stride;
	subResourceData.SlicePitch = m_stride;

	uint8_t* memory = nullptr;
	if (SUCCEEDED(m_stagingBuffer->Map(0, nullptr, reinterpret_cast<void**>(&memory))))
	{
		std::memcpy(memory + m_stride * pos, data, uploadCount * m_stride);
		m_stagingBuffer->Unmap(0, nullptr);

		Begin(cmdBuffer);
		cmdBuffer->GetCommandBuffer()->CopyResource(m_resourceBuffer, m_stagingBuffer);
		End(cmdBuffer);
	}
}

template <typename DataType>
void StructuredBuffer<DataType>::UploadResource(CommandBufferBase* cmdBuffer, uint32_t elementIndex, const void* data)
{
	D3D12_SUBRESOURCE_DATA subResourceData;
	subResourceData.pData = data;
	subResourceData.RowPitch = m_stride;
	subResourceData.SlicePitch = m_stride;

	uint8_t* memory = nullptr;
	if (SUCCEEDED(m_stagingBuffer->Map(0, nullptr, reinterpret_cast<void**>(&memory))))
	{
		std::memcpy(memory + m_stride * elementIndex, data, m_stride);
		m_stagingBuffer->Unmap(0, nullptr);

		Begin(cmdBuffer);
		cmdBuffer->GetCommandBuffer()->CopyResource(m_resourceBuffer, m_stagingBuffer);
		End(cmdBuffer);
	}
}

template <typename DataType>
void StructuredBuffer<DataType>::UploadResource(CommandBufferBase* cmdBuffer, const void* data)
{
	D3D12_SUBRESOURCE_DATA subResourceData;
	subResourceData.pData = data;
	subResourceData.RowPitch = m_stride;
	subResourceData.SlicePitch = m_stride;
	uint8_t* memory = nullptr;
	if (SUCCEEDED(m_stagingBuffer->Map(0, nullptr, reinterpret_cast<void**>(&memory))))
	{
		std::memcpy(memory, data, m_byteSize);
		m_stagingBuffer->Unmap(0, nullptr);

		Begin(cmdBuffer);
		cmdBuffer->GetCommandBuffer()->CopyResource(m_resourceBuffer, m_stagingBuffer);
		End(cmdBuffer);
	}
}

template <typename DataType>
void StructuredBuffer<DataType>::Begin(CommandBufferBase* cmdBuffer)
{
	cmdBuffer->GetCommandBuffer()->ResourceBarrier
	(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition
		(
			m_resourceBuffer,
			m_resourceState,
			D3D12_RESOURCE_STATE_COPY_DEST
		)
	);
}

template <typename DataType>
void StructuredBuffer<DataType>::End(CommandBufferBase* cmdBuffer)
{
	cmdBuffer->GetCommandBuffer()->ResourceBarrier
	(
		1,
		&CD3DX12_RESOURCE_BARRIER::Transition
		(
			m_resourceBuffer,
			D3D12_RESOURCE_STATE_COPY_DEST,
			m_resourceState
		)
	);
}

class ReadOnlyDeviceBuffer
{
public:
	ReadOnlyDeviceBuffer();
	virtual ~ReadOnlyDeviceBuffer();

	bool Initialize(uint32_t count, uint32_t stride);
	void UploadResource(CommandBufferBase* cmdBufferBase, void* data);
	void Destroy();

	ID3D12Resource* GetResource() { return m_resourceBuffer; }
	uint32_t GetCount() { return m_count; }

protected:

	ID3D12Resource* m_resourceBuffer;
	ID3D12Resource* m_stagingBuffer;

	D3D12_RESOURCE_STATES m_resourceState = D3D12_RESOURCE_STATE_COMMON;

	uint32_t m_count	= 0;
	uint32_t m_stride	= 0;
	uint32_t m_byteSize	= 0;
};		

#define NUM_SKIN_WEIGHTS 4

struct StaticMeshVertex
{
	XMFLOAT3 m_position	= {};
	XMFLOAT3 m_normal	= {};
	XMFLOAT3 m_tangent	= {};
	XMFLOAT4 m_color	= {};
	XMFLOAT2 m_texcoord	= {};
};

struct SkinnedMeshVertex : public StaticMeshVertex
{
	uint32_t m_boneIndices[NUM_SKIN_WEIGHTS]	= {};
	float	 m_weights[NUM_SKIN_WEIGHTS]		= {};
};

class VertexBuffer : public ReadOnlyDeviceBuffer
{
public:
	VertexBuffer()
	{
		m_resourceState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	}

	virtual ~VertexBuffer()
	{

	}

	bool Initialize(CommandBufferBase* cmdBuffer, uint32_t count, uint32_t stride, void* data);

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView()
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = m_resourceBuffer->GetGPUVirtualAddress();
		vbv.StrideInBytes = m_stride;
		vbv.SizeInBytes = m_byteSize;

		return vbv;
	}
};

class IndexBuffer : public ReadOnlyDeviceBuffer
{
public:
	IndexBuffer()
	{
		m_resourceState = D3D12_RESOURCE_STATE_INDEX_BUFFER;
	}
	virtual ~IndexBuffer(){ }

	bool Initialize(CommandBufferBase* cmdBuffer, uint32_t count, uint32_t stride, void* data);
	
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView()
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = m_resourceBuffer->GetGPUVirtualAddress();
		ibv.Format = DXGI_FORMAT_R32_UINT;
		ibv.SizeInBytes = m_byteSize;

		return ibv;
	}
};

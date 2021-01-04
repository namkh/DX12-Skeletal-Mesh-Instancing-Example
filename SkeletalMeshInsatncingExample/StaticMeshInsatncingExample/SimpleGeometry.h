#pragma once

#include <functional>

#include "DeviceBuffers.h"
#include "FbxLoader.h"
#include "Singleton.h"
#include "SimpleSkeleton.h"

static const uint32_t MESHLET_MAX_VERTS = 64;
static const uint32_t MESHLET_MAX_PRIMS = 126;

//전체 인덱스 바인딩

struct MeshletData
{
	uint32_t VertexIndexCount = 0;
	uint32_t VertexIndexOffset = 0;
	uint32_t PrimitiveCount = 0;
	uint32_t PrimitiveOffset = 0;
};

struct CullData
{
	XMFLOAT3 m_direction;
	float m_seta;
	XMFLOAT3 m_centerPosition;
};

struct LocPrimIndex
{
	uint32_t index0 : 10;
	uint32_t index1 : 10;
	uint32_t index2 : 10;
};

class Meshlet
{
public:
	Meshlet(uint32_t vertIdxOffset, uint32_t primIdxOffset) : m_vertIdxOffset(vertIdxOffset), m_primIdxOffset(primIdxOffset)
	{

	}

public:

	bool LimitCheck(uint32_t* tris);
	void AddTriangle(uint32_t* tris);

	void GetMeshletData(MeshletData& meshletData);

	std::vector<uint32_t>& GetLocalVertexIndices() { return m_meshletLocalVertexIndices; };
	std::vector<LocPrimIndex>& GetLocalPrimitiveIndices() { return m_meshletLocalPrimitiveIndices; };

	uint32_t GetVertIdxCount() { return static_cast<uint32_t>(m_meshletLocalVertexIndices.size()); }
	uint32_t GetPrimCount() { return static_cast<uint32_t>(m_meshletLocalPrimitiveIndices.size()); }
	CullData& GetCullData() { return m_cullData; }

private:

	std::map<uint32_t, uint32_t> m_meshletIndicesMatchTable;
	std::vector<uint32_t> m_meshletLocalVertexIndices;
	std::vector<LocPrimIndex> m_meshletLocalPrimitiveIndices;

	CullData m_cullData = {};

	uint32_t m_vertIdxOffset = 0;
	uint32_t m_primIdxOffset = 0;
};

struct MeshletInfo
{
	uint32_t MeshletCount = 0;
	uint32_t LastMeshletVertIdxCount = 0;
	uint32_t LastMeshletPrimCount = 0;
};

class SimpleMeshData : public UniqueIdentifier
{
	friend class GeometryContainer;
public:
	~SimpleMeshData() {}
public:

	VertexBuffer& GetVertexBuffer() { return m_vertexBuffer; }
	IndexBuffer& GetIndexBuffer() { return m_indexBuffer; }
	
	IndexBuffer& GetVertLocIdxBuffer() { return m_locVertIndexBuffer; }
	StructuredBuffer<LocPrimIndex>& GetPrimLocIdxBuffer() { return m_locPrimIndexBuffer; }

	StructuredBuffer<MeshletData>& GetMeshletBuffer() { return m_meshletBuffer; }

	MeshletInfo& GetMeshletInfo() { return m_meshletInfo; }

	std::vector<D3D12_INPUT_ELEMENT_DESC>* GetInputLayout() { return& m_inputLayout; }

	bool HasMeshlet() { return m_hasMeshlet; }

protected:
	void Unload();

	bool CreateMeshlet(CommandBufferBase* cmdBuffer, std::vector<uint32_t>& indices);
	virtual void CreateInputLayout() = 0;

	void OnUpdated();

	LambdaCommandWithOneParam<std::function<void(uint32_t)>, uint32_t> OnMeshUpdated;

protected:
	VertexBuffer m_vertexBuffer = {};
	IndexBuffer m_indexBuffer = {};
	IndexBuffer m_locVertIndexBuffer = {};
	StructuredBuffer<LocPrimIndex> m_locPrimIndexBuffer = {};
	StructuredBuffer<MeshletData> m_meshletBuffer = {};
	MeshletInfo m_meshletInfo = {};

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout = {};

	bool m_hasMeshlet = false;
};

class StaticMeshData : public SimpleMeshData
{
	friend class GeometryContainer;
public:
	~StaticMeshData() {}
protected:
	bool Load(CommandBufferBase* cmdBuffer, FbxStaticMeshData& geometryData, bool genMeshlet = false);
	virtual void CreateInputLayout() override;
};

class SkeletalMeshData : public SimpleMeshData
{
	friend class GeometryContainer;
public:
	~SkeletalMeshData() {}
protected:
	bool Load(CommandBufferBase* cmdBuffer, FbxSkeletalMeshData& geometryData, bool genMeshlet = false);
	virtual void CreateInputLayout() override;
};

class SimpleGeometry : public RefCounter, public UniqueIdentifier
{
	friend class GeometryContainer;
public:
	void Destroy();

	void SetSrcFilePath(std::string& srcFilePath) { m_srcFilePath = srcFilePath; }
	std::string GetSrcFilePath() { return m_srcFilePath; }

	uint32_t GetMeshCount() { return static_cast<uint32_t>(m_meshList.size()); }
	SimpleMeshData* GetMesh(uint32_t index);

	std::vector<D3D12_INPUT_ELEMENT_DESC>* GetDefaultInputLayout();

protected:
	void Unload();

protected:

	std::vector<SimpleMeshData*> m_meshList = {};
	std::string m_srcFilePath = "";
};

class StaticMeshGeometry : public SimpleGeometry
{
	friend class GeometryContainer;
protected:
	bool Load(CommandBufferBase* cmdBuffer, std::string& fbxFilePath, bool genMeshlet = false);
};

//로드 함수 고치자... 
class SkeletalMeshGeometry : public SimpleGeometry
{
	friend class GeometryContainer;
protected:
	bool Load(CommandBufferBase* cmdBuffer, SimpleSkeleton* skeleton, std::string& fbxFilePath, bool genMeshlet = false);
};

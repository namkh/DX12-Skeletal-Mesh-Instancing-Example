#include "SimpleGeometry.h"
#include "GeometryContainer.h"

#include <map>

bool Meshlet::LimitCheck(uint32_t* tris)
{
	if (GetPrimCount() + 3 > MESHLET_MAX_PRIMS)
	{
		return false;
	}

	uint32_t numOfAddVerts = 0;
	for (uint8_t i = 0; i < 3; i++)
	{
		uint32_t localIndex = tris[i] - m_vertIdxOffset;
		auto iterFind = m_meshletIndicesMatchTable.find(localIndex);
		if (iterFind == m_meshletIndicesMatchTable.end())
		{
			numOfAddVerts++;
		}
	}
	if (static_cast<uint32_t>(m_meshletLocalVertexIndices.size()) + numOfAddVerts > MESHLET_MAX_VERTS)
	{
		return false;
	}
	return true;
}

void Meshlet::AddTriangle(uint32_t* tris)
{
	uint32_t primIdx[3] = { 0, 0, 0 };
	uint32_t locPrimIdx[3] = { 0, 0, 0 };
	for (uint8_t i = 0; i < 3; i++)
	{
		uint32_t vertIndex = tris[i];
		auto iterFind = m_meshletIndicesMatchTable.find(vertIndex);
		if (iterFind == m_meshletIndicesMatchTable.end())
		{
			m_meshletLocalVertexIndices.push_back(vertIndex);
			uint32_t addedIndex = static_cast<uint32_t>(m_meshletLocalVertexIndices.size() - 1);
			m_meshletIndicesMatchTable.insert(std::make_pair(vertIndex, addedIndex));
			locPrimIdx[i] = static_cast<uint32_t>(addedIndex);
		}
		else
		{
			locPrimIdx[i] = iterFind->second;
		}
	}
	LocPrimIndex data = { locPrimIdx[0], locPrimIdx[1], locPrimIdx[2] };
	m_meshletLocalPrimitiveIndices.push_back(data);
}

void Meshlet::GetMeshletData(MeshletData& meshletData)
{
	meshletData.VertexIndexCount = static_cast<uint32_t>(m_meshletLocalVertexIndices.size());
	meshletData.VertexIndexOffset = m_vertIdxOffset;
	meshletData.PrimitiveCount = static_cast<uint32_t>(m_meshletLocalPrimitiveIndices.size());
	meshletData.PrimitiveOffset = m_primIdxOffset;
}

bool SimpleMeshData::CreateMeshlet(CommandBufferBase* cmdBuffer, std::vector<uint32_t>& indices)
{
	uint32_t vertLocIdxOffset = 0;
	uint32_t primOffset = 0;

	std::vector<Meshlet*> meshlets;
	Meshlet* curMeshlet = new Meshlet(vertLocIdxOffset, primOffset);
	meshlets.push_back(curMeshlet);

	uint32_t tris[3] = { 0, 0, 0 };
	for (uint32_t i = 0; i < indices.size(); i += 3)
	{
		tris[0] = indices[i];
		tris[1] = indices[i + 1];
		tris[2] = indices[i + 2];
		if (!curMeshlet->LimitCheck(tris))
		{
			vertLocIdxOffset += curMeshlet->GetVertIdxCount();
			primOffset += curMeshlet->GetPrimCount();
			curMeshlet = new Meshlet(vertLocIdxOffset, primOffset);
			meshlets.push_back(curMeshlet);
		}
		curMeshlet->AddTriangle(tris);
	}

	Meshlet* lastMeshlet = meshlets[meshlets.size() - 1];
	m_meshletInfo.MeshletCount = static_cast<uint32_t>(meshlets.size());
	m_meshletInfo.LastMeshletVertIdxCount = lastMeshlet->GetVertIdxCount();
	m_meshletInfo.LastMeshletPrimCount = lastMeshlet->GetPrimCount();

	std::vector<uint32_t> vertIdxList;
	std::vector<LocPrimIndex> primIdxList;
	std::vector<MeshletData> meshletDatas(meshlets.size());
	for (uint32_t i = 0; i < meshletDatas.size(); i++)
	{
		meshlets[i]->GetMeshletData(meshletDatas[i]);

		std::vector<uint32_t>& curMeshletVertIdxs = meshlets[i]->GetLocalVertexIndices();
		std::vector<LocPrimIndex>& curMeshletPrimIdxs = meshlets[i]->GetLocalPrimitiveIndices();

		vertIdxList.insert(vertIdxList.end(), curMeshletVertIdxs.begin(), curMeshletVertIdxs.end());
		primIdxList.insert(primIdxList.end(), curMeshletPrimIdxs.begin(), curMeshletPrimIdxs.end());
	}

	if (!m_locVertIndexBuffer.Initialize(cmdBuffer, static_cast<uint32_t>(vertIdxList.size()), sizeof(uint32_t), reinterpret_cast<void*>(vertIdxList.data())))
	{
		return false;
	}

	if (!m_locPrimIndexBuffer.Initialize(static_cast<uint32_t>(primIdxList.size()), false))
	{
		return false;
	}
	m_locPrimIndexBuffer.UploadResource(cmdBuffer, static_cast<void*>(primIdxList.data()));

	if (!m_meshletBuffer.Initialize(static_cast<uint32_t>(meshletDatas.size()), false))
	{
		return false;
	}
	m_meshletBuffer.UploadResource(cmdBuffer, static_cast<void*>(meshletDatas.data()));

	m_hasMeshlet = true;

	for (auto& cur : meshlets)
	{
		if (cur != nullptr)
		{
			delete cur;
		}
	}
	meshlets.clear();

	return true;
}

void SimpleMeshData::CreateInputLayout()
{
	
}

void SimpleMeshData::Unload()
{
	gDX12DeviceRes->WaitForCommandQueue();

	m_vertexBuffer.Destroy();
	m_indexBuffer.Destroy();
	if (m_hasMeshlet)
	{
		m_locVertIndexBuffer.Destroy();
		m_locPrimIndexBuffer.Destroy();
		m_meshletBuffer.Destroy();
	}
}

void SimpleMeshData::OnUpdated()
{
	if (OnMeshUpdated.IsBinded())
	{
		int index = gGeomContainer.GetMeshBindIndex(this);
		if (index != -1)
		{
			OnMeshUpdated.Exec(static_cast<uint32_t>(index));
		}
	}
}

bool StaticMeshData::Load(CommandBufferBase* cmdBuffer, FbxStaticMeshData& geometryData, bool genMeshlet)
{
	std::vector<StaticMeshVertex> verts(geometryData.m_positions.size());
	std::vector<uint32_t> indices(geometryData.m_indices);

	for (int i = 0; i < geometryData.m_positions.size(); i++)
	{
		verts[i].m_position = geometryData.m_positions[i];

		if (geometryData.m_normals.size() != 0)
		{
			verts[i].m_normal = geometryData.m_normals[i];
		}
		if (geometryData.m_tangents.size() != 0)
		{
			verts[i].m_tangent = geometryData.m_tangents[i];
		}
		if (geometryData.m_color.size() != 0)
		{
			verts[i].m_color = geometryData.m_color[i];
		}
		if (geometryData.m_uv.size() != 0)
		{
			verts[i].m_texcoord = geometryData.m_uv[i];
		}
	}

	if (genMeshlet)
	{
		CreateMeshlet(cmdBuffer, indices);
	}

	if (!m_vertexBuffer.Initialize(cmdBuffer, static_cast<uint32_t>(verts.size()), sizeof(StaticMeshVertex), reinterpret_cast<void*>(verts.data())))
	{
		return false;
	}
	if (!m_indexBuffer.Initialize(cmdBuffer, static_cast<uint32_t>(indices.size()), sizeof(uint32_t), reinterpret_cast<void*>(indices.data())))
	{
		return false;
	}

	CreateInputLayout();

	return true;
}

void StaticMeshData::CreateInputLayout()
{
	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

bool SkeletalMeshData::Load(CommandBufferBase* cmdBuffer, FbxSkeletalMeshData& geometryData, bool genMeshlet)
{
	std::vector<SkinnedMeshVertex> verts(geometryData.m_positions.size());
	std::vector<uint32_t> indices(geometryData.m_indices.size());
	for (uint32_t i = 0; i < geometryData.m_indices.size(); i+=3)
	{
		indices[i] = geometryData.m_indices[i];
		indices[i + 1] = geometryData.m_indices[i + 2];
		indices[i + 2] = geometryData.m_indices[i + 1];
	}

	for (int i = 0; i < geometryData.m_positions.size(); i++)
	{
		verts[i].m_position = geometryData.m_positions[i];

		if (geometryData.m_normals.size() != 0)
		{
			verts[i].m_normal = geometryData.m_normals[i];
		}
		if (geometryData.m_tangents.size() != 0)
		{
			verts[i].m_tangent = geometryData.m_tangents[i];
		}
		if (geometryData.m_color.size() != 0)
		{
			verts[i].m_color = geometryData.m_color[i];
		}
		if (geometryData.m_uv.size() != 0)
		{
			verts[i].m_texcoord = geometryData.m_uv[i];
		}

		for (uint32_t j = 0; j < 4; j++)
		{
			if (j < geometryData.m_boneIndices[i].size())
			{
				verts[i].m_boneIndices[j] = geometryData.m_boneIndices[i][j];
				if (verts[i].m_boneIndices[j] > 40)
				{
					REPORT(EReportType::REPORT_TYPE_LOG, "??");
				}
			}
			else
			{
				verts[i].m_boneIndices[j] = 0;
			}

			if (j < geometryData.m_weights[i].size())
			{
				verts[i].m_weights[j] = geometryData.m_weights[i][j];
			}
			else
			{
				verts[i].m_weights[j] = 0.0f;
			}
		}
	}

	if (genMeshlet)
	{
		CreateMeshlet(cmdBuffer, indices);
	}

	if (!m_vertexBuffer.Initialize(cmdBuffer, static_cast<uint32_t>(verts.size()), sizeof(SkinnedMeshVertex), reinterpret_cast<void*>(verts.data())))
	{
		return false;
	}
	if (!m_indexBuffer.Initialize(cmdBuffer, static_cast<uint32_t>(indices.size()), sizeof(uint32_t), reinterpret_cast<void*>(indices.data())))
	{
		return false;
	}

	CreateInputLayout();

	return true;
}

void SkeletalMeshData::CreateInputLayout()
{
	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 76, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

void SimpleGeometry::Destroy()
{
	DecRef();
}

SimpleMeshData* SimpleGeometry::GetMesh(uint32_t index)
{
	if (index < m_meshList.size())
	{
		return m_meshList[index];
	}
	return nullptr;
}

std::vector<D3D12_INPUT_ELEMENT_DESC>* SimpleGeometry::GetDefaultInputLayout()
{
	SimpleMeshData* firstMeshData = GetMesh(0);
	if (firstMeshData != nullptr)
	{
		return firstMeshData->GetInputLayout();
	}
	return nullptr;
}

void SimpleGeometry::Unload()
{
	for (auto& cur : m_meshList)
	{
		gGeomContainer.UnloadMesh(cur);
	}
	m_meshList.clear();
}

bool StaticMeshGeometry::Load(CommandBufferBase* cmdBuffer, std::string& fbxFilePath, bool genMeshlet)
{
	std::vector<FbxStaticMeshData> geomDatas;
	gFbxLoader.LoadStaticMeshGeometry(fbxFilePath, geomDatas);

	for (auto& cur : geomDatas)
	{
		StaticMeshData* meshData = gGeomContainer.LoadStaticMesh(cmdBuffer, cur, genMeshlet);
		m_meshList.push_back(meshData);
	}
	m_srcFilePath = fbxFilePath;
	SetSrcFilePath(fbxFilePath);

	return true;
}

bool SkeletalMeshGeometry::Load(CommandBufferBase* cmdBuffer, SimpleSkeleton* skeleton, std::string& fbxFilePath, bool genMeshlet)
{
	std::vector<FbxSkeletalMeshData> geomDatas;
	gFbxLoader.LoadSkeletalMeshGeometry(fbxFilePath, skeleton, geomDatas);

	for (auto& cur : geomDatas)
	{
		SkeletalMeshData* meshData = gGeomContainer.LoadSkeletalMesh(cmdBuffer, cur, genMeshlet);
		m_meshList.push_back(meshData);
	}
	m_srcFilePath = fbxFilePath;
	SetSrcFilePath(fbxFilePath);

	return true;
}
#include "RenderObject.h"
#include "RenderObjectContainer.h"
#include "SkeletonContainer.h"

void RenderObjectInstancePerMesh::Initialize(RenderObjectInstance* parentInstance, SimpleMeshData* meshData, SimpleMaterial* material)
{
	m_meshData = meshData;
	m_material = material;
	m_parentInstance = parentInstance;
}

void RenderObjectInstancePerMesh::Destroy()
{
	if (m_overrideMaterial != nullptr)
	{
		gMaterialContainer.RemoveMaterial(m_overrideMaterial);
		m_overrideMaterial = nullptr;
	}
}

void RenderObjectInstancePerMesh::SetOverrideMaterial(CommandBufferBase* cmdBuffer, ExampleMaterialType type)
{
	m_overrideMaterial = gMaterialContainer.CreateMaterial(cmdBuffer, type);
}

SimpleMeshData* RenderObjectInstancePerMesh::GetMeshData()
{
	return m_meshData;
}

SimpleMaterial* RenderObjectInstancePerMesh::GetMaterial()
{
	if (m_overrideMaterial != nullptr)
	{
		return m_overrideMaterial;
	}
	return m_material;
}


void RenderObjectInstance::Initialzie(XMMATRIX& worldMat)
{
	m_worldMatrix = worldMat;
}

void RenderObjectInstance::Destroy()
{
	OnInstanceUpdated.Clear();
	for (auto& cur : m_instancePerMesh)
	{
		gRenderObjContainer.RemoveRenderObjectInstancePerMesh(cur);
	}
}

void RenderObjectInstance::CreateInstancePerMesh(SimpleMeshData* meshData, SimpleMaterial* material)
{	
	m_instancePerMesh.push_back(gRenderObjContainer.CreateRenderObjectInstancePerMesh(this, meshData, material));
}

void RenderObjectInstance::SetWorldMatrix(XMMATRIX& matWorld, bool isUpdate)
{
	m_worldMatrix = matWorld;
	if (isUpdate)
	{
		for (auto& cur : m_instancePerMesh)
		{
			int index = gRenderObjContainer.GetRenderObjectInstancePerMeshBindIndex(cur);
			if (index != INVALID_INDEX_INT)
			{
				OnInstanceUpdated.Exec(index);
			}
		}	
	}
}

void RenderObjectInstance::SetOverrideMaterial(CommandBufferBase* cmdBuffer, ExampleMaterialType type)
{
	for (auto& cur : m_instancePerMesh)
	{
		cur->SetOverrideMaterial(cmdBuffer, type);
	}

	//서브셋에 대한 머트리얼 처리 미구현, 0번 머트리얼을 세팅해 놓는다
	if (m_instancePerMesh.size() > 0)
	{
		m_overrideMaterial = m_instancePerMesh[0]->GetMaterial();
	}
}

void SkeletalMeshRenderObjectInstance::Initialzie(XMMATRIX& worldMat, SimpleSkeleton* skeleton)
{
	m_skeleton = skeleton;
	RenderObjectInstance::Initialzie(worldMat);
}

void SkeletalMeshRenderObjectInstance::Update(float timeDelta)
{
	m_animation.Update(timeDelta);
}

RenderObjectInstancePerMesh* RenderObjectInstance::GetInstancePerMesh(uint32_t index)
{
	if (index < m_instancePerMesh.size())
	{
		return m_instancePerMesh[index];
	}
	return nullptr;
}

void RenderObject::Destroy()
{
	for (uint32_t i = 0; i < m_instances.size(); i++)
	{
		gRenderObjContainer.RemoveRenderObjectInstance(m_instances[i]);
	}
	if (m_geometry != nullptr)
	{
		m_geometry->Destroy();
		m_geometry = nullptr;
	}
	if (m_material != nullptr)
	{
		gMaterialContainer.RemoveMaterial(m_material);
	}
}

RenderObjectInstance* RenderObject::CreateInstance(XMMATRIX& matWorld)
{
	RenderObjectInstance* inst = nullptr;

	if (m_geometry != nullptr && m_material != nullptr)
	{
		inst = CreateRenderObjectInstance(matWorld);
		m_instances.push_back(inst);
		m_indexTable.insert(std::make_pair(inst->GetUID(), static_cast<uint32_t>(m_instances.size()) - 1));
		int instancePerMeshIndex = -1;
		for (uint32_t i = 0; i < m_geometry->GetMeshCount(); i++)
		{
			SimpleMeshData* meshData = m_geometry->GetMesh(i);
			if (meshData != nullptr)
			{
				inst->CreateInstancePerMesh(meshData, m_material);
			}
		}
	}
	return inst;
}
	
void RenderObject::RemoveInstance(RenderObjectInstance* subMeshInstance)
{
	auto iterFind = m_indexTable.find(subMeshInstance->GetUID());
	if (iterFind != m_indexTable.end())
	{
		uint32_t index = iterFind->second;
		m_indexTable.erase(iterFind);
		m_instances.erase(m_instances.begin() + index);
	}
	gRenderObjContainer.RemoveRenderObjectInstance(subMeshInstance);
}

RenderObjectInstance* RenderObject::GetInstance(uint32_t index)
{
	if (index < m_instances.size())
	{
		return m_instances[index];
	}
	return nullptr;
}

void RenderObject::CollectMaterials(std::vector<SimpleMaterial*>& buffer)
{
	std::map<UID, SimpleMaterial*> m_matMap;
	if (m_material != nullptr)
	{
		m_matMap.insert(std::make_pair(m_material->GetUID(), m_material));
	}
	for (auto& cur : m_instances)
	{		
		if (cur != nullptr && cur->HasOverrideMaterial())
		{
			SimpleMaterial* curMat = cur->GetOverrideMaterial();
			if (curMat != nullptr)
			{
				auto iterFind = m_matMap.find(curMat->GetUID());
				if (iterFind == m_matMap.end())
				{
					m_matMap.insert(std::make_pair(curMat->GetUID(), curMat));
				}
			}
		}
	}

	for (auto& cur : m_matMap)
	{
		buffer.push_back(cur.second);
	}
}

void StaticMeshRenderObject::Initialize(CommandBufferBase* cmdBuffer, std::string fbxFilePath, ExampleMaterialType exampleMaterialType, bool useMeshShader)
{
	if (m_geometry != nullptr)
	{
		m_geometry->Destroy();
		m_geometry = nullptr;
	}
	m_geometry = gGeomContainer.CreateStaticMeshGeometry(cmdBuffer, fbxFilePath, useMeshShader);

	if (m_material != nullptr)
	{
		gMaterialContainer.RemoveMaterial(m_material);
	}
	m_material = gMaterialContainer.CreateMaterial(cmdBuffer, exampleMaterialType);
}

RenderObjectInstance* StaticMeshRenderObject::CreateRenderObjectInstance(XMMATRIX& matWorld)
{
	return gRenderObjContainer.CreateStaticMeshRenderObjectInstance(matWorld);
}

void SkeletalMeshRenderObject::Initialize(CommandBufferBase* cmdBuffer, std::string fbxFilePath, ExampleMaterialType exampleMaterialType, bool useMeshShader)
{
	if (m_skelton != nullptr)
	{
		m_skelton->Destroy();
		m_skelton = nullptr;
	}
	m_skelton = gSkelContainer.CreateSkeleton(fbxFilePath);

	if (m_geometry != nullptr)
	{
		m_geometry->Destroy();
		m_geometry = nullptr;
	}
	m_geometry = gGeomContainer.CreateSkeletalMeshGeometry(cmdBuffer, m_skelton, fbxFilePath, useMeshShader);

	if (m_material != nullptr)
	{
		gMaterialContainer.RemoveMaterial(m_material);
	}
	m_material = gMaterialContainer.CreateMaterial(cmdBuffer, exampleMaterialType);
}

void SkeletalMeshRenderObject::Destroy()
{
	RenderObject::Destroy();

	if (m_skelton != nullptr)
	{
		m_skelton->Destroy();
		m_skelton = nullptr;
	}
}

RenderObjectInstance* SkeletalMeshRenderObject::CreateRenderObjectInstance(XMMATRIX& matWorld)
{
	return gRenderObjContainer.CreateSkeletalMeshRenderObjectInstance(matWorld, this);
}
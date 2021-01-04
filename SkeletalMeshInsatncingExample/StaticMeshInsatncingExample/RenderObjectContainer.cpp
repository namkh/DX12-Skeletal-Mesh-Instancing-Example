#include "RenderObjectContainer.h"

RenderObject* RenderObjectContainer::CreateStaticMeshRenderObject(CommandBufferBase* cmdBuffer, std::string fbxFilePath, ExampleMaterialType exampleMaterialType, bool useMeshShader)
{
	RenderObject* obj = Create<StaticMeshRenderObject, std::unordered_map<UID, RenderObject*> >(&m_renderObjList);
	obj->Initialize(cmdBuffer, fbxFilePath, exampleMaterialType, useMeshShader);
	return obj;
}

RenderObject* RenderObjectContainer::CreateSkeletalMeshRenderObject(CommandBufferBase* cmdBuffer, std::string fbxFilePath, ExampleMaterialType exampleMaterialType, bool useMeshShader)
{
	RenderObject* obj = Create<SkeletalMeshRenderObject, std::unordered_map<UID, RenderObject*> >(&m_renderObjList);
	obj->Initialize(cmdBuffer, fbxFilePath, exampleMaterialType, useMeshShader);
	return obj;
}

void RenderObjectContainer::RemoveRenderObject(RenderObject* obj)
{
	Remove(obj, &m_renderObjList);
}

RenderObjectInstance* RenderObjectContainer::CreateStaticMeshRenderObjectInstance(XMMATRIX& matWorld)
{
	RenderObjectInstance* inst = Create<StaticMeshRenderObjectInstance, std::unordered_map<UID, RenderObjectInstance*> >(&m_instList);
	inst->Initialzie(matWorld);
	
	OnInstPerMeshAdded.Exec(static_cast<uint32_t>(m_instPerMeshList.size() - 1));

	return inst;
}

RenderObjectInstance* RenderObjectContainer::CreateSkeletalMeshRenderObjectInstance(XMMATRIX& matWorld, SkeletalMeshRenderObject* parent)
{
	RenderObjectInstance* inst = Create<SkeletalMeshRenderObjectInstance, std::unordered_map<UID, RenderObjectInstance*> >(&m_instList);
	SkeletalMeshRenderObjectInstance* skelMeshInst = dynamic_cast<SkeletalMeshRenderObjectInstance*>(inst);
	skelMeshInst->Initialzie(matWorld, parent->GetSkeleton());

	OnInstPerMeshAdded.Exec(static_cast<uint32_t>(m_instPerMeshList.size() - 1));

	return inst;
}

void RenderObjectContainer::RemoveRenderObjectInstance(RenderObjectInstance* instance)
{
	Remove(instance, &m_instList);
}

RenderObjectInstancePerMesh* RenderObjectContainer::CreateRenderObjectInstancePerMesh(RenderObjectInstance* parentInst, SimpleMeshData* meshData, SimpleMaterial* material)
{
	RenderObjectInstancePerMesh* instPerMesh = Create<RenderObjectInstancePerMesh, std::unordered_map<UID, RenderObjectInstancePerMesh*> >(&m_instPerMeshList);
	instPerMesh->Initialize(parentInst, meshData, material);
	
	OnInstPerMeshAdded.Exec();

	return instPerMesh;
}

void RenderObjectContainer::RemoveRenderObjectInstancePerMesh(RenderObjectInstancePerMesh* instancePreMesh)
{
	int removeIndex = GetRenderObjectInstancePerMeshBindIndex(instancePreMesh);
	if (removeIndex != -1)
	{
		Remove(instancePreMesh, &m_instPerMeshList);

		OnInstPerMeshRemoved.Exec(static_cast<uint32_t>(removeIndex));
	}
}

void RenderObjectContainer::Clear()
{
	for (auto& cur : m_renderObjList)
	{
		if (cur.second != nullptr)
		{
			cur.second->Destroy();
			delete cur.second;
		}
	}
	m_renderObjList.clear();
	m_instList.clear();
	m_instPerMeshList.clear();
}

uint32_t RenderObjectContainer::GetRenderObjectCount()
{
	return static_cast<uint32_t>(m_renderObjList.size());
}

RenderObject* RenderObjectContainer::GetRenderObject(uint32_t index)
{
	if (index < m_renderObjList.size())
	{
		auto pos = m_renderObjList.begin();
		std::advance(pos, index);
		return pos->second;
	}
	return nullptr;
}

int RenderObjectContainer::GetRenderObjectBindIndex(RenderObject* renderObj)
{
	if (renderObj != nullptr)
	{
		auto iterFind = m_instList.find(renderObj->GetUID());
		if (iterFind != m_instList.end())
		{
			return static_cast<int>(std::distance(m_instList.begin(), iterFind));
		}
	}
	return INVALID_INDEX_INT;
}

uint32_t RenderObjectContainer::GetRenderObjectInstanceCount()
{
	return static_cast<uint32_t>(m_instList.size());
}

RenderObjectInstance* RenderObjectContainer::GetRenderObjectInstance(uint32_t index)
{
	if (index < m_instList.size())
	{
		auto pos = m_instList.begin();
		std::advance(pos, index);
		return pos->second;
	}
	return nullptr;
}

int RenderObjectContainer::GetRenderObjectInstanceBindIndex(RenderObjectInstance* inst)
{
	if (inst != nullptr)
	{
		auto iterFind = m_instList.find(inst->GetUID());
		if (iterFind != m_instList.end())
		{
			return static_cast<int>(std::distance(m_instList.begin(), iterFind));
		}
	}
	return INVALID_INDEX_INT;
}

uint32_t RenderObjectContainer::GetRenderObjectInstancePerMeshCount()
{
	return static_cast<uint32_t>(m_instPerMeshList.size());
}

RenderObjectInstancePerMesh* RenderObjectContainer::GetRenderObjectInstancePerMesh(uint32_t index)
{
	if (index < m_instPerMeshList.size())
	{
		auto pos = m_instPerMeshList.begin();
		std::advance(pos, index);
		return pos->second;
	}
	return nullptr;
}

int RenderObjectContainer::GetRenderObjectInstancePerMeshBindIndex(RenderObjectInstancePerMesh* instPerMesh)
{
	if (instPerMesh != nullptr)
	{
		auto iterFind = m_instPerMeshList.find(instPerMesh->GetUID());
		if (iterFind != m_instPerMeshList.end())
		{
			return static_cast<int>(std::distance(m_instPerMeshList.begin(), iterFind));
		}
	}
	return INVALID_INDEX_INT;
}
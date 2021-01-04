#pragma once

#include "RenderObject.h"
#include "Singleton.h"
#include <unordered_map>

class RenderObjectContainer : public TSingleton<RenderObjectContainer>
{
public:
	RenderObjectContainer(token) 
	{
	};

public:
	RenderObject* CreateStaticMeshRenderObject(CommandBufferBase* cmdBuffer, std::string fbxFilePath, ExampleMaterialType exampleMaterialType, bool useMeshShader = false);
	RenderObject* CreateSkeletalMeshRenderObject(CommandBufferBase* cmdBuffer, std::string fbxFilePath, ExampleMaterialType exampleMaterialType, bool useMeshShader = false);
	void RemoveRenderObject(RenderObject* obj);

	RenderObjectInstance* CreateStaticMeshRenderObjectInstance(XMMATRIX& matWorld);
	RenderObjectInstance* CreateSkeletalMeshRenderObjectInstance(XMMATRIX& matWorld, SkeletalMeshRenderObject* parent);
	void RemoveRenderObjectInstance(RenderObjectInstance* instance);

	RenderObjectInstancePerMesh* CreateRenderObjectInstancePerMesh(RenderObjectInstance* parentInst, SimpleMeshData* meshData, SimpleMaterial* material);
	void RemoveRenderObjectInstancePerMesh(RenderObjectInstancePerMesh* instancePreMesh);

	void Clear();

public:

	uint32_t GetRenderObjectCount();
	RenderObject* GetRenderObject(uint32_t index);
	int GetRenderObjectBindIndex(RenderObject* renderObj);

	uint32_t GetRenderObjectInstanceCount();
	RenderObjectInstance* GetRenderObjectInstance(uint32_t index);
	int GetRenderObjectInstanceBindIndex(RenderObjectInstance* inst);

	uint32_t GetRenderObjectInstancePerMeshCount();
	RenderObjectInstancePerMesh* GetRenderObjectInstancePerMesh(uint32_t index);
	int GetRenderObjectInstancePerMeshBindIndex(RenderObjectInstancePerMesh* instPerMesh);

	bool IsDirty() { return m_isDirty; }
	void SetDirty(bool isDirty) { m_isDirty = true; }

	LambdaCommandListWithOneParam<std::function<void(uint32_t)>, uint32_t> OnInstPerMeshAdded;
	LambdaCommandListWithOneParam<std::function<void(uint32_t)>, uint32_t> OnInstPerMeshRemoved;

protected:

	template <typename CreateItemType, typename ListType>
	CreateItemType* Create(ListType* idxList);

	template <typename RemoveItemType, typename ListType>
	void Remove(RemoveItemType* item, ListType* idxList);

private:

	std::unordered_map<UID, RenderObject*> m_renderObjList;
	std::unordered_map<UID, RenderObjectInstance*> m_instList;
	std::unordered_map<UID, RenderObjectInstancePerMesh*> m_instPerMeshList;

	bool m_isDirty = false;
};

template <typename CreateItemType, typename ListType>
CreateItemType* RenderObjectContainer::Create(ListType* itemList)
{
	CreateItemType* item = new CreateItemType();
	itemList->insert(std::make_pair(item->GetUID(), item));
	m_isDirty = true;
	return item;
}

template <typename RemoveItemType, typename ListType>
void RenderObjectContainer::Remove(RemoveItemType* item, ListType* itemList)
{
	auto iterIdxFind = itemList->find(item->GetUID());
	if (iterIdxFind != itemList->end())
	{
		if (iterIdxFind->second != nullptr)
		{
			iterIdxFind->second->Destroy();
			delete iterIdxFind->second;
			itemList->erase(iterIdxFind);
			m_isDirty = true;
		}
	}
}
#define gRenderObjContainer RenderObjectContainer::Instance()
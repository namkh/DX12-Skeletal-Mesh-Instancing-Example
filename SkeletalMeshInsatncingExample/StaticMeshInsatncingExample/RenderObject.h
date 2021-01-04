#pragma once

#include "GeometryContainer.h"
#include "MaterialContainer.h"
#include "Utils.h"

#include "SimpleSkeleton.h"
#include "SkeletalAnimation.h"

class RenderObjectInstance;

class RenderObjectInstancePerMesh : public UniqueIdentifier
{
friend class RenderObjectContainer;
public:
	RenderObjectInstancePerMesh() : UniqueIdentifier() {}
public:
	void SetOverrideMaterial(CommandBufferBase* cmdBuffer, ExampleMaterialType type);

	SimpleMeshData* GetMeshData();
	SimpleMaterial* GetMaterial();
	RenderObjectInstance* GetParentInstance() { return m_parentInstance; }

	bool HasOverridMaterial() { return m_overrideMaterial != nullptr; }

protected:
	void Initialize(RenderObjectInstance* parentInstance,  SimpleMeshData* m_meshData, SimpleMaterial* m_material);
	void Destroy();

protected:
	SimpleMeshData* m_meshData = nullptr;
	SimpleMaterial* m_material = nullptr;
	SimpleMaterial* m_overrideMaterial = nullptr;
	RenderObjectInstance* m_parentInstance = nullptr;
};

class RenderObjectInstance : public UniqueIdentifier
{
friend class RenderObjectContainer;
public:
	RenderObjectInstance() : UniqueIdentifier() {}
	virtual ~RenderObjectInstance(){}
public:
	virtual void Update(float timeDelta) = 0;
	void SetOverrideMaterial(CommandBufferBase* cmdBuffer, ExampleMaterialType type);

	uint32_t GetInstancePerMeshCount() { return static_cast<uint32_t>(m_instancePerMesh.size()); }

	RenderObjectInstancePerMesh* GetInstancePerMesh(uint32_t index);
	void CreateInstancePerMesh(SimpleMeshData* meshData, SimpleMaterial* material);

	void SetWorldMatrix(XMMATRIX& matWorld, bool isUpdate);
	XMMATRIX& GetWorldMatrix() { return m_worldMatrix; }

	bool HasOverrideMaterial() { return m_overrideMaterial != nullptr; }
	SimpleMaterial* GetOverrideMaterial() { return m_overrideMaterial; }

	LambdaCommandListWithOneParam<std::function<void(uint32_t)>, uint32_t> OnInstanceUpdated;

protected:
	void Initialzie(XMMATRIX& worldMat);
	void Destroy();

private:
	XMMATRIX m_worldMatrix = {};
	std::vector<RenderObjectInstancePerMesh*> m_instancePerMesh = {};
	SimpleMaterial* m_overrideMaterial = nullptr;
};

class StaticMeshRenderObjectInstance : public RenderObjectInstance
{
	friend class RenderObjectContainer;
public:
	StaticMeshRenderObjectInstance() : RenderObjectInstance() {}
	virtual ~StaticMeshRenderObjectInstance() {}
	virtual void Update(float timeDelta) override {};
};

class SkeletalMeshRenderObjectInstance : public RenderObjectInstance
{
	friend class RenderObjectContainer;
public:
	SkeletalMeshRenderObjectInstance() : RenderObjectInstance() {}
	virtual void Update(float timeDelta) override;
	
	SkeletalAnimation* GetAnimation() { return &m_animation; }
	SimpleSkeleton* GetSkeleton() { return m_skeleton; }

protected:
	void Initialzie(XMMATRIX& worldMat, SimpleSkeleton* skeleton);

private:
	SimpleSkeleton* m_skeleton = nullptr;
	SkeletalAnimation m_animation;
};

class RenderObject : public UniqueIdentifier
{
friend class RenderObjectContainer;
public:
	RenderObject() : UniqueIdentifier() {}
public:
	RenderObjectInstance* CreateInstance(XMMATRIX& matWorld);
	void RemoveInstance(RenderObjectInstance* subMeshInstance);

	uint32_t GetInstanceCount() { return static_cast<uint32_t>(m_instances.size()); } 
	RenderObjectInstance* GetInstance(uint32_t index);

	SimpleGeometry* GetGeometry() { return m_geometry; }
	SimpleMaterial* GetMaterial() { return m_material; }
	
	void CollectMaterials(std::vector<SimpleMaterial*>& buffer);

protected:
	virtual void Initialize(CommandBufferBase* cmdBuffer, std::string fbxFilePath, ExampleMaterialType exampleMaterialType, bool useMeshShader = false) = 0;
	virtual void Destroy();

	virtual RenderObjectInstance* CreateRenderObjectInstance(XMMATRIX& matWorld) = 0;

protected:
	SimpleGeometry* m_geometry = nullptr;
	SimpleMaterial* m_material = nullptr;
	uint32_t m_materialIndex = 0;

	std::map<UID, uint32_t> m_indexTable = {};
	std::vector<RenderObjectInstance*> m_instances = {};
};

class StaticMeshRenderObject : public RenderObject
{
	friend class RenderObjectContainer;
public:
	StaticMeshRenderObject() : RenderObject() {}
protected:
	virtual void Initialize(CommandBufferBase* cmdBuffer, std::string fbxFilePath, ExampleMaterialType exampleMaterialType, bool useMeshShader = false) override;
	virtual RenderObjectInstance* CreateRenderObjectInstance(XMMATRIX& matWorld) override;
};

class SkeletalMeshRenderObject : public RenderObject
{
	friend class RenderObjectContainer;
public:
	SkeletalMeshRenderObject() : RenderObject() {}
protected:
	virtual void Initialize(CommandBufferBase* cmdBuffer, std::string fbxFilePath, ExampleMaterialType exampleMaterialType, bool useMeshShader = false) override;
	virtual void Destroy() override;
	virtual RenderObjectInstance* CreateRenderObjectInstance(XMMATRIX& matWorld) override;
public:
	SimpleSkeleton* GetSkeleton() { return m_skelton; }
private:
	SimpleSkeleton* m_skelton = nullptr;
};

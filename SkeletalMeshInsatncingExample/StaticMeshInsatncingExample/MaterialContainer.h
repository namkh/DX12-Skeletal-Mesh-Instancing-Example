#pragma once

#include "SimpleMaterial.h"
#include "Singleton.h"

class MaterialContainer : public TSingleton<MaterialContainer>
{
public:
	MaterialContainer(token) 
	{
	};

public:
	SimpleMaterial* CreateMaterial(CommandBufferBase* cmdBuffer, ExampleMaterialType matType);
	void RemoveMaterial(SimpleMaterial* material);
	int GetBindIndex(SimpleMaterial* material);

	void Clear();

	uint32_t GetMaterialCount() { return static_cast<uint32_t>(m_materialDatas.size()); }
	SimpleMaterial* GetMaterial(int index);

protected:
	void RefreshIndexTable();

private:
	std::map<ExampleMaterialType, UID> m_materialUidTable;
	std::map<UID, SimpleMaterial*> m_materialDatas;
};

#define gMaterialContainer MaterialContainer::Instance()

//TODO : hard coded
class ExampleMaterialCreateCode : public TSingleton<ExampleMaterialCreateCode>
{
public:
	ExampleMaterialCreateCode(token) {};
public:
	void CreateExampleMaterial(CommandBufferBase*cmdBuffer, ExampleMaterialType type, SimpleMaterial* mat);
};


#include "MaterialContainer.h"
#include "TextureContainer.h"
#include "Defaults.h"

SimpleMaterial* MaterialContainer::CreateMaterial(CommandBufferBase* cmdBuffer, ExampleMaterialType matType)
{
	SimpleMaterial* material = nullptr;

	auto iterUidFind = m_materialUidTable.find(matType);
	if(iterUidFind != m_materialUidTable.end())
	{
		auto iterFind = m_materialDatas.find(iterUidFind->second);
		if (iterFind != m_materialDatas.end())
		{
			material = iterFind->second;
		}
	}

	if (material == nullptr)
	{
		material = new SimpleMaterial();
		material->m_exampleMatType = matType;
		UID uid = material->GetUID();
		ExampleMaterialCreateCode::Instance().CreateExampleMaterial(cmdBuffer, matType, material);
		m_materialDatas.insert(std::make_pair(uid, material));
		m_materialUidTable.insert(std::make_pair(matType, uid));
	}
	
	return material;
}

void MaterialContainer::RemoveMaterial(SimpleMaterial* material)
{
	auto iterFind = m_materialDatas.find(material->GetUID());
	if (iterFind != m_materialDatas.end())
	{
		if (iterFind->second != nullptr)
		{
			m_materialUidTable.erase(iterFind->second->m_exampleMatType);
			iterFind->second->Destory();
			delete iterFind->second;
		}
		m_materialDatas.erase(iterFind);
	}
}

int MaterialContainer::GetBindIndex(SimpleMaterial* material)
{
	auto iterFind = m_materialDatas.find(material->GetUID());
	if (iterFind != m_materialDatas.end())
	{
		return static_cast<int>(std::distance(m_materialDatas.begin(), iterFind));
	}
	return INVALID_INDEX_INT;
}

void MaterialContainer::Clear()
{
	for (auto& cur : m_materialDatas)
	{
		if (cur.second != nullptr)
		{
			cur.second->Destory();
		}
		delete cur.second;
	}

	m_materialDatas.clear();
	m_materialUidTable.clear();
}

SimpleMaterial* MaterialContainer::GetMaterial(int index)
{
	if (index < m_materialDatas.size())
	{
		auto pos = m_materialDatas.begin();
		std::advance(pos, index);
		return pos->second;
	}
	return nullptr;
}

//머트리얼 하드코딩 작성
void ExampleMaterialCreateCode::CreateExampleMaterial(CommandBufferBase* cmdBuffer, ExampleMaterialType type, SimpleMaterial* mat)
{
	switch (type)
	{
		case ExampleMaterialType::EXAMPLE_MAT_INSTANCING1 :
			{
				std::wstring filePath = L"../Resources/Textures/r.dds";
				mat->m_diffuseTex = gTexContainer.CreateTexture(cmdBuffer, filePath);
				
			}
			break;

		case ExampleMaterialType::EXAMPLE_MAT_INSTANCING2:
			{
				std::wstring filePath = L"../Resources/Textures/g.dds";
				mat->m_diffuseTex = gTexContainer.CreateTexture(cmdBuffer, filePath);
			}	
			break;

		case ExampleMaterialType::EXAMPLE_MAT_INSTANCING3:
			{
			std::wstring filePath = L"../Resources/Textures/b.dds";
				mat->m_diffuseTex = gTexContainer.CreateTexture(cmdBuffer, filePath);
			}
			break;

		default:
			break;
	}
}
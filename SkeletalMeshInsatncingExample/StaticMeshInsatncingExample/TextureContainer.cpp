#include "TextureContainer.h"

SimpleTexture2D* TextureContainer::CreateTexture(CommandBufferBase* cmdBuffer, std::wstring& filePath)
{
	SimpleTexture2D* texture = nullptr;

	auto iterKeyFinded = m_keyTable.find(filePath);
	if (iterKeyFinded != m_keyTable.end())
	{
		auto iterFind = m_textureDatas.find(iterKeyFinded->second);
		if (iterFind != m_textureDatas.end())
		{
			texture = iterFind->second;
		}
	}

	if (texture == nullptr)
	{
		texture = LoadTexture(cmdBuffer, filePath.c_str());
	}

	if (texture != nullptr)
	{
		texture->IncRef();
	}

	return texture;
}

int TextureContainer::GetBindIndex(SimpleTexture2D* texture)
{
	auto iterFind = m_textureDatas.find(texture->GetUID());
	if (iterFind != m_textureDatas.end())
	{
		return static_cast<int>(std::distance(m_textureDatas.begin(), iterFind));
	}
	return INVALID_INDEX_INT;
}

SimpleTexture2D* TextureContainer::GetTexture(uint32_t  index)
{
	if (index < m_textureDatas.size())
	{
		auto pos = m_textureDatas.begin();
		std::advance(pos, index);
		return pos->second;
	}
	return nullptr;
}

void TextureContainer::RemoveUnusedTextrures()
{
	std::vector<SimpleTexture2D*> m_removeList;
	for (auto& cur : m_textureDatas)
	{
		if (cur.second->GetRefCount() == 0)
		{
			m_removeList.push_back(cur.second);
		}
	}

	for (auto& cur : m_removeList)
	{
		UnloadTexture(cur);
	}
}

SimpleTexture2D* TextureContainer::LoadTexture(CommandBufferBase* cmdBuffer, const wchar_t* filePath)
{
	SimpleTexture2D* texture = new SimpleTexture2D();
	
	if (!texture->Load(cmdBuffer, filePath))
	{
		return nullptr;
	}
	UID uid = texture->GetUID();
	m_textureDatas.insert(std::make_pair(uid, texture));
	m_keyTable.insert(std::make_pair(filePath, uid));

	return texture;
}

void TextureContainer::UnloadTexture(SimpleTexture2D* texture)
{
	UID uid = texture->GetUID();
	auto iterFind = m_textureDatas.find(uid);
	if (iterFind != m_textureDatas.end())
	{
		if (iterFind->second != nullptr)
		{
			m_keyTable.erase(iterFind->second->GetSrcFilePath());
			iterFind->second->Unload();
			delete iterFind->second;
			m_textureDatas.erase(iterFind);
		}
	}
}

void TextureContainer::Clear()
{
	for (auto& cur : m_textureDatas)
	{
		if (cur.second != nullptr && cur.second->GetRefCount() == 0)
		{
			cur.second->Unload();
			delete cur.second;
		}
		else
		{
			continue;
		}
	}
	m_textureDatas.clear();
	m_keyTable.clear();
}
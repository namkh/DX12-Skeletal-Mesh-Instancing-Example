
#include "SkeletonContainer.h"
#include "FbxLoader.h"

SimpleSkeleton* SkeletonContainer::CreateSkeleton(std::string& filePath)
{
	SimpleSkeleton* skeleton = nullptr;

	auto iterKeyFinded = m_keyTable.find(filePath);
	if (iterKeyFinded != m_keyTable.end())
	{
		auto iterFind = m_skeletonDatas.find(iterKeyFinded->second);
		if (iterFind != m_skeletonDatas.end())
		{
			skeleton = iterFind->second;
		}
	}

	if (skeleton == nullptr)
	{
		skeleton = LoadSkeleton(filePath.c_str());
	}

	if (skeleton != nullptr)
	{
		skeleton->IncRef();
	}

	return skeleton;
}

void SkeletonContainer::RemoveUnusedSkeletons()
{
	std::vector<SimpleSkeleton*> m_removeList;
	for (auto& cur : m_skeletonDatas)
	{
		if (cur.second->GetRefCount() == 0)
		{
			m_removeList.push_back(cur.second);
		}
	}

	for (auto& cur : m_removeList)
	{
		UnloadSkeleton(cur);
	}
}

void SkeletonContainer::Clear()
{
	for (auto& cur : m_skeletonDatas)
	{
		if (cur.second != nullptr && cur.second->GetRefCount() == 0)
		{
			delete cur.second;
		}
		else
		{
			continue;
		}
	}
	m_skeletonDatas.clear();
	m_keyTable.clear();
}

int SkeletonContainer::GetBindIndex(SimpleSkeleton* skeleton)
{
	auto iterFind = m_skeletonDatas.find(skeleton->GetUID());
	if (iterFind != m_skeletonDatas.end())
	{
		static_cast<int>(std::distance(m_skeletonDatas.begin(), iterFind));
	}
	return INVALID_INDEX_INT;
}

SimpleSkeleton* SkeletonContainer::GetSkeleton(uint32_t index)
{
	if (index < m_skeletonDatas.size())
	{
		auto pos = m_skeletonDatas.begin();
		std::advance(pos, index);
		return pos->second;
	}
	return nullptr;
}

SimpleSkeleton* SkeletonContainer::LoadSkeleton(std::string fbxFilePath)
{
	SimpleSkeleton* skeleton = new SimpleSkeleton();

	gFbxLoader.LoadSkeleton(fbxFilePath, skeleton);
	if (skeleton != nullptr)
	{
		skeleton->SetSrcFilePath(fbxFilePath);
		UID uid = skeleton->GetUID();
		m_skeletonDatas.insert(std::make_pair(uid, skeleton));
		m_keyTable.insert(std::make_pair(fbxFilePath, uid));
	}

	return skeleton;
}

void SkeletonContainer::UnloadSkeleton(SimpleSkeleton* skeleton)
{
	UID uid = skeleton->GetUID();
	auto iterFind = m_skeletonDatas.find(uid);
	if (iterFind != m_skeletonDatas.end())
	{
		if (iterFind->second != nullptr)
		{
			m_keyTable.erase(iterFind->second->GetSrcFilePath());
			delete iterFind->second;
			m_skeletonDatas.erase(iterFind);
		}
	}
}
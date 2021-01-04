
#include "AnimSequenceContainer.h"
#include "FbxLoader.h"

SkeletalAnimationSequence* AnimSequenceContainer::CreateAnimSequence(std::string name, std::string filePath, SimpleSkeleton* skeleton)
{
	SkeletalAnimationSequence* animSeq = nullptr;

	auto iterKeyFinded = m_keyTable.find(filePath);
	if (iterKeyFinded != m_keyTable.end())
	{
		auto iterFind = m_animDatas.find(iterKeyFinded->second);
		if (iterFind != m_animDatas.end())
		{
			animSeq = iterFind->second;
		}
	}

	if (animSeq == nullptr)
	{
		animSeq = LoadAnimSequence(name, filePath,  skeleton);
	}

	if (animSeq != nullptr)
	{
		animSeq->IncRef();
	}

	return animSeq;
}

void AnimSequenceContainer::RemoveUnusedAnimSequence()
{
	std::vector<SkeletalAnimationSequence*> m_removeList;
	for (auto& cur : m_animDatas)
	{
		if (cur.second->GetRefCount() == 0)
		{
			m_removeList.push_back(cur.second);
		}
	}

	for (auto& cur : m_removeList)
	{
		UnloadAnimSequence(cur);
	}
}

void AnimSequenceContainer::Clear()
{
	for (auto& cur : m_animDatas)
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
	m_animDatas.clear();
	m_keyTable.clear();
}

int AnimSequenceContainer::GetBindIndex(SkeletalAnimationSequence* skeleton)
{
	auto iterFind = m_animDatas.find(skeleton->GetUID());
	if (iterFind != m_animDatas.end())
	{
		static_cast<int>(std::distance(m_animDatas.begin(), iterFind));
	}
	return INVALID_INDEX_INT;
}

SkeletalAnimationSequence* AnimSequenceContainer::GetAnimSequence(uint32_t  index)
{
	if (index < m_animDatas.size())
	{
		auto pos = m_animDatas.begin();
		std::advance(pos, index);
		return pos->second;
	}
	return nullptr;
}

SkeletalAnimationSequence* AnimSequenceContainer::LoadAnimSequence(std::string name, std::string fbxFilePath, SimpleSkeleton* skeleton)
{
	SkeletalAnimationSequence* animSeq = new SkeletalAnimationSequence();
	animSeq->Initialize(skeleton, name);
	gFbxLoader.LoadAnimation(fbxFilePath, animSeq);
	if (animSeq != nullptr)
	{
		animSeq->SetSrcFilePath(fbxFilePath);
		UID uid = animSeq->GetUID();
		m_animDatas.insert(std::make_pair(uid, animSeq));
		m_keyTable.insert(std::make_pair(fbxFilePath, uid));
	}

	return animSeq;
}

void AnimSequenceContainer::UnloadAnimSequence(SkeletalAnimationSequence* anim)
{
	UID uid = anim->GetUID();
	auto iterFind = m_animDatas.find(uid);
	if (iterFind != m_animDatas.end())
	{
		if (iterFind->second != nullptr)
		{
			m_keyTable.erase(iterFind->second->GetSrcFilePath());
			delete iterFind->second;
			m_animDatas.erase(iterFind);
		}
	}
}
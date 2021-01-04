#pragma once

#include "SkeletalAnimation.h"
#include "SimpleSkeleton.h"
#include "CommandBuffers.h"

class AnimSequenceContainer : public TSingleton<AnimSequenceContainer>
{
public:
	AnimSequenceContainer(token)
	{
	};
public:
	SkeletalAnimationSequence* CreateAnimSequence(std::string name, std::string filePath, SimpleSkeleton* skeleton);
	void RemoveUnusedAnimSequence();
	void Clear();

	int GetBindIndex(SkeletalAnimationSequence* skeleton);
	uint32_t GetTextureCount() { return static_cast<uint32_t>(m_animDatas.size()); }
	SkeletalAnimationSequence* GetAnimSequence(uint32_t  index);

protected:
	SkeletalAnimationSequence* LoadAnimSequence(std::string name, std::string fbxFilePath, SimpleSkeleton* skeleton);
	void UnloadAnimSequence(SkeletalAnimationSequence* skeleton);

private:
	std::map<std::string, UID> m_keyTable;
	std::unordered_map<UID, SkeletalAnimationSequence*> m_animDatas;
};

#define gAnimSeqContainer AnimSequenceContainer::Instance()
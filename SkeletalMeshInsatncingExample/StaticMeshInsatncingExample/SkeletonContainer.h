#pragma once

#include "Singleton.h"
#include "SimpleSkeleton.h"
#include "CommandBuffers.h"

class SkeletonContainer : public TSingleton<SkeletonContainer>
{
public:
	SkeletonContainer(token)
	{
	};
public:
	SimpleSkeleton* CreateSkeleton(std::string& filePath);
	void RemoveUnusedSkeletons();
	void Clear();

	int GetBindIndex(SimpleSkeleton* skeleton);
	uint32_t GetTextureCount() { return static_cast<uint32_t>(m_skeletonDatas.size()); }
	SimpleSkeleton* GetSkeleton(uint32_t  index);

protected:
	SimpleSkeleton* LoadSkeleton(std::string fbxFilePath);
	void UnloadSkeleton(SimpleSkeleton* skeleton);

private:
	std::map<std::string, UID> m_keyTable;
	std::unordered_map<UID, SimpleSkeleton*> m_skeletonDatas;
};

#define gSkelContainer SkeletonContainer::Instance()
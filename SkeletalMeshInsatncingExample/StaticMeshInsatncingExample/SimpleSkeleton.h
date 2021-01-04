#pragma once

#include "Defaults.h"
#include "Utils.h"

#include <string>
#include <vector>
#include <unordered_map>

struct TransformData
{
	XMFLOAT4X4 m_offset = {};
	XMFLOAT4X4 m_anim = {};
	int m_parentTransformIndex = -1;
};

class SimpleSkeleton;

class Bone
{	
public:
	Bone(std::string& boneName) : m_boneName(boneName) { }

public:

	void Initialize(SimpleSkeleton* skel, int matIndex);
	Bone* NewChild(std::string boneName, XMFLOAT4X4& offset);
	Bone* NewChild(std::string boneName);
	
	uint32_t GetChildCount() { return static_cast<uint32_t>(m_childs.size()); }
	Bone* GetChild(uint32_t index);

	int GetTransformDataIndex() { return m_matIndex; }

	std::string GetName() { return m_boneName; }

private:
	std::string m_boneName = "";
	int m_matIndex = -1;

	SimpleSkeleton* m_skeleton = nullptr;
	Bone* m_parent = nullptr;
	std::vector<Bone*> m_childs = {};
};

struct BoneBindInfo
{
	Bone* Owner = nullptr;
	uint32_t TransformIndex = -1;
};

class SimpleSkeleton : public RefCounter, public UniqueIdentifier
{
public:
	SimpleSkeleton(){ }
	~SimpleSkeleton();

	void Destroy();

	uint32_t AddData(Bone* bone, int parentIndex, XMFLOAT4X4& offset);
	uint32_t AddNewData(Bone* bone, int parentIndex);
	Bone* CreateRootBone(SimpleSkeleton* skel, std::string boneName);
	void RemoveBone(Bone* bone);

	TransformData* GetBoneTransformByName(const char* boneName);
	int GetBoneTransformIndexByName(const char* boneName);
	uint32_t GetBoneCount() { return static_cast<uint32_t>(m_transformDatas.size()); }
	Bone* GetBone(uint32_t index);
	Bone* GetBone(const char* boneName);

	std::vector<TransformData>& GetTransformDatas() { return m_transformDatas; }

	void SetName(const char* name) { m_name = name; }
	std::string& GetName() { return m_name; }

	void SetSrcFilePath(std::string& filePath) { m_filePath = filePath; }
	std::string& GetSrcFilePath() { return m_filePath; }

private:

	uint32_t m_availableIndex = 0;
	Bone* m_root = nullptr;
	std::map<std::string, BoneBindInfo> m_boneBindInfoMap = {};
	std::vector<BoneBindInfo> m_boneBindInfoList = {};
	std::vector<TransformData> m_transformDatas = {};

	std::string m_name = {};
	std::string m_filePath = {};
};
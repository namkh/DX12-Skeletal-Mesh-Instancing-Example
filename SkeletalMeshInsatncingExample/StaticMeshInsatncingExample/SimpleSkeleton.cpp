
#include "SimpleSkeleton.h"
#include "Utils.h"

void Bone::Initialize(SimpleSkeleton* skel, int matIndex)
{
	m_skeleton = skel;
	m_matIndex = matIndex;
}

Bone* Bone::NewChild(std::string boneName, XMFLOAT4X4& offset)
{
	Bone* newBone = new Bone(boneName);
	int matIndex = m_skeleton->AddData(newBone, m_matIndex, offset);
	newBone->Initialize(m_skeleton, matIndex);
	m_childs.push_back(newBone);

	return newBone;
}

Bone* Bone::NewChild(std::string boneName)
{
	Bone* newBone = new Bone(boneName);
	int matIndex = m_skeleton->AddNewData(newBone, m_matIndex);
	newBone->Initialize(m_skeleton, matIndex);
	m_childs.push_back(newBone);

	return newBone;
}

Bone* Bone::GetChild(uint32_t index) 
{
	if (index < static_cast<uint32_t>(m_childs.size()))
	{
		return m_childs[index];
	}
	return nullptr;
}

SimpleSkeleton::~SimpleSkeleton()
{
	RemoveBone(m_root);
}

void SimpleSkeleton::Destroy()
{
	DecRef();
}

uint32_t SimpleSkeleton::AddData(Bone* bone, int parentIndex, XMFLOAT4X4& offset)
{
	XMMATRIX offsetTranspose = XMLoadFloat4x4(&offset);	
	offsetTranspose = XMMatrixTranspose(offsetTranspose);

	XMFLOAT4X4 offset4x4;
	XMStoreFloat4x4(&offset4x4, offsetTranspose);

	m_transformDatas.emplace_back(TransformData() = { offset4x4, IDENTITY_FLOAT4X4, parentIndex });
	uint32_t transformIndex = static_cast<uint32_t>(m_transformDatas.size() - 1);
	m_boneBindInfoList.emplace_back(BoneBindInfo() = { bone, transformIndex });
	m_boneBindInfoMap.insert(std::make_pair(bone->GetName(), BoneBindInfo() = { bone, transformIndex }));
	return static_cast<uint32_t>(m_transformDatas.size() - 1);
}

uint32_t SimpleSkeleton::AddNewData(Bone* bone, int parentIndex)
{
	m_transformDatas.emplace_back(TransformData() = { IDENTITY_FLOAT4X4, IDENTITY_FLOAT4X4, parentIndex });
	uint32_t transformIndex = static_cast<uint32_t>(m_transformDatas.size() - 1);
	m_boneBindInfoList.emplace_back(BoneBindInfo() = { bone, transformIndex });
	m_boneBindInfoMap.insert(std::make_pair(bone->GetName(), BoneBindInfo() = { bone, transformIndex }));

	return static_cast<uint32_t>(m_transformDatas.size() - 1);
}

Bone* SimpleSkeleton::CreateRootBone(SimpleSkeleton* skel, std::string boneName)
{
	m_transformDatas.emplace_back(TransformData() = { IDENTITY_FLOAT4X4, IDENTITY_FLOAT4X4, -1 });
	m_root = new Bone(boneName);
	m_root->Initialize(skel, 0);
	m_boneBindInfoList.emplace_back(BoneBindInfo() = { m_root, 0 });
	m_boneBindInfoMap.insert(std::make_pair(boneName, BoneBindInfo() = { m_root, 0 }));

	return m_root;
}

void SimpleSkeleton::RemoveBone(Bone* bone)
{
	if (bone != nullptr)
	{
		uint32_t childCount = bone->GetChildCount();
		for (uint32_t i = 0; i < childCount; i++)
		{
			Bone* child = bone->GetChild(i);
			RemoveBone(child);
		}
		delete bone;
		bone = nullptr;
	}
}

TransformData* SimpleSkeleton::GetBoneTransformByName(const char* boneName)
{
	return &m_transformDatas[GetBoneTransformIndexByName(boneName)];
}

int SimpleSkeleton::GetBoneTransformIndexByName(const char* boneName)
{
	std::string strBoneName = boneName;
	auto iterFind = m_boneBindInfoMap.find(strBoneName);
	if (iterFind != m_boneBindInfoMap.end())
	{
		return iterFind->second.TransformIndex;
	}
	return -1;
}

Bone* SimpleSkeleton::GetBone(uint32_t index)
{
	if (index < m_boneBindInfoList.size())
	{
		return m_boneBindInfoList[index].Owner;
		/*
		auto pos = m_boneBindInfos.begin();
		std::advance(pos, index);
		return pos->second.Owner;
		*/
	}
	return nullptr;
}

Bone* SimpleSkeleton::GetBone(const char* boneName)
{
	auto iterFind = m_boneBindInfoMap.find(boneName);
	if (iterFind != m_boneBindInfoMap.end())
	{
		return iterFind->second.Owner;
	}
	return nullptr;
}
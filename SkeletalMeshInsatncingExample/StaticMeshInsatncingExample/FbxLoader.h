#pragma once

#include<vector>
#include <fbxsdk.h>

#include "Defaults.h"
#include "Singleton.h"
#include "SkeletalAnimation.h"

struct FbxStaticMeshData
{
	std::vector<XMFLOAT3> m_positions;
	std::vector<uint32_t> m_indices;
	std::vector<XMFLOAT3> m_normals;
	std::vector<XMFLOAT3> m_tangents;
	std::vector<XMFLOAT3> m_bitangents;
	std::vector<XMFLOAT4> m_color;
	std::vector<XMFLOAT2> m_uv;
};

struct FbxSkeletalMeshData : public FbxStaticMeshData
{
	std::vector<std::vector<uint32_t> > m_boneIndices;
	std::vector<std::vector<float> > m_weights;
};

class SimpleFbxLoader : public TSingleton<SimpleFbxLoader>
{
public:
	SimpleFbxLoader(token) {};

public:
	bool Initialize();
	void Destory();

protected:
	void GetGeometries(fbxsdk::FbxNode* curNode, std::vector<fbxsdk::FbxMesh*>& fbxMeshes);

	void ReadNormal(fbxsdk::FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outNormal);
	void ReadTangent(fbxsdk::FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outTangent);
	void ReadUV(fbxsdk::FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT2& outUV);
	void ReadColor(fbxsdk::FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT4& outColor);
	void ReadSkinDatas(fbxsdk::FbxMesh* inMesh, SimpleSkeleton* skeleton, std::vector<std::vector<uint32_t> >* boneIndices, std::vector<std::vector<float> >* weights);

public:
	void LoadStaticMeshGeometry(std::string filePath, std::vector<FbxStaticMeshData>& fbxGeometryDatas, bool regenNormalAndTangent = false);
	void LoadSkeletalMeshGeometry(std::string filePath, SimpleSkeleton* skeleton, std::vector<FbxSkeletalMeshData>& fbxGeometryDatas, bool regenNormalAndTangent = false);
	void LoadSkeleton(std::string filePath, SimpleSkeleton* skeleton);
	void LoadAnimation(std::string filePath, SkeletalAnimationSequence* animSequence);

protected:
	fbxsdk::FbxNode* GetRootBone(fbxsdk::FbxNode* parent);
	void ReadBone(fbxsdk::FbxNode* curNode, Bone* parentBone);
	void ReadBoneTransforms(fbxsdk::FbxNode* rootNode, SimpleSkeleton* skeleton);
	void ReadAnimation(fbxsdk::FbxNode* curNode, fbxsdk::FbxAnimLayer* fbxAnimLayer, SkeletalAnimationLayer* animLayer, SimpleSkeleton* skeleton);

	XMMATRIX FbxMatrixToXMMatrix(fbxsdk::FbxAMatrix& fbxMatrix);

private:

	fbxsdk::FbxManager* m_fbxManager = nullptr;
	fbxsdk::FbxIOSettings* m_fbxIoSettings = nullptr;
};

#define gFbxLoader SimpleFbxLoader::Instance()
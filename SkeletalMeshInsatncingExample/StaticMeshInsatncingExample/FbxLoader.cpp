#include "FbxLoader.h"
#include "Utils.h"

bool SimpleFbxLoader::Initialize()
{
	m_fbxManager = FbxManager::Create();
	m_fbxIoSettings = FbxIOSettings::Create(m_fbxManager, IOSROOT);
	m_fbxManager->SetIOSettings(m_fbxIoSettings);

	return true;
}

void SimpleFbxLoader::Destory()
{
	m_fbxIoSettings->Destroy();
	m_fbxManager->Destroy();
}

void SimpleFbxLoader::GetGeometries(fbxsdk::FbxNode* curNode, std::vector<fbxsdk::FbxMesh*>& fbxMeshes)
{
	if (curNode != nullptr)
	{
		fbxsdk::FbxMesh* mesh = curNode->GetMesh();
		if (mesh != nullptr)
		{
			fbxMeshes.push_back(mesh);
		}
		for (int i = 0; i < curNode->GetChildCount(); i++)
		{
			fbxsdk::FbxNode* childNode = curNode->GetChild(i);
			GetGeometries(childNode, fbxMeshes);
		}
	}
}

void SimpleFbxLoader::ReadNormal(fbxsdk::FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outNormal)
{
	fbxsdk::FbxGeometryElementNormal* vertexNormal = inMesh->GetElementNormal(0);

	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inCtrlPointIndex);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		default:
			throw std::exception("Invalid Reference");
		}
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inVertexCounter);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		default: throw std::exception("Invalid Reference");
		}
		break;
	}
}

void SimpleFbxLoader::ReadTangent(fbxsdk::FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outTangent)
{
	fbxsdk::FbxGeometryElementTangent* vertexTangent = inMesh->GetElementTangent(0);

	switch (vertexTangent->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(inCtrlPointIndex);
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
		break;
		}
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexTangent->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexTangent->GetIndexArray().GetAt(inVertexCounter);
			outTangent.x = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[0]);
			outTangent.y = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[1]);
			outTangent.z = static_cast<float>(vertexTangent->GetDirectArray().GetAt(index).mData[2]);
		}
		}
		break;
	}
}

void SimpleFbxLoader::ReadUV(fbxsdk::FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT2& outUV)
{
	fbxsdk::FbxGeometryElementUV* vertexUV = inMesh->GetElementUV(0);

	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outUV.y = static_cast<float>(1.0f - vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(inCtrlPointIndex);
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			outUV.y = static_cast<float>(1.0f - vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
		break;
		}
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outUV.y = static_cast<float>(1.0f - vertexUV->GetDirectArray().GetAt(inVertexCounter).mData[1]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(inVertexCounter);
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			outUV.y = static_cast<float>(1.0f - vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
		}
		break;
	}
}

void SimpleFbxLoader::ReadColor(fbxsdk::FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT4& outColor)
{
	if (inMesh->GetElementVertexColorCount() == 0)
	{
		return;
	}
	
	fbxsdk::FbxGeometryElementVertexColor* vertexColor = inMesh->GetElementVertexColor(0);
	switch (vertexColor->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexColor->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outColor.x = static_cast<float>(vertexColor->GetDirectArray().GetAt(inCtrlPointIndex).mRed);
			outColor.y = static_cast<float>(vertexColor->GetDirectArray().GetAt(inCtrlPointIndex).mGreen);
			outColor.z = static_cast<float>(vertexColor->GetDirectArray().GetAt(inCtrlPointIndex).mBlue);
			outColor.w = static_cast<float>(vertexColor->GetDirectArray().GetAt(inCtrlPointIndex).mAlpha);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexColor->GetIndexArray().GetAt(inCtrlPointIndex);
			outColor.x = static_cast<float>(vertexColor->GetDirectArray().GetAt(index).mRed);
			outColor.y = static_cast<float>(vertexColor->GetDirectArray().GetAt(index).mGreen);
			outColor.z = static_cast<float>(vertexColor->GetDirectArray().GetAt(index).mBlue);
			outColor.w = static_cast<float>(vertexColor->GetDirectArray().GetAt(index).mAlpha);
		}
		break;
		}
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexColor->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outColor.x = static_cast<float>(vertexColor->GetDirectArray().GetAt(inVertexCounter).mRed);
			outColor.y = static_cast<float>(vertexColor->GetDirectArray().GetAt(inVertexCounter).mGreen);
			outColor.z = static_cast<float>(vertexColor->GetDirectArray().GetAt(inVertexCounter).mBlue);
			outColor.w = static_cast<float>(vertexColor->GetDirectArray().GetAt(inVertexCounter).mAlpha);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexColor->GetIndexArray().GetAt(inVertexCounter);
			outColor.x = static_cast<float>(vertexColor->GetDirectArray().GetAt(index).mRed);
			outColor.y = static_cast<float>(vertexColor->GetDirectArray().GetAt(index).mGreen);
			outColor.z = static_cast<float>(vertexColor->GetDirectArray().GetAt(index).mBlue);
			outColor.w = static_cast<float>(vertexColor->GetDirectArray().GetAt(index).mAlpha);
		}
		break;
		}
		break;
	}
}

void SimpleFbxLoader::ReadSkinDatas(fbxsdk::FbxMesh* inMesh, SimpleSkeleton* skeleton, std::vector<std::vector<uint32_t> >* boneIndices, std::vector<std::vector<float> >* weights)
{
	uint32_t deformerCount = inMesh->GetDeformerCount();
	for (uint32_t i = 0; i < deformerCount; i++)
	{
		fbxsdk::FbxSkin* curSkin = fbxsdk::FbxCast<fbxsdk::FbxSkin>(inMesh->GetDeformer(i));
		if (curSkin != nullptr)
		{
			uint32_t clusterCount = curSkin->GetClusterCount();
			for (uint32_t j = 0; j < clusterCount; j++)
			{
				fbxsdk::FbxCluster* curCluster = curSkin->GetCluster(j);

				if (curCluster != nullptr)
				{
					fbxsdk::FbxNode* curNode = curCluster->GetLink();
					int transformIndex = skeleton->GetBoneTransformIndexByName(curNode->GetName());
					if (transformIndex != -1)
					{
						uint32_t indexCount = curCluster->GetControlPointIndicesCount();
						for (uint32_t k = 0; k < indexCount; k++)
						{
							uint32_t index = curCluster->GetControlPointIndices()[k];
							float weight = static_cast<float>(curCluster->GetControlPointWeights()[k]);
							(*boneIndices)[index].push_back(transformIndex);
							(*weights)[index].push_back(weight);
						}
					}
				}
			}
		}
	}
}

void SimpleFbxLoader::LoadStaticMeshGeometry(std::string filePath, std::vector<FbxStaticMeshData>& fbxGeometryDatas, bool regenNormalAndTangent)
{
	fbxsdk::FbxScene* fbxScene = fbxsdk::FbxScene::Create(m_fbxManager, "LoadScene");

	FbxImporter* fbxImpoter = FbxImporter::Create(m_fbxManager, "LoadImporter");
	fbxImpoter->Initialize(filePath.c_str());
	fbxImpoter->Import(fbxScene);

	fbxsdk::FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
	if (sceneAxisSystem != fbxsdk::FbxAxisSystem::DirectX)
	{
		fbxsdk::FbxAxisSystem::DirectX.DeepConvertScene(fbxScene);
	}

	fbxsdk::FbxNode* rootNode = fbxScene->GetRootNode();
	if (rootNode != nullptr)
	{
		std::vector<fbxsdk::FbxMesh*> fbxMeshList;
		GetGeometries(rootNode, fbxMeshList);
		if (fbxMeshList.size() > 0)
		{
			fbxGeometryDatas.resize(fbxMeshList.size());
			for (int i = 0; i < fbxMeshList.size(); i++)
			{
				uint32_t controlpointCount = fbxMeshList[i]->GetControlPointsCount();
				int numPolygons = fbxMeshList[i]->GetPolygonCount();

				std::vector<bool> writeTable;
				writeTable.resize(controlpointCount, false);

				fbxGeometryDatas[i].m_positions.resize(controlpointCount);
				fbxGeometryDatas[i].m_normals.resize(controlpointCount);
				fbxGeometryDatas[i].m_tangents.resize(controlpointCount);
				fbxGeometryDatas[i].m_bitangents.resize(controlpointCount);
				fbxGeometryDatas[i].m_color.resize(controlpointCount);
				fbxGeometryDatas[i].m_uv.resize(controlpointCount);

				if (fbxMeshList[i]->GetElementNormalCount() == 0 || regenNormalAndTangent)
				{
					fbxMeshList[i]->GenerateNormals(true);
				}

				if (fbxMeshList[i]->GetElementTangentCount() == 0 || regenNormalAndTangent)
				{
					fbxMeshList[i]->GenerateTangentsDataForAllUVSets(true);
				}

				int vertexCounter = 0;
				//index
				for (int j = 0; j < numPolygons; j++)
				{
					for (int k = 0; k < 3; k++)
					{
						int ctrlPointIdx = fbxMeshList[i]->GetPolygonVertex(j, k);
						fbxGeometryDatas[i].m_indices.push_back(ctrlPointIdx);
						if (!writeTable[ctrlPointIdx])
						{

							fbxGeometryDatas[i].m_positions[ctrlPointIdx] = XMFLOAT3(static_cast<float>(fbxMeshList[i]->GetControlPointAt(ctrlPointIdx).mData[0]),
																					 static_cast<float>(fbxMeshList[i]->GetControlPointAt(ctrlPointIdx).mData[1]),
																					 static_cast<float>(fbxMeshList[i]->GetControlPointAt(ctrlPointIdx).mData[2]));

							ReadNormal(fbxMeshList[i], ctrlPointIdx, vertexCounter, fbxGeometryDatas[i].m_normals[ctrlPointIdx]);
							ReadTangent(fbxMeshList[i], ctrlPointIdx, vertexCounter, fbxGeometryDatas[i].m_tangents[ctrlPointIdx]);
							ReadColor(fbxMeshList[i], ctrlPointIdx, vertexCounter, fbxGeometryDatas[i].m_color[ctrlPointIdx]);
							ReadUV(fbxMeshList[i], ctrlPointIdx, vertexCounter, fbxGeometryDatas[i].m_uv[ctrlPointIdx]);
							writeTable[ctrlPointIdx] = true;
						}
						++vertexCounter;
					}
				}
			}
		}
	}
	else
	{
		//비어있다는 로깅
	}

	fbxImpoter->Destroy();
	fbxScene->Destroy();
}

void SimpleFbxLoader::LoadSkeletalMeshGeometry(std::string filePath, SimpleSkeleton* skeleton, std::vector<FbxSkeletalMeshData>& fbxGeometryDatas, bool regenNormalAndTangent)
{
	fbxsdk::FbxScene* fbxScene = fbxsdk::FbxScene::Create(m_fbxManager, "LoadScene");

	FbxImporter* fbxImpoter = FbxImporter::Create(m_fbxManager, "LoadImporter");
	fbxImpoter->Initialize(filePath.c_str());
	fbxImpoter->Import(fbxScene);

	fbxsdk::FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
	if (sceneAxisSystem != fbxsdk::FbxAxisSystem::DirectX)
	{
		fbxsdk::FbxAxisSystem::DirectX.DeepConvertScene(fbxScene);
	}

	fbxsdk::FbxNode* rootNode = fbxScene->GetRootNode();
	if (rootNode != nullptr)
	{
		std::vector<fbxsdk::FbxMesh*> fbxMeshList;
		GetGeometries(rootNode, fbxMeshList);
		if (fbxMeshList.size() > 0)
		{
			fbxGeometryDatas.resize(fbxMeshList.size());
			for (int i = 0; i < fbxMeshList.size(); i++)
			{
				uint32_t controlpointCount = fbxMeshList[i]->GetControlPointsCount();
				int numPolygons = fbxMeshList[i]->GetPolygonCount();

				std::vector<bool> writeTable;
				writeTable.resize(controlpointCount, false);

				fbxGeometryDatas[i].m_positions.resize(controlpointCount);
				fbxGeometryDatas[i].m_normals.resize(controlpointCount);
				fbxGeometryDatas[i].m_tangents.resize(controlpointCount);
				fbxGeometryDatas[i].m_bitangents.resize(controlpointCount);
				fbxGeometryDatas[i].m_boneIndices.resize(controlpointCount);
				fbxGeometryDatas[i].m_weights.resize(controlpointCount);
				fbxGeometryDatas[i].m_color.resize(controlpointCount);
				fbxGeometryDatas[i].m_uv.resize(controlpointCount);

				if (fbxMeshList[i]->GetElementNormalCount() == 0 || regenNormalAndTangent)
				{
					fbxMeshList[i]->GenerateNormals(true);
				}

				if (fbxMeshList[i]->GetElementTangentCount() == 0 || regenNormalAndTangent)
				{
					fbxMeshList[i]->GenerateTangentsDataForAllUVSets(true);
				}

				int vertexCounter = 0;
				//index
				for (int j = 0; j < numPolygons; j++)
				{
					for (int k = 0; k < 3; k++)
					{
						int ctrlPointIdx = fbxMeshList[i]->GetPolygonVertex(j, k);
						fbxGeometryDatas[i].m_indices.push_back(ctrlPointIdx);
						if (!writeTable[ctrlPointIdx])
						{
							fbxGeometryDatas[i].m_positions[ctrlPointIdx] = XMFLOAT3(static_cast<float>(fbxMeshList[i]->GetControlPointAt(ctrlPointIdx).mData[0]),
																					 static_cast<float>(fbxMeshList[i]->GetControlPointAt(ctrlPointIdx).mData[1]),
																					 static_cast<float>(fbxMeshList[i]->GetControlPointAt(ctrlPointIdx).mData[2]));

							ReadNormal(fbxMeshList[i], ctrlPointIdx, vertexCounter, fbxGeometryDatas[i].m_normals[ctrlPointIdx]);
							ReadTangent(fbxMeshList[i], ctrlPointIdx, vertexCounter, fbxGeometryDatas[i].m_tangents[ctrlPointIdx]);
							ReadColor(fbxMeshList[i], ctrlPointIdx, vertexCounter, fbxGeometryDatas[i].m_color[ctrlPointIdx]);
							ReadUV(fbxMeshList[i], ctrlPointIdx, vertexCounter, fbxGeometryDatas[i].m_uv[ctrlPointIdx]);
							writeTable[ctrlPointIdx] = true;
						}
						++vertexCounter;
					}
				}

				ReadSkinDatas(fbxMeshList[i], skeleton,  &fbxGeometryDatas[i].m_boneIndices, &fbxGeometryDatas[i].m_weights);
			}
		}
	}
	else
	{
		//비어있다는 로깅
	}

	fbxImpoter->Destroy();
	fbxScene->Destroy();
}

fbxsdk::FbxNode* SimpleFbxLoader::GetRootBone(fbxsdk::FbxNode* parent)
{
	fbxsdk::FbxNode* rootBone = nullptr;
	uint32_t childCount = parent->GetChildCount();
	std::string name = parent->GetName();
	for (uint32_t i = 0; i < childCount; i++)
	{
		fbxsdk::FbxNode* curChild = parent->GetChild(i);
		fbxsdk::FbxNodeAttribute* attribute = curChild->GetNodeAttribute();
		if (attribute != nullptr && attribute->GetAttributeType() == fbxsdk::FbxNodeAttribute::EType::eSkeleton)
		{
			rootBone = curChild;
			break;
		}
		rootBone = GetRootBone(curChild);
		if (rootBone != nullptr)
		{
			break;
		}
	}
	return rootBone;
}

void SimpleFbxLoader::ReadBone(fbxsdk::FbxNode* curNode, Bone* parentBone)
{
	std::string nodeName = curNode->GetName();
	Bone* curBone = parentBone->NewChild(curNode->GetName());

	uint32_t childCount = curNode->GetChildCount();
	for (uint32_t i = 0; i < childCount; i++)
	{
		fbxsdk::FbxNode* curChild = curNode->GetChild(i);
		fbxsdk::FbxNodeAttribute* curChildAttrib = curChild->GetNodeAttribute();
		if (curChildAttrib != nullptr && curChildAttrib->GetAttributeType() == fbxsdk::FbxNodeAttribute::EType::eSkeleton)
		{
			ReadBone(curChild, curBone);
		}
	}
}

XMMATRIX GetToRootMatrix(FbxAMatrix& matInvLink)
{
	fbxsdk::FbxVector4 T = matInvLink.GetT();
	fbxsdk::FbxVector4 R = matInvLink.GetR();
	fbxsdk::FbxVector4 S = matInvLink.GetS();

	XMVECTOR xmT = XMVectorSet(static_cast<float>(T.mData[0]), static_cast<float>(T.mData[1]), static_cast<float>(T.mData[2]), static_cast<float>(T.mData[3]));
	XMVECTOR xmR = XMVectorSet(static_cast<float>(R.mData[0]), static_cast<float>(R.mData[1]), static_cast<float>(R.mData[2]), static_cast<float>(R.mData[3]));
	XMVECTOR xmS = XMVectorSet(static_cast<float>(S.mData[0]), static_cast<float>(S.mData[1]), static_cast<float>(S.mData[2]), static_cast<float>(S.mData[3]));
	XMVECTOR xmQ = XMQuaternionRotationRollPitchYawFromVector(xmR);
	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMMATRIX xmMatT = XMMatrixTranslation(xmT.m128_f32[0], xmT.m128_f32[1], xmT.m128_f32[2]);
	XMMATRIX xmMatR = XMMatrixRotationQuaternion(xmQ);
	XMMATRIX xmMatS = XMMatrixScaling(xmS.m128_f32[0], xmS.m128_f32[1], xmS.m128_f32[2]);

	XMVECTOR det = XMMatrixDeterminant(xmMatT);
	XMMATRIX xmInvMatT = XMMatrixInverse(&det, xmMatT);
	det = XMMatrixDeterminant(xmMatR);
	XMMATRIX xmInvMatR = XMMatrixInverse(&det, xmMatR);
	det = XMMatrixDeterminant(xmMatR);
	XMMATRIX xmInvMatS = XMMatrixInverse(&det, xmMatS);

	return xmInvMatS * xmInvMatR * xmInvMatT;
}

void SimpleFbxLoader::ReadBoneTransforms(fbxsdk::FbxNode* rootNode, SimpleSkeleton* skeleton)
{
	std::vector<fbxsdk::FbxMesh*> fbxMeshList;
	GetGeometries(rootNode, fbxMeshList);
	if (fbxMeshList.size() > 0)
	{
		for (auto& curMesh : fbxMeshList)
		{
			uint32_t deformerCount = curMesh->GetDeformerCount();
			for (uint32_t i = 0; i < deformerCount; i++)
			{
				fbxsdk::FbxSkin* curSkin = fbxsdk::FbxCast<fbxsdk::FbxSkin>(curMesh->GetDeformer(i));
				if (curSkin != nullptr)
				{
					curSkin->GetControlPointBlendWeights();
					uint32_t clusterCount = curSkin->GetClusterCount();
					for (uint32_t j = 0; j < clusterCount; j++)
					{
						fbxsdk::FbxCluster* curCluster = curSkin->GetCluster(j);
						if (curCluster != nullptr)
						{
							fbxsdk::FbxNode* linkNode = curCluster->GetLink();
							if (linkNode != nullptr)
							{
								TransformData* curTransfromData = skeleton->GetBoneTransformByName(linkNode->GetName());
								if (curTransfromData != nullptr)
								{
									int index = skeleton->GetBoneTransformIndexByName(linkNode->GetName());

									const FbxVector4 geoT = linkNode->GetGeometricTranslation(FbxNode::eSourcePivot);
									const FbxVector4 geoR = linkNode->GetGeometricRotation(FbxNode::eSourcePivot);
									const FbxVector4 geoS = linkNode->GetGeometricScaling(FbxNode::eSourcePivot);
									fbxsdk::FbxAMatrix geometricMatrix = fbxsdk::FbxAMatrix(geoT, geoR, geoS);
									XMMATRIX xmGeometricMatrix = FbxMatrixToXMMatrix(geometricMatrix);

									FbxAMatrix matLink = linkNode->EvaluateGlobalTransform();
									XMMATRIX xmMatOffset = XMMatrixInverse(nullptr, FbxMatrixToXMMatrix(matLink) * xmGeometricMatrix);
									XMMATRIX globalBindPoseInv = XMMatrixTranspose(xmMatOffset);
									
									XMStoreFloat4x4(&curTransfromData->m_offset, globalBindPoseInv);
								}
							}
						}
					}
				}
			}
		}
	}	
}

void SimpleFbxLoader::ReadAnimation(fbxsdk::FbxNode* curNode, fbxsdk::FbxAnimLayer* fbxAnimLayer, SkeletalAnimationLayer* animLayer, SimpleSkeleton* skeleton)
{
	if (fbxAnimLayer != nullptr)
	{
		fbxsdk::FbxAnimCurve* translationCurve = curNode->LclTranslation.GetCurve(fbxAnimLayer);
		if (translationCurve != nullptr)
		{
			std::string curNodeName = curNode->GetName();
			uint32_t keyCount = translationCurve->KeyGetCount();
			fbxsdk::FbxTimeSpan timeInterval;
			translationCurve->GetTimeInterval(timeInterval);
			float startTime = static_cast<float>(timeInterval.GetStart().GetSecondDouble());
			float endTime = static_cast<float>(timeInterval.GetStop().GetSecondDouble());
			float duration = static_cast<float>(timeInterval.GetDuration().GetSecondDouble());
			AnimationPerBone* animPerBone = animLayer->NewAnimationPerBone(curNodeName, keyCount, startTime, endTime, duration);

			fbxsdk::FbxTime keyTime = {};		
			for (uint32_t i = 0; i < keyCount; i++)
			{
				keyTime = translationCurve->KeyGetTime(i);
				animPerBone->AnimKeys[i].KeyTime = static_cast<float>(keyTime.GetSecondDouble());
				fbxsdk::FbxAMatrix curLocal = curNode->EvaluateLocalTransform(keyTime);

				FbxVector4 T = curLocal.GetT();
				animPerBone->AnimKeys[i].Translation = XMFLOAT3(static_cast<float>(T.mData[0]), static_cast<float>(T.mData[1]), static_cast<float>(T.mData[2]));
				
				FbxQuaternion Q = curLocal.GetQ();
				XMVECTOR qRot = XMVectorSet(static_cast<float>(Q[0]), static_cast<float>(Q[1]), static_cast<float>(Q[2]), static_cast<float>(Q[3]));
				XMStoreFloat4(&animPerBone->AnimKeys[i].Rotation, qRot);
				
				FbxVector4 S = curLocal.GetS();
				animPerBone->AnimKeys[i].Scale = XMFLOAT3(static_cast<float>(S.mData[0]), static_cast<float>(S.mData[1]), static_cast<float>(S.mData[2]));
			}
		}

		int childCount = curNode->GetChildCount();
		for (int i = 0; i < childCount; i++)
		{
			fbxsdk::FbxNode* curChild = curNode->GetChild(i);
			fbxsdk::FbxNodeAttribute* curChildAttrib = curChild->GetNodeAttribute();
			if (curChildAttrib != nullptr && curChildAttrib->GetAttributeType() == fbxsdk::FbxNodeAttribute::EType::eSkeleton)
			{
				ReadAnimation(curChild, fbxAnimLayer, animLayer, skeleton);
			}
		}
	}
}

XMMATRIX SimpleFbxLoader::FbxMatrixToXMMatrix(fbxsdk::FbxAMatrix& fbxMatrix)
{	
	fbxsdk::FbxVector4 fbxT = fbxMatrix.GetT();
	fbxsdk::FbxVector4 fbxR = fbxMatrix.GetR();
	fbxsdk::FbxVector4 fbxS = fbxMatrix.GetS();
	
	fbxsdk::FbxQuaternion fbxQ = fbxMatrix.GetQ();
	XMVECTOR testQ = XMVectorSet(static_cast<float>(fbxQ[0]), static_cast<float>(fbxQ[1]), static_cast<float>(fbxQ[2]), static_cast<float>(fbxQ[3]));

	XMVECTOR xmT = XMVectorSet(static_cast<float>(fbxT.mData[0]), static_cast<float>(fbxT.mData[1]), static_cast<float>(fbxT.mData[2]), static_cast<float>(fbxT.mData[3]));
	XMVECTOR xmR = XMVectorSet(static_cast<float>(fbxR.mData[0]), static_cast<float>(fbxR.mData[1]), static_cast<float>(fbxR.mData[2]), static_cast<float>(fbxR.mData[3]));
	XMVECTOR xmS = XMVectorSet(static_cast<float>(fbxS.mData[0]), static_cast<float>(fbxS.mData[1]), static_cast<float>(fbxS.mData[2]), static_cast<float>(fbxS.mData[3]));
	XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR xmQ = XMQuaternionRotationRollPitchYawFromVector(xmR);
	
	XMMATRIX xmMatrix = XMMatrixAffineTransformation(xmS, zero, testQ, xmT);

	return xmMatrix;
}

void SimpleFbxLoader::LoadSkeleton(std::string filePath, SimpleSkeleton* skeleton)
{
	fbxsdk::FbxScene* fbxScene = fbxsdk::FbxScene::Create(m_fbxManager, "LoadScene");

	fbxsdk::FbxImporter* fbxImpoter = fbxsdk::FbxImporter::Create(m_fbxManager, "LoadImporter");
	fbxImpoter->Initialize(filePath.c_str());
	fbxImpoter->Import(fbxScene);

	fbxsdk::FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
	if (sceneAxisSystem != fbxsdk::FbxAxisSystem::DirectX)
	{
		fbxsdk::FbxAxisSystem::DirectX.DeepConvertScene(fbxScene);
	}

	fbxsdk::FbxNode* rootNode = fbxScene->GetRootNode();
	fbxsdk::FbxNode* rootBoneNode = GetRootBone(rootNode);
	fbxsdk::FbxNode* skeletonRoot = rootBoneNode->GetParent();
	if (rootBoneNode != nullptr && skeletonRoot != nullptr)
	{
		Bone* rootBone = skeleton->CreateRootBone(skeleton, rootBoneNode->GetName());
		uint32_t childCount = rootBoneNode->GetChildCount();
		for (uint32_t i = 0; i < childCount; i++)
		{
			fbxsdk::FbxNode* curChild = rootBoneNode->GetChild(i);
			fbxsdk::FbxNodeAttribute* curChildAttrib = curChild->GetNodeAttribute();
			if (curChildAttrib != nullptr && curChildAttrib->GetAttributeType() == fbxsdk::FbxNodeAttribute::EType::eSkeleton)
			{
				ReadBone(curChild, rootBone);
			}
		}

		ReadBoneTransforms(rootNode, skeleton);
	}
}

void SimpleFbxLoader::LoadAnimation(std::string filePath, SkeletalAnimationSequence* animSequence)
{
	fbxsdk::FbxScene* fbxScene = fbxsdk::FbxScene::Create(m_fbxManager, "LoadScene");

	fbxsdk::FbxImporter* fbxImpoter = fbxsdk::FbxImporter::Create(m_fbxManager, "LoadImporter");
	fbxImpoter->Initialize(filePath.c_str());
	fbxImpoter->Import(fbxScene);

	fbxsdk::FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
	if (sceneAxisSystem != fbxsdk::FbxAxisSystem::DirectX)
	{
		fbxsdk::FbxAxisSystem::DirectX.DeepConvertScene(fbxScene);
	}

	fbxsdk::FbxNode* rootNode = fbxScene->GetRootNode();
	fbxsdk::FbxNode* rootBoneNode = GetRootBone(rootNode);

	int numStacks = fbxScene->GetSrcObjectCount<fbxsdk::FbxAnimStack>();
	for (int i = 0; i < numStacks; i++)
	{
		fbxsdk::FbxAnimStack* fbxAnimStack = fbxScene->GetSrcObject<fbxsdk::FbxAnimStack>(i);
		if (fbxAnimStack != nullptr)
		{
			std::string stackName = fbxAnimStack->GetName();
			int memberCount = fbxAnimStack->GetMemberCount();
			for (int j = 0; j < memberCount; j++)
			{
				fbxsdk::FbxAnimLayer* fbxAnimLayer = fbxAnimStack->GetMember<fbxsdk::FbxAnimLayer>(j);
				std::string name = fbxAnimLayer->GetName();
				if (fbxAnimLayer != nullptr)
				{
					SkeletalAnimationLayer* animLayer = animSequence->NewAnimLayer(fbxAnimLayer->GetName());
					ReadAnimation(rootBoneNode, fbxAnimLayer, animLayer, animSequence->GetSkeleton());
					animSequence->SetDurationIfGreater(animLayer->GetDuration());
					animSequence->SetKeyCountIfGreater(animLayer->GetKeyCount());
				}
			}
		}
	}
}
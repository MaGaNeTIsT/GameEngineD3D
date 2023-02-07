#include "../Headers/CassimpManager.h"
#include "../../../Sources/EngineBase/Headers/CBaseType.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define CUSTOM_IS_QNAN(f)	(f != f)

static Assimp::Importer* _GAssetImporter = nullptr;

CustomType::Quaternion _GTranslateQuaternion(const aiQuaternion& v)
{
	return (CustomType::Quaternion(v.x, v.y, v.z, v.w));
}

CustomType::Vector3 _GTranslateVector3(const aiVector3D& v)
{
	return (CustomType::Vector3(v.x, v.y, v.z));
}

void _GTranslateMatrix(const aiMatrix4x4& m, CustomType::Vector3& pos, CustomType::Quaternion& rot, CustomType::Vector3& scl)
{
	aiVector3D aiScl, aiPos; aiQuaternion aiRot;
	m.Decompose(aiScl, aiRot, aiPos);
	pos = _GTranslateVector3(aiPos);
	scl = _GTranslateVector3(aiScl);
	rot = _GTranslateQuaternion(aiRot);
}

CustomType::Matrix4x4 _GTranslateMatrix(const aiMatrix4x4& m, const BOOL& useOrigin = TRUE)
{
	if (useOrigin)
	{
		return (CustomType::Matrix4x4(DirectX::XMMATRIX(&(m.a1))));
	}
	else
	{
		aiVector3D scl, pos; aiQuaternion rot;
		m.Decompose(scl, rot, pos);
		return (CustomType::Matrix4x4(
			DirectX::XMMatrixTranspose(
				DirectX::XMMatrixAffineTransformation(
					DirectX::XMVectorSet(scl.x, scl.y, scl.z, 1.f),
					DirectX::XMVectorSet(0.f, 0.f, 0.f, 0.f),
					DirectX::XMVectorSet(rot.x, rot.y, rot.z, rot.w),
					DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.f)))));
	}
}

std::string _GTranslateString(const aiString& s)
{
	return (std::string(s.C_Str()));
}

aiString _GTranslateString(const std::string& s)
{
	return (aiString(s));
}

struct _GVertexSemanticName
{
	_GVertexSemanticName() { Exist = FALSE; Offset = 0u; }
	BOOL	Exist;
	UINT	Offset;
};

struct _GMeshAssetImporterInfo
{
	_GMeshAssetImporterInfo() { NumIndices = 0u; NumVertices = 0u; VertexStride = 0u; }
	std::string				Name;
	UINT					NumIndices;
	UINT					NumVertices;
	UINT					VertexStride;
	_GVertexSemanticName	VertexColor;
	_GVertexSemanticName	VertexPosition;
	_GVertexSemanticName	VertexNormal;
	_GVertexSemanticName	VertexTangent;
	_GVertexSemanticName	VertexUV;
	_GVertexSemanticName	VertexBoneIndices;
	_GVertexSemanticName	VertexBoneWeight;
};

void _GTranslateMeshDesc(const aiScene* scene, std::vector<CustomStruct::CSubMeshInfo>& subMesh, std::vector<_GMeshAssetImporterInfo>& sceneMeshesInfo, UINT& totalNumVertices, UINT& totalNumIndices, const CustomStruct::CRenderInputLayoutDesc* inputLayoutDesc, const UINT& inputLayoutNum)
{
	const static UINT constVertexColorIndex		= 0u;
	const static UINT constVertexTexcoordIndex	= 0u;

	if (subMesh.size() > 0)
	{
		subMesh.clear();
	}
	if (sceneMeshesInfo.size() > 0)
	{
		sceneMeshesInfo.clear();
	}
	totalNumVertices	= 0u;
	totalNumIndices		= 0u;

	if (scene == nullptr || !scene->HasMeshes())
	{
		return;
	}

	UINT numMeshes = scene->mNumMeshes;
	for (UINT indexMesh = 0u; indexMesh < numMeshes; indexMesh++)
	{
		const aiMesh* tempMesh = scene->mMeshes[indexMesh];
		if (tempMesh == nullptr)
		{
			continue;
		}

		_GMeshAssetImporterInfo tempPerMeshInfo;
		tempPerMeshInfo.Name = tempMesh->mName.C_Str();
		tempPerMeshInfo.NumVertices = tempMesh->mNumVertices;
		tempPerMeshInfo.NumIndices = tempMesh->mNumFaces * 3u;

		for (UINT i = 0u; i < inputLayoutNum; i++)
		{
			CustomStruct::CRenderInputLayoutDesc desc(inputLayoutDesc[i]);
			if (desc.SemanticName == CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_COLOR)
			{
				tempPerMeshInfo.VertexColor.Exist = TRUE;
				if (!tempMesh->HasVertexColors(constVertexColorIndex))
				{
					tempPerMeshInfo.VertexColor.Exist = FALSE;
				}
				tempPerMeshInfo.VertexColor.Offset = tempPerMeshInfo.VertexStride;
			}
			else if (desc.SemanticName == CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_POSITION)
			{
				tempPerMeshInfo.VertexPosition.Exist = TRUE;
				if (!tempMesh->HasPositions())
				{
					tempPerMeshInfo.VertexPosition.Exist = FALSE;
				}
				tempPerMeshInfo.VertexPosition.Offset = tempPerMeshInfo.VertexStride;
			}
			else if (desc.SemanticName == CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_NORMAL)
			{
				tempPerMeshInfo.VertexNormal.Exist = TRUE;
				if (!tempMesh->HasNormals())
				{
					tempPerMeshInfo.VertexNormal.Exist = FALSE;
				}
				tempPerMeshInfo.VertexNormal.Offset = tempPerMeshInfo.VertexStride;
			}
			else if (desc.SemanticName == CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_TANGENT)
			{
				tempPerMeshInfo.VertexTangent.Exist = TRUE;
				if (!tempMesh->HasTangentsAndBitangents())
				{
					tempPerMeshInfo.VertexTangent.Exist = FALSE;
				}
				tempPerMeshInfo.VertexTangent.Offset = tempPerMeshInfo.VertexStride;
			}
			else if (desc.SemanticName == CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_TEXCOORD)
			{
				tempPerMeshInfo.VertexUV.Exist = TRUE;
				if (!tempMesh->HasTextureCoords(constVertexTexcoordIndex))
				{
					tempPerMeshInfo.VertexUV.Exist = FALSE;
				}
				tempPerMeshInfo.VertexUV.Offset = tempPerMeshInfo.VertexStride;
			}
			else if (desc.SemanticName == CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_BLENDINDICES)
			{
				tempPerMeshInfo.VertexBoneIndices.Exist = TRUE;
				if (!tempMesh->HasBones())
				{
					tempPerMeshInfo.VertexBoneIndices.Exist = FALSE;
				}
				tempPerMeshInfo.VertexBoneIndices.Offset = tempPerMeshInfo.VertexStride;
			}
			else if (desc.SemanticName == CustomStruct::CRenderShaderSemantic::SHADER_SEMANTIC_BLENDWEIGHT)
			{
				tempPerMeshInfo.VertexBoneWeight.Exist = TRUE;
				if (!tempMesh->HasBones())
				{
					tempPerMeshInfo.VertexBoneWeight.Exist = FALSE;
				}
				tempPerMeshInfo.VertexBoneWeight.Offset = tempPerMeshInfo.VertexStride;
			}
			tempPerMeshInfo.VertexStride += desc.GetSemanticSizeByByte();
		}

		sceneMeshesInfo.push_back(tempPerMeshInfo);
	}

	subMesh.resize(sceneMeshesInfo.size());
	UINT indexSubMesh = 0u;
	for (UINT indexMesh = 0u; indexMesh < numMeshes; indexMesh++)
	{
		if (scene->mMeshes[indexMesh] == nullptr)
		{
			continue;
		}

		CustomStruct::CSubMeshInfo& subMeshInfo = subMesh[indexSubMesh];
		_GMeshAssetImporterInfo& sceneMeshInfo = sceneMeshesInfo[indexSubMesh];

		subMeshInfo.IndexStart = totalNumIndices;
		subMeshInfo.IndexCount = sceneMeshInfo.NumIndices;
		subMeshInfo.VertexStart = totalNumVertices;
		subMeshInfo.VertexCount = sceneMeshInfo.NumVertices;

		totalNumVertices += sceneMeshInfo.NumVertices;
		totalNumIndices += sceneMeshInfo.NumIndices;

		indexSubMesh += 1u;
	}
}

void _GTranslateMeshVertexData(const aiMesh* mesh, const _GMeshAssetImporterInfo& assetInfo, const CustomStruct::CSubMeshInfo& subMeshInfo, CHAR*& verticesData, std::vector<UINT>& indicesData, std::vector<CustomStruct::CGameBoneNodeInfo>& allGameNodes, const std::map<std::string, SHORT>& allGameNodeIndices, std::vector<SHORT>& allBoneList)
{
	const static UINT constVertexColorIndex		= 0u;
	const static UINT constVertexTexcoordIndex	= 0u;

	{
		const UINT numFaces = mesh->mNumFaces;
		aiFace* faces = mesh->mFaces;
		for (UINT indexFace = 0u; indexFace < numFaces; indexFace++)
		{
			aiFace& tempFace = faces[indexFace];
			for (UINT i = 0u; i < 3u; i++)
			{
				indicesData[subMeshInfo.IndexStart + (indexFace * 3u + i)] = tempFace.mIndices[i];
			}
		}
	}

	{
		const UINT	numVertices		= mesh->mNumVertices;

		aiColor4D*	colors			= mesh->mColors[constVertexColorIndex];
		aiVector3D*	vertices		= mesh->mVertices;
		aiVector3D*	normals			= mesh->mNormals;
		aiVector3D*	tangents		= mesh->mTangents;
		aiVector3D*	texcoords		= mesh->mTextureCoords[constVertexTexcoordIndex];

		std::vector<std::vector<std::pair<USHORT, FLOAT>>> blendIndicesWeights;
		UINT* blendWriteIndex = nullptr;

		if (allGameNodes.size() != 0 && allGameNodeIndices.size() != 0 && allGameNodes.size() == allGameNodeIndices.size())
		{
			blendWriteIndex = new UINT[numVertices];
			blendIndicesWeights.resize(numVertices);

			auto _FindBoneListIndex = [](const std::string& boneName, const std::map<std::string, SHORT>& allNodeIndices)->SHORT {
				auto itFindName = allNodeIndices.find(boneName);
				if (itFindName != allNodeIndices.end())
				{
					return (itFindName->second);
				}
				return -1; };
			auto _GetBoneIndex = [](const SHORT& indexInAllGameNodes, std::vector<SHORT>& boneList)->USHORT {
				USHORT boneListSize = static_cast<USHORT>(boneList.size());
				for (USHORT i = 0u; i < boneListSize; i++)
				{
					if (indexInAllGameNodes == boneList[i])
					{
						return i;
					}
				}
				boneList.push_back(indexInAllGameNodes);
				return (static_cast<USHORT>(boneList.size() - 1));
			};
			auto _SortBoneIndicesAndWeight = [](const UINT& num, std::vector<std::pair<USHORT, FLOAT>>& inputIndicesWeights) {
				BOOL needLoop = TRUE;
				while (needLoop == TRUE)
				{
					BOOL isModify = FALSE;
					for (UINT index = 1u; index < num; index++)
					{
						if (inputIndicesWeights[index].second > inputIndicesWeights[index - 1].second)
						{
							USHORT indexOfSmaller = inputIndicesWeights[index - 1].first;
							FLOAT weightOfSmaller = inputIndicesWeights[index - 1].second;
							inputIndicesWeights[index - 1] = std::pair<USHORT, FLOAT>(inputIndicesWeights[index].first, inputIndicesWeights[index].second);
							inputIndicesWeights[index] = std::pair<USHORT, FLOAT>(indexOfSmaller, weightOfSmaller);
							isModify |= TRUE;
						}
					}
					needLoop = isModify;
				}};

			for (UINT indexVertex = 0u; indexVertex < numVertices; indexVertex++)
			{
				blendWriteIndex[indexVertex] = 0u;
			}
			const UINT boneNum = mesh->mNumBones;
			for (UINT boneIndex = 0u; boneIndex < boneNum; boneIndex++)
			{
				const aiBone* bone = mesh->mBones[boneIndex];
				if (bone == nullptr || bone->mNumWeights == 0u || bone->mName.length == 0u)
				{
					continue;
				}
				SHORT tempFindBoneIndex = _FindBoneListIndex(_GTranslateString(bone->mName), allGameNodeIndices);
				if (tempFindBoneIndex < 0)
				{
					continue;
				}

				// Save bind pose matrix of bone.
				allGameNodes[tempFindBoneIndex].BindPoseMatrix = _GTranslateMatrix(bone->mOffsetMatrix, TRUE);
				// [Get new bone list index] and [save bone index] that will be used for rendering.
				USHORT tempBoneIndex = _GetBoneIndex(tempFindBoneIndex, allBoneList);

				for (UINT boneWeightIndex = 0u; boneWeightIndex < bone->mNumWeights; boneWeightIndex++)
				{
					const aiVertexWeight& tempBoneWeight = bone->mWeights[boneWeightIndex];
					blendIndicesWeights[tempBoneWeight.mVertexId].push_back(std::pair<USHORT, FLOAT>(tempBoneIndex, tempBoneWeight.mWeight));
					blendWriteIndex[tempBoneWeight.mVertexId] += 1u;
				}
			}
			for (UINT vertexIndex = 0u; vertexIndex < numVertices; vertexIndex++)
			{
				if (blendWriteIndex[vertexIndex] <= 4u)
				{
					continue;
				}
				_SortBoneIndicesAndWeight(blendWriteIndex[vertexIndex], blendIndicesWeights[vertexIndex]);
			}
		}

		for (UINT indexVertex = 0u; indexVertex < numVertices; indexVertex++)
		{
			CHAR* tempVertex = &(verticesData[assetInfo.VertexStride * (indexVertex + subMeshInfo.VertexStart)]);

			{
				FLOAT* tempColor = (FLOAT*)(&(tempVertex[assetInfo.VertexColor.Offset]));
				if (assetInfo.VertexColor.Exist)
				{
					aiColor4D& tempSceneMeshColor = colors[indexVertex];
					tempColor[0] = tempSceneMeshColor.r;
					tempColor[1] = tempSceneMeshColor.g;
					tempColor[2] = tempSceneMeshColor.b;
					tempColor[3] = tempSceneMeshColor.a;
				}
				else
				{
					tempColor[0] = 0.5f;
					tempColor[1] = 0.5f;
					tempColor[2] = 0.5f;
					tempColor[3] = 1.f;
				}
			}

			{
				FLOAT* tempPosition = (FLOAT*)(&(tempVertex[assetInfo.VertexPosition.Offset]));
				if (assetInfo.VertexPosition.Exist)
				{
					aiVector3D& tempSceneMeshPos = vertices[indexVertex];
					tempPosition[0] = tempSceneMeshPos.x;
					tempPosition[1] = tempSceneMeshPos.y;
					tempPosition[2] = tempSceneMeshPos.z;
					tempPosition[3] = 1.f;
				}
				else
				{
					tempPosition[0] = 0.f;
					tempPosition[1] = 0.f;
					tempPosition[2] = 0.f;
					tempPosition[3] = 1.f;
				}
			}

			{
				FLOAT* tempNormal = (FLOAT*)(&(tempVertex[assetInfo.VertexNormal.Offset]));
				if (assetInfo.VertexNormal.Exist)
				{
					aiVector3D& tempSceneMeshNormal = normals[indexVertex];
					tempNormal[0] = tempSceneMeshNormal.x;
					tempNormal[1] = tempSceneMeshNormal.y;
					tempNormal[2] = tempSceneMeshNormal.z;
					tempNormal[3] = 0.f;
				}
				else
				{
					tempNormal[0] = 0.f;
					tempNormal[1] = 1.f;
					tempNormal[2] = 0.f;
					tempNormal[3] = 0.f;
				}
			}

			{
				FLOAT* tempTangent = (FLOAT*)(&(tempVertex[assetInfo.VertexTangent.Offset]));
				if (assetInfo.VertexTangent.Exist)
				{
					aiVector3D& tempSceneMeshTangent = tangents[indexVertex];
					tempTangent[0] = tempSceneMeshTangent.x;
					tempTangent[1] = tempSceneMeshTangent.y;
					tempTangent[2] = tempSceneMeshTangent.z;
					tempTangent[3] = 0.f;
				}
				else
				{
					tempTangent[0] = 1.f;
					tempTangent[1] = 0.f;
					tempTangent[2] = 0.f;
					tempTangent[3] = 0.f;
				}
			}

			{
				FLOAT* tempUV = (FLOAT*)(&(tempVertex[assetInfo.VertexUV.Offset]));
				if (assetInfo.VertexUV.Exist)
				{
					aiVector3D& tempSceneMeshUV = texcoords[indexVertex];
					tempUV[0] = tempSceneMeshUV.x;
					tempUV[1] = tempSceneMeshUV.y;
				}
				else
				{
					tempUV[0] = 0.f;
					tempUV[1] = 0.f;
				}
			}

			{
				USHORT* tempBoneIndices = (USHORT*)(&(tempVertex[assetInfo.VertexBoneIndices.Offset]));
				if (assetInfo.VertexBoneIndices.Exist && blendWriteIndex != nullptr && blendIndicesWeights.size() > 0)
				{
					std::vector<std::pair<USHORT, FLOAT>>& tempSceneMeshBoneIndices = blendIndicesWeights[indexVertex];
					UINT& tempSceneMeshBoneWriteIndex = blendWriteIndex[indexVertex];
					tempBoneIndices[0] = tempSceneMeshBoneWriteIndex > 0u ? tempSceneMeshBoneIndices[0].first : 0u;
					tempBoneIndices[1] = tempSceneMeshBoneWriteIndex > 1u ? tempSceneMeshBoneIndices[1].first : 0u;
					tempBoneIndices[2] = tempSceneMeshBoneWriteIndex > 2u ? tempSceneMeshBoneIndices[2].first : 0u;
					tempBoneIndices[3] = tempSceneMeshBoneWriteIndex > 3u ? tempSceneMeshBoneIndices[3].first : 0u;
				}
				else
				{
					tempBoneIndices[0] = 0u;
					tempBoneIndices[1] = 0u;
					tempBoneIndices[2] = 0u;
					tempBoneIndices[3] = 0u;
				}
			}

			{
				FLOAT* tempBoneWeights = (FLOAT*)(&(tempVertex[assetInfo.VertexBoneWeight.Offset]));
				if (assetInfo.VertexBoneWeight.Exist && blendWriteIndex != nullptr && blendIndicesWeights.size() > 0)
				{
					std::vector<std::pair<USHORT, FLOAT>>& tempSceneMeshBoneWeights = blendIndicesWeights[indexVertex];
					UINT& tempSceneMeshBoneWriteIndex = blendWriteIndex[indexVertex];
					tempBoneWeights[0] = tempSceneMeshBoneWriteIndex > 0u ? tempSceneMeshBoneWeights[0].second : 1.f;
					tempBoneWeights[1] = tempSceneMeshBoneWriteIndex > 1u ? tempSceneMeshBoneWeights[1].second : 0.f;
					tempBoneWeights[2] = tempSceneMeshBoneWriteIndex > 2u ? tempSceneMeshBoneWeights[2].second : 0.f;
					tempBoneWeights[3] = tempSceneMeshBoneWriteIndex > 3u ? tempSceneMeshBoneWeights[3].second : 0.f;
				}
				else
				{
					tempBoneWeights[0] = 1.f;
					tempBoneWeights[1] = 0.f;
					tempBoneWeights[2] = 0.f;
					tempBoneWeights[3] = 0.f;
				}
			}
		}

		if (blendWriteIndex != nullptr)
		{
			delete[]blendWriteIndex;
		}
	}
}

void _GTranslateDefaultMeshData(const aiScene* scene, std::vector<CustomStruct::CSubMeshInfo>& subMesh, UINT& vertexStride, CHAR*& vertices, UINT& numVertices, std::vector<UINT>& indices, UINT& numIndices, const CustomStruct::CRenderInputLayoutDesc* inputLayoutDesc, const UINT& inputLayoutNum)
{
	vertexStride	= 0u;
	numVertices		= 0u;
	numIndices		= 0u;
	if (vertices != nullptr)
	{
		delete[]vertices;
		vertices = nullptr;
	}
	if (indices.size() > 0)
	{
		indices.clear();
	}

	std::vector<_GMeshAssetImporterInfo> sceneMeshesInfo;
	UINT totalNumVertices, totalNumIndices;
	_GTranslateMeshDesc(scene, subMesh, sceneMeshesInfo, totalNumVertices, totalNumIndices, inputLayoutDesc, inputLayoutNum);

	{
		vertexStride = sceneMeshesInfo[0].VertexStride;
		numVertices = totalNumVertices;
		numIndices = totalNumIndices;
		vertices = new CHAR[totalNumVertices * vertexStride];
		indices.resize(totalNumIndices);
	}

	std::vector<CustomStruct::CGameBoneNodeInfo> tempSkeleton;
	const std::map<std::string, SHORT> tempSkeletonNodeIndices;
	std::vector<SHORT> tempBoneList;

	UINT numMeshes = scene->mNumMeshes;
	UINT indexSubMesh = 0u;
	for (UINT indexMesh = 0u; indexMesh < numMeshes; indexMesh++)
	{
		const aiMesh* tempMesh = scene->mMeshes[indexMesh];
		if (tempMesh == nullptr)
		{
			continue;
		}
		_GMeshAssetImporterInfo& sceneMeshInfo = sceneMeshesInfo[indexSubMesh];
		CustomStruct::CSubMeshInfo& subMeshInfo = subMesh[indexSubMesh];
		_GTranslateMeshVertexData(tempMesh, sceneMeshInfo, subMeshInfo, vertices, indices, tempSkeleton, tempSkeletonNodeIndices, tempBoneList);
		indexSubMesh += 1u;
	}
}

void _GTranslateSkeletonMeshData(const aiScene* scene, std::vector<CustomStruct::CSubMeshInfo>& subMesh, UINT& vertexStride, CHAR*& vertices, UINT& numVertices, std::vector<UINT>& indices, UINT& numIndices, std::vector<CustomStruct::CGameBoneNodeInfo>& allGameNodes, const std::map<std::string, SHORT>& allGameNodeIndices, std::vector<USHORT>& allBoneList, const CustomStruct::CRenderInputLayoutDesc* inputLayoutDesc, const UINT& inputLayoutNum)
{
	vertexStride	= 0u;
	numVertices		= 0u;
	numIndices		= 0u;
	if (vertices != nullptr)
	{
		delete[]vertices;
		vertices = nullptr;
	}
	if (indices.size() > 0)
	{
		indices.clear();
	}
	if (allBoneList.size() > 0)
	{
		allBoneList.clear();
	}

	std::vector<_GMeshAssetImporterInfo> sceneMeshesInfo;
	UINT totalNumVertices, totalNumIndices;
	_GTranslateMeshDesc(scene, subMesh, sceneMeshesInfo, totalNumVertices, totalNumIndices, inputLayoutDesc, inputLayoutNum);

	{
		vertexStride = sceneMeshesInfo[0].VertexStride;
		numVertices = totalNumVertices;
		numIndices = totalNumIndices;
		vertices = new CHAR[totalNumVertices * vertexStride];
		indices.resize(totalNumIndices);
	}

	std::vector<SHORT> tempBoneList;
	tempBoneList.push_back(-1);

	UINT numMeshes = scene->mNumMeshes;
	UINT indexSubMesh = 0u;
	for (UINT indexMesh = 0u; indexMesh < numMeshes; indexMesh++)
	{
		const aiMesh* tempMesh = scene->mMeshes[indexMesh];
		if (tempMesh == nullptr)
		{
			continue;
		}
		_GMeshAssetImporterInfo& sceneMeshInfo = sceneMeshesInfo[indexSubMesh];
		CustomStruct::CSubMeshInfo& subMeshInfo = subMesh[indexSubMesh];
		_GTranslateMeshVertexData(tempMesh, sceneMeshInfo, subMeshInfo, vertices, indices, allGameNodes, allGameNodeIndices, tempBoneList);
		indexSubMesh += 1u;
	}

	if (tempBoneList.size() > 1)
	{
		allBoneList.resize(tempBoneList.size());
		allBoneList[0] = 0u;
		USHORT tempNumBoneList = static_cast<USHORT>(tempBoneList.size());
		for (USHORT boneIndex = 1u; boneIndex < tempNumBoneList; boneIndex++)
		{
			allBoneList[boneIndex] = static_cast<USHORT>(tempBoneList[boneIndex]);
		}
	}
}

const static std::string _GConstImporterNodeEmptyName	= "_ConstImporterNodeEmptyName";
const static std::string _GConstDummyName				= "_ConstDummyName";

void _GReadNodeTransformRecursion(const aiNode* node, std::vector<CustomStruct::CGameBoneNodeInfo>& allGameNodes, const std::map<const aiNode*, aiString>& allNodeNames, const std::map<aiString, INT>& allNodeIndices, CustomType::Matrix4x4 globalMatrix)
{
	if (node == nullptr)
	{
		return;
	}

	CustomType::Matrix4x4 tempGlobalMatrix = _GTranslateMatrix(node->mTransformation, TRUE);
	{
		aiVector3D scl, pos; aiQuaternion rot;
		INT tempNodeIndex = allNodeIndices.find(allNodeNames.find(node)->second)->second;
		_GTranslateMatrix(node->mTransformation, allGameNodes[tempNodeIndex].DefaultPosition, allGameNodes[tempNodeIndex].DefaultRotation, allGameNodes[tempNodeIndex].DefaultScaling);
		tempGlobalMatrix = globalMatrix * tempGlobalMatrix;
	}

	for (UINT indexChild = 0u; indexChild < node->mNumChildren; indexChild++)
	{
		_GReadNodeTransformRecursion(node->mChildren[indexChild], allGameNodes, allNodeNames, allNodeIndices, tempGlobalMatrix);
	}
}

void _GGatherSingleNodeRecursion(const aiNode* node, std::vector<const aiNode*>& allNodes, std::map<const aiNode*, aiString>& allNodeNames, std::map<aiString, INT>& allNodeIndices)
{
	if (node == nullptr)
	{
		return;
	}

	allNodes.push_back(node);
	{
		INT indexNode = static_cast<INT>(allNodes.size() - 1);

		aiString tempNodeName;
		if (node->mName.length > 0)
		{
			tempNodeName = node->mName;
		}
		else
		{
			tempNodeName = _GTranslateString(_GConstImporterNodeEmptyName);
		}

		auto itFindNode = allNodeIndices.find(tempNodeName);

		std::string tempOldName = _GTranslateString(tempNodeName) + "_";
		aiString tempNewName = tempNodeName;
		UINT tempSameNameIndex = 0u;
		while (itFindNode != allNodeIndices.end())
		{
			std::string tempName = tempOldName + std::to_string(tempSameNameIndex);
			tempNewName = _GTranslateString(tempName);
			tempSameNameIndex += 1u;
			itFindNode = allNodeIndices.find(tempNewName);
		}

		allNodeIndices.insert_or_assign(tempNewName, indexNode);
		allNodeNames.insert_or_assign(node, tempNewName);
	}

	for (UINT indexChild = 0u; indexChild < node->mNumChildren; indexChild++)
	{
		_GGatherSingleNodeRecursion(node->mChildren[indexChild], allNodes, allNodeNames, allNodeIndices);
	}
}

BOOL _GGatherSkeletonMeshAllNodeStructures(const aiScene* scene,
	std::vector<CustomStruct::CSubMeshInfo>& subMesh, UINT& vertexStride, CHAR*& vertices, UINT& numVertices, std::vector<UINT>& indices, UINT& numIndices,
	std::vector<CustomStruct::CGameBoneNodeInfo>& allGameNodes, std::map<std::string, SHORT>& allGameNodeIndices, std::vector<USHORT>& allBoneList,
	const CustomStruct::CRenderInputLayoutDesc* inputLayoutDesc, const UINT& inputLayoutNum)
{
	// Init all output data.
	{
		if (vertices != nullptr)
		{
			delete[]vertices;
			vertices = nullptr;
		}
		vertexStride = 0u;
		numVertices = 0u;
		numIndices = 0u;
		if (subMesh.size() > 0)
		{
			subMesh.clear();
		}
		if (indices.size() > 0)
		{
			indices.clear();
		}
		if (allGameNodes.size() > 0)
		{
			allGameNodes.clear();
		}
		if (allGameNodeIndices.size() > 0)
		{
			allGameNodeIndices.clear();
		}
		if (allBoneList.size() > 0)
		{
			allBoneList.clear();
		}
	}

	// Check scene pointer.
	if (scene == nullptr)
	{
		return FALSE;
	}

	// Read data structures of all nodes.
	std::vector<const aiNode*> allNodes; std::map<const aiNode*, aiString> allNodeNames; std::map<aiString, INT> allNodeIndices;
	_GGatherSingleNodeRecursion(scene->mRootNode, allNodes, allNodeNames, allNodeIndices);

	// Check read state.
	if (allNodes.size() == 0 || allNodes.size() != allNodeIndices.size())
	{
		return FALSE;
	}

	// Create engine's hierarchical bones' data.
	{
		const UINT numAllNodes = static_cast<UINT>(allNodes.size());
		for (UINT indexNode = 0u; indexNode < numAllNodes; indexNode++)
		{
			auto itFindNodeName = allNodeNames.find(allNodes[indexNode]);
			CustomStruct::CGameBoneNodeInfo tempNewBone(_GTranslateString(itFindNodeName->second));
			tempNewBone.Index = static_cast<SHORT>(indexNode);
			tempNewBone.Parent = -1;
			if (allNodes[indexNode]->mParent != nullptr)
			{
				tempNewBone.Parent = allNodeIndices.find(itFindNodeName->second)->second;
			}
			allGameNodes.push_back(tempNewBone);
			allGameNodeIndices.insert_or_assign(tempNewBone.Name, tempNewBone.Index);
		}
	}

	// Read mesh data for skeleton mesh.
	_GTranslateSkeletonMeshData(scene, subMesh, vertexStride, vertices, numVertices, indices, numIndices, allGameNodes, allGameNodeIndices, allBoneList, inputLayoutDesc, inputLayoutNum);

	// Read skeleton default transform.
	CustomType::Matrix4x4 dummyMat(CustomType::Matrix4x4::Identity());
	_GReadNodeTransformRecursion(scene->mRootNode, allGameNodes, allNodeNames, allNodeIndices, dummyMat);

	// Save node's child relation.
	{
		const SHORT numAllGameNodes = static_cast<SHORT>(allGameNodes.size());
		for (SHORT indexNode = 0; indexNode < numAllGameNodes; indexNode++)
		{
			CustomStruct::CGameBoneNodeInfo& gameNode = allGameNodes[indexNode];
			if (indexNode != gameNode.Index || gameNode.Parent < 0 || gameNode.Parent >= numAllGameNodes)
			{
				continue;
			}
			CustomStruct::CGameBoneNodeInfo& gameParentNode = allGameNodes[gameNode.Parent];
			gameParentNode.Children.push_back(gameNode.Index);
		}
	}

	return TRUE;
}

struct _GImporterBoneNodeData
{
	_GImporterBoneNodeData()
	{
		this->Node			= nullptr;
		this->Bone			= nullptr;
		this->Index			= -2;
		this->Parent		= nullptr;
	}
	_GImporterBoneNodeData(const _GImporterBoneNodeData& v)
	{
		this->Node			= v.Node;
		this->Bone			= v.Bone;
		this->Index			= v.Index;
		this->Parent		= v.Parent;
		this->Name			= v.Name;
	}
	const aiNode*							Node;
	const aiBone*							Bone;
	INT										Index;
	_GImporterBoneNodeData*					Parent;
	std::string								Name;
};

void _GGatherSingleNodeRecursion(const aiNode* node, std::vector<const aiNode*>& outputNode, std::map<const aiNode*, INT>& outputParent)
{
	if (node == nullptr)
	{
		return;
	}

	outputNode.push_back(node);
	outputParent.insert_or_assign(node, -2);

	for (UINT indexChild = 0u; indexChild < node->mNumChildren; indexChild++)
	{
		_GGatherSingleNodeRecursion(node->mChildren[indexChild], outputNode, outputParent);
	}
}

BOOL _GGatherAllNodes(const aiNode* rootNode, std::vector<const aiNode*>& outputAllNodes, std::map<const aiNode*, INT>& outputNodeParent, std::map<std::string, _GImporterBoneNodeData>& output)
{
	if (outputAllNodes.size() > 0)
	{
		outputAllNodes.clear();
	}
	if (outputNodeParent.size() > 0)
	{
		outputNodeParent.clear();
	}
	if (output.size() > 0)
	{
		output.clear();
	}
	if (rootNode == nullptr)
	{
		return FALSE;
	}

	_GGatherSingleNodeRecursion(rootNode, outputAllNodes, outputNodeParent);

	if (outputAllNodes.size() == 0 || outputAllNodes.size() != outputNodeParent.size())
	{
		return FALSE;
	}

	const UINT numNodes = static_cast<UINT>(outputAllNodes.size());
	{
		auto findParentIndex = [](const aiNode* node, const std::vector<const aiNode*>& allNodes, const UINT& numNodes)->INT {
			if (node == nullptr)
			{
				return -2;
			}
			if (node->mParent == nullptr)
			{
				return -1;
			}
			for (UINT indexNode = 0u; indexNode < numNodes; indexNode++)
			{
				const aiNode* tempNode = allNodes[indexNode];
				if (tempNode == nullptr)
				{
					continue;
				}
				if (tempNode == node->mParent)
				{
					return static_cast<INT>(indexNode);
				}
			}
			return -2; };

		for (UINT indexNode = 0u; indexNode < numNodes; indexNode++)
		{
			const aiNode* tempNode = outputAllNodes[indexNode];
			if (tempNode == nullptr)
			{
				continue;
			}
			auto itNodeParent = outputNodeParent.find(tempNode);
			if (itNodeParent == outputNodeParent.end())
			{
				continue;
			}
			itNodeParent->second = findParentIndex(tempNode, outputAllNodes, numNodes);
		}
	}

	for (UINT indexNode = 0u; indexNode < numNodes; indexNode++)
	{
		const aiNode* tempNode = outputAllNodes[indexNode];
		if (tempNode == nullptr)
		{
			continue;
		}

		_GImporterBoneNodeData tempBoneNodeData;
		tempBoneNodeData.Node = tempNode;
		tempBoneNodeData.Bone = nullptr;
		tempBoneNodeData.Index = static_cast<INT>(indexNode);
		tempBoneNodeData.Parent = nullptr;

		std::string tempBoneNodeName;
		if (tempNode->mName.length < 1)
		{
			tempBoneNodeName = _GConstImporterNodeEmptyName;
		}
		else
		{
			tempBoneNodeName = _GTranslateString(tempNode->mName);
		}

		tempBoneNodeData.Name = tempBoneNodeName;

		if (output.find(tempBoneNodeData.Name) != output.end())
		{
			UINT tempDummyIndex = 0u;
			tempBoneNodeData.Name = tempBoneNodeName + "_" + std::to_string(tempDummyIndex);
			tempDummyIndex += 1u;
			while (output.find(tempBoneNodeData.Name) != output.end())
			{
				tempBoneNodeData.Name = tempBoneNodeName + "_" + std::to_string(tempDummyIndex);
				tempDummyIndex += 1u;
			}
		}

		output.insert_or_assign(tempBoneNodeData.Name, tempBoneNodeData);
	}

	for (auto itNode = output.begin(); itNode != output.end(); itNode++)
	{
		if (itNode->second.Node == nullptr)
		{
			continue;
		}
		auto itNodeParent = outputNodeParent.find(itNode->second.Node);
		if (itNodeParent == outputNodeParent.end())
		{
			continue;
		}
		if (itNodeParent->second == -2 || itNodeParent->second == -1)
		{
			continue;
		}
		for (auto findNodeParent = output.begin(); findNodeParent != output.end(); findNodeParent++)
		{
			if (findNodeParent->second.Index == itNodeParent->second)
			{
				itNode->second.Parent = &(findNodeParent->second);
			}
		}
	}

	return TRUE;
}

void _GGatherSingleNodeRecursion(const aiNode* node, _GImporterBoneNodeData* parentNode, std::map<std::string, std::vector<_GImporterBoneNodeData>>& output)
{
	if (node == nullptr)
	{
		return;
	}
	_GImporterBoneNodeData* currentNode = nullptr;
	{
		std::string nodeName = _GTranslateString(node->mName);
		{
			if (node->mName.length < 1)
			{
				nodeName = _GConstImporterNodeEmptyName;
			}
			std::map<std::string, std::vector<_GImporterBoneNodeData>>::iterator result = output.end();
			{
				result = output.find(nodeName);
				if (result == output.end())
				{
					std::pair<std::map<std::string, std::vector<_GImporterBoneNodeData>>::iterator, bool> tempResult = output.insert_or_assign(nodeName, std::vector<_GImporterBoneNodeData>());
					result = tempResult.first;
				}
			}
			if (result != output.end())
			{
				result->second.push_back(_GImporterBoneNodeData());
				currentNode = &(result->second[result->second.size() - 1]);
			}
		}
		if (currentNode != nullptr)
		{
			currentNode->Name = nodeName;
			currentNode->Node = node;
			currentNode->Parent = parentNode;
		}
	}
	for (UINT childIndex = 0u; childIndex < node->mNumChildren; childIndex++)
	{
		_GGatherSingleNodeRecursion(node->mChildren[childIndex], currentNode, output);
	}
}

void _GResortAllNodeRecursion(std::map<std::string, std::vector<_GImporterBoneNodeData>>& input, std::map<std::string, _GImporterBoneNodeData>& output)
{
	if (output.size() != 0)
	{
		output.clear();
	}

	{
		// Add into a new list that delete same name node and add dummy node.
		UINT dummyNodeIndex = 0u;
		for (auto it = input.begin(); it != input.end(); it++)
		{
			if (it->second.size() == 1)
			{
				_GImporterBoneNodeData& tempNodeData = (it->second)[0];
				output.insert_or_assign(tempNodeData.Name, _GImporterBoneNodeData(tempNodeData));
			}
			else if (it->second.size() > 1)
			{
				for (UINT nodeIndex = 0u; nodeIndex < static_cast<UINT>(it->second.size()); nodeIndex++)
				{
					_GImporterBoneNodeData& tempNodeData = (it->second)[nodeIndex];
					tempNodeData.Name = (_GConstDummyName + '_' + std::to_string(dummyNodeIndex));
					dummyNodeIndex += 1u;
					_GImporterBoneNodeData newNodeData(tempNodeData);
					output.insert_or_assign(newNodeData.Name, newNodeData);
				}
			}
		}
	}

	// Restore parent and child connection.
	for (auto it = output.begin(); it != output.end(); it++)
	{
		if (it->second.Parent != nullptr)
		{
			auto parentIt = output.find(it->second.Parent->Name);
			if (parentIt != output.end())
			{
				it->second.Parent = &(parentIt->second);
			}
			else
			{
				it->second.Parent = nullptr;
			}
		}
	}
}

void _GGatherAllNodes(const aiNode* root, std::map<std::string, _GImporterBoneNodeData>& output)
{
	std::map<std::string, std::vector<_GImporterBoneNodeData>> nodeDatas;
	_GGatherSingleNodeRecursion(root, nullptr, nodeDatas);
	_GResortAllNodeRecursion(nodeDatas, output);
}

void _GGatherAllBones(const aiScene* scene, std::map<std::string, const aiBone*>& output)
{
	if (output.size() != 0)
	{
		output.clear();
	}
	UINT meshNum = scene->mNumMeshes;
	for (UINT meshIndex = 0u; meshIndex < meshNum; meshIndex++)
	{
		const aiMesh* mesh = scene->mMeshes[meshIndex];
		if (mesh == nullptr || !mesh->HasBones())
		{
			continue;
		}
		UINT boneNum = mesh->mNumBones;
		for (UINT boneIndex = 0u; boneIndex < boneNum; boneIndex++)
		{
			const aiBone* bone = mesh->mBones[boneIndex];
			if (bone == nullptr)
			{
				continue;
			}
			output.insert_or_assign(_GTranslateString(bone->mName), bone);
		}
	}
}

#if 0
BOOL _GGatherBoneDatas(const aiScene* scene, std::vector<CustomStruct::CGameBoneNodeInfo>& skeletonOutput, std::vector<UINT>& boneList, INT& skeletonRootNode)
{
	if (skeletonOutput.size() != 0)
	{
		skeletonOutput.clear();
	}
	if (boneList.size() != 0)
	{
		boneList.clear();
	}
	skeletonRootNode = -1;

	std::map<std::string, const aiBone*> boneMap;
	_GGatherAllBones(scene, boneMap);

	if (boneMap.size() < 1)
	{
		return FALSE;
	}

	std::map<std::string, _GImporterBoneNodeData> nodeMap;

#if 1

	{
		std::vector<const aiNode*> allNodes;
		std::map<const aiNode*, INT> allNodeParents;
		if (!_GGatherAllNodes(scene->mRootNode, allNodes, allNodeParents, nodeMap))
		{
			return FALSE;
		}
	}

#else

	_GGatherAllNodes(scene->mRootNode, nodeMap);

#endif

	for (auto it = nodeMap.begin(); it != nodeMap.end(); it++)
	{
		auto boneIt = boneMap.find(it->second.Name);
		if (boneIt != boneMap.end())
		{
			it->second.Bone = boneIt->second;
		}
	}

	{
		UINT boneIndex = 0u;
		for (auto it = nodeMap.begin(); it != nodeMap.end(); it++)
		{
			skeletonOutput.push_back(CustomStruct::CGameBoneNodeInfo(it->second.Name));
			if (it->second.Bone != nullptr)
			{
				boneList.push_back(boneIndex);
			}
			boneIndex += 1u;
		}
	}

	{
		UINT skeletonNodeNum = static_cast<UINT>(skeletonOutput.size());
		for (UINT nodeIndex = 0u; nodeIndex < skeletonNodeNum; nodeIndex++)
		{
			CustomStruct::CGameBoneNodeInfo& node = skeletonOutput[nodeIndex];

			auto it = nodeMap.find(node.Name);
			if (it == nodeMap.end())
			{
				continue;
			}

			_GTranslateTransformMatrix(it->second.Node->mTransformation, node.Location, node.Rotation, node.Scale);

			if (it->second.Bone != nullptr)
			{
				node.Offset = _GTranslateBindPoseMatrix(it->second.Bone->mOffsetMatrix);
#if 0
				{
					aiMatrix4x4 tempRawOffsetNoT = it->second.Bone->mOffsetMatrix;

					DirectX::XMMATRIX tempRawOffsetMatNoT(&(tempRawOffsetNoT.a1));
					CustomType::Matrix4x4 tempOffsetMatNoT(tempRawOffsetMatNoT);

					aiVector3D aiScalingNoT, aiPositionNoT;
					aiVector3D aiRotationENoT; aiQuaternion aiRotationQNoT;
					tempRawOffsetNoT.Decompose(aiScalingNoT, aiRotationENoT, aiPositionNoT);
					tempRawOffsetNoT.Decompose(aiScalingNoT, aiRotationQNoT, aiPositionNoT);

					CustomType::Quaternion tempRotNoT(DirectX::XMMatrixRotationRollPitchYaw(aiRotationENoT.x, aiRotationENoT.y, aiRotationENoT.z));
					CustomType::Matrix4x4 tempOffsetMatCom_0NoT(CustomType::Vector3(aiPositionNoT.x, aiPositionNoT.y, aiPositionNoT.z), tempRotNoT, CustomType::Vector3(aiScalingNoT.x, aiScalingNoT.y, aiScalingNoT.z));
					CustomType::Matrix4x4 tempOffsetMatCom_1NoT(CustomType::Vector3(aiPositionNoT.x, aiPositionNoT.y, aiPositionNoT.z), CustomType::Quaternion(aiRotationQNoT.w, aiRotationQNoT.x, aiRotationQNoT.y, aiRotationQNoT.z), CustomType::Vector3(aiScalingNoT.x, aiScalingNoT.y, aiScalingNoT.z));
					CustomType::Matrix4x4 tempOffsetMatCom_2NoT(CustomType::Vector3(aiPositionNoT.x, aiPositionNoT.y, aiPositionNoT.z), CustomType::Quaternion(aiRotationQNoT.y, aiRotationQNoT.z, aiRotationQNoT.w, aiRotationQNoT.x), CustomType::Vector3(aiScalingNoT.x, aiScalingNoT.y, aiScalingNoT.z));
					CustomType::Matrix4x4 tempOffsetMatCom_3NoT(CustomType::Vector3(aiPositionNoT.x, aiPositionNoT.y, aiPositionNoT.z), CustomType::Quaternion(aiRotationQNoT.x, aiRotationQNoT.y, aiRotationQNoT.z, aiRotationQNoT.w), CustomType::Vector3(aiScalingNoT.x, aiScalingNoT.y, aiScalingNoT.z));


					aiMatrix4x4 tempRawOffset = it->second.Bone->mOffsetMatrix;
					tempRawOffset.Transpose();

					DirectX::XMMATRIX tempRawOffsetMat(&(tempRawOffset.a1));
					CustomType::Matrix4x4 tempOffsetMat(tempRawOffsetMat);

					aiVector3D aiScaling, aiPosition;
					aiVector3D aiRotationE; aiQuaternion aiRotationQ;
					tempRawOffset.Decompose(aiScaling, aiRotationE, aiPosition);
					tempRawOffset.Decompose(aiScaling, aiRotationQ, aiPosition);

					CustomType::Quaternion tempRot(DirectX::XMMatrixRotationRollPitchYaw(aiRotationE.x, aiRotationE.y, aiRotationE.z));
					CustomType::Matrix4x4 tempOffsetMatCom_0(CustomType::Vector3(aiPosition.x, aiPosition.y, aiPosition.z), tempRot, CustomType::Vector3(aiScaling.x, aiScaling.y, aiScaling.z));
					CustomType::Matrix4x4 tempOffsetMatCom_1(CustomType::Vector3(aiPosition.x, aiPosition.y, aiPosition.z), CustomType::Quaternion(aiRotationQ.w, aiRotationQ.x, aiRotationQ.y, aiRotationQ.z), CustomType::Vector3(aiScaling.x, aiScaling.y, aiScaling.z));
					CustomType::Matrix4x4 tempOffsetMatCom_2(CustomType::Vector3(aiPosition.x, aiPosition.y, aiPosition.z), CustomType::Quaternion(aiRotationQ.y, aiRotationQ.z, aiRotationQ.w, aiRotationQ.x), CustomType::Vector3(aiScaling.x, aiScaling.y, aiScaling.z));
					CustomType::Matrix4x4 tempOffsetMatCom_3(CustomType::Vector3(aiPosition.x, aiPosition.y, aiPosition.z), CustomType::Quaternion(aiRotationQ.x, aiRotationQ.y, aiRotationQ.z, aiRotationQ.w), CustomType::Vector3(aiScaling.x, aiScaling.y, aiScaling.z));

					int a = 0;
				}
#endif
			}

			if (it->second.Parent != nullptr)
			{
				for (UINT parentIndex = 0u; parentIndex < skeletonNodeNum; parentIndex++)
				{
					if (skeletonOutput[parentIndex].Name == it->second.Parent->Name)
					{
						node.Parent = &(skeletonOutput[parentIndex]);
						break;
					}
				}
			}
		}
		std::string rawRootNodeName(scene->mRootNode->mName.C_Str());
		for (UINT nodeIndex = 0u; nodeIndex < skeletonNodeNum; nodeIndex++)
		{
			CustomStruct::CGameBoneNodeInfo& node = skeletonOutput[nodeIndex];
			node.Index = static_cast<INT>(nodeIndex);
			if (skeletonRootNode == -1 && node.Name == rawRootNodeName)
			{
				skeletonRootNode = static_cast<INT>(nodeIndex);
			}
			if (node.Parent != nullptr)
			{
				node.Parent->Children.push_back(&(skeletonOutput[nodeIndex]));
			}
		}
	}

	return (boneList.size() > 0 && skeletonRootNode >= 0);
}
#endif

void _GTranslateAnimationBehaviour(const aiAnimBehaviour& input, CustomStruct::CGameAnimationBehaviour& output)
{
	if (input == aiAnimBehaviour::aiAnimBehaviour_DEFAULT)
	{
		output = CustomStruct::CGameAnimationBehaviour::ANIMATION_BEHAVIOUR_DEFAULT;
	}
	else if (input == aiAnimBehaviour::aiAnimBehaviour_CONSTANT)
	{
		output = CustomStruct::CGameAnimationBehaviour::ANIMATION_BEHAVIOUR_CONSTANT;
	}
	else if (input == aiAnimBehaviour::aiAnimBehaviour_LINEAR)
	{
		output = CustomStruct::CGameAnimationBehaviour::ANIMATION_BEHAVIOUR_LINEAR;
	}
	else if (input == aiAnimBehaviour::aiAnimBehaviour_REPEAT)
	{
		output = CustomStruct::CGameAnimationBehaviour::ANIMATION_BEHAVIOUR_REPEAT;
	}
}

void _GGatherSingleAnimationDatas(const aiAnimation* animationNode, CustomStruct::CGameAnimationInfo& animationData)
{
	if (animationData.AnimationNodes.size() > 0)
	{
		animationData.AnimationNodes.clear();
	}

	UINT numChannels = animationNode->mNumChannels;
	for (UINT indexChannel = 0u; indexChannel < numChannels; indexChannel++)
	{
		const aiNodeAnim* tempChannel = animationNode->mChannels[indexChannel];
		if (tempChannel == nullptr || tempChannel->mNodeName.length < 1)
		{
			continue;
		}

		CustomStruct::CGameAnimationNodeInfo tempAnimationNodeInfo(_GTranslateString(tempChannel->mNodeName));

		for (UINT indexPosKey = 0u; indexPosKey < tempChannel->mNumPositionKeys && tempChannel->mPositionKeys != nullptr; indexPosKey++)
		{
			const aiVectorKey& tempChannelPosKey = tempChannel->mPositionKeys[indexPosKey];

			CustomStruct::CGameAnimationKey<CustomType::Vector3> tempPosKey;
			tempPosKey.Time = tempChannelPosKey.mTime;
			tempPosKey.Value = _GTranslateVector3(tempChannelPosKey.mValue);

			tempAnimationNodeInfo.PositionKeys.push_back(tempPosKey);
		}

		for (UINT indexRotKey = 0u; indexRotKey < tempChannel->mNumRotationKeys && tempChannel->mRotationKeys != nullptr; indexRotKey++)
		{
			const aiQuatKey& tempChannelRotKey = tempChannel->mRotationKeys[indexRotKey];

			CustomStruct::CGameAnimationKey<CustomType::Quaternion> tempRotKey;
			tempRotKey.Time = tempChannelRotKey.mTime;
			tempRotKey.Value = _GTranslateQuaternion(tempChannelRotKey.mValue);

			tempAnimationNodeInfo.RotationKeys.push_back(tempRotKey);
		}

		for (UINT indexSclKey = 0u; indexSclKey < tempChannel->mNumScalingKeys && tempChannel->mScalingKeys != nullptr; indexSclKey++)
		{
			const aiVectorKey& tempChannelSclKey = tempChannel->mScalingKeys[indexSclKey];

			CustomStruct::CGameAnimationKey<CustomType::Vector3> tempSclKey;
			tempSclKey.Time = tempChannelSclKey.mTime;
			tempSclKey.Value = _GTranslateVector3(tempChannelSclKey.mValue);

			tempAnimationNodeInfo.ScalingKeys.push_back(tempSclKey);
		}

		_GTranslateAnimationBehaviour(tempChannel->mPreState, tempAnimationNodeInfo.PreState);
		_GTranslateAnimationBehaviour(tempChannel->mPostState, tempAnimationNodeInfo.PostState);

		// TODO is this necessary?
		{
			std::sort(tempAnimationNodeInfo.PositionKeys.begin(), tempAnimationNodeInfo.PositionKeys.end());
			std::sort(tempAnimationNodeInfo.RotationKeys.begin(), tempAnimationNodeInfo.RotationKeys.end());
			std::sort(tempAnimationNodeInfo.ScalingKeys.begin(), tempAnimationNodeInfo.ScalingKeys.end());
		}
		
		animationData.AnimationNodes.push_back(tempAnimationNodeInfo);
	}
}

BOOL _GGatherAllAnimationDatas(const std::string& path, const aiScene* scene, std::map<std::string, CustomStruct::CGameAnimationInfo>& animationDatas)
{
	if (animationDatas.size() > 0)
	{
		animationDatas.clear();
	}

	BOOL result = TRUE;

	UINT animationDummyIndex = 0u;
	UINT numAnimationNodes = scene->mNumAnimations;
	for (UINT indexAnimationNode = 0u; indexAnimationNode < numAnimationNodes; indexAnimationNode++)
	{
		const aiAnimation* tempAnimationNode = scene->mAnimations[indexAnimationNode];
		if (tempAnimationNode == nullptr)
		{
			continue;
		}

		std::string tempAnimationNodeName;

		if (tempAnimationNode->mName.length > 0)
		{
			tempAnimationNodeName = _GTranslateString(tempAnimationNode->mName);
		}
		else
		{
			tempAnimationNodeName = std::string("DummyAnimation_") + std::to_string(animationDummyIndex);
			animationDummyIndex += 1u;
		}

		{
			auto tempFindAnimation = animationDatas.find(tempAnimationNodeName);
			if (tempFindAnimation != animationDatas.end())
			{
				result = FALSE;
				continue;
			}
		}

		{
			auto tempCurrentAnimation = animationDatas.insert_or_assign(tempAnimationNodeName, CustomStruct::CGameAnimationInfo());
			CustomStruct::CGameAnimationInfo& tempAnimationData = tempCurrentAnimation.first->second;
			tempAnimationData.Name				= path + "_" + tempAnimationNodeName;
			tempAnimationData.Duration			= tempAnimationNode->mDuration;
			tempAnimationData.TicksPerSecond	= tempAnimationNode->mTicksPerSecond;
			_GGatherSingleAnimationDatas(tempAnimationNode, tempAnimationData);
		}
	}

	return result;
}

CassimpManager* CassimpManager::m_AssimpManager = nullptr;
void CassimpManager::Initialize()
{
	if (CassimpManager::m_AssimpManager == nullptr)
	{
		CassimpManager::m_AssimpManager = new CassimpManager();
	}
	if (_GAssetImporter == nullptr)
	{
		_GAssetImporter = new Assimp::Importer();
	}
}
void CassimpManager::ShutDown()
{
	if (_GAssetImporter != nullptr)
	{
		_GAssetImporter->FreeScene();
		delete (_GAssetImporter);
		_GAssetImporter = nullptr;
	}
	if (CassimpManager::m_AssimpManager != nullptr)
	{
		delete (CassimpManager::m_AssimpManager);
		CassimpManager::m_AssimpManager = nullptr;
	}
}
CassimpManager::CassimpReadFileState CassimpManager::ReadDefaultMeshFile(const std::string& path, std::vector<CustomStruct::CSubMeshInfo>& subMesh, UINT& vertexStride, CHAR*& vertices, UINT& numVertices, std::vector<UINT>& indices, UINT& numIndices)
{
	if (vertices != nullptr)
	{
		delete[]vertices;
		vertices = nullptr;
	}
	if (indices.size() > 0)
	{
		indices.clear();
	}
	if (subMesh.size() > 0)
	{
		subMesh.clear();
	}
	vertexStride	= 0u;
	numVertices		= 0u;
	numIndices		= 0u;

	CassimpReadFileState result = CassimpReadFileState::ASSIMP_READ_FILE_STATE_FAILED;

	Assimp::Importer* impoter = _GAssetImporter;
	if (impoter == nullptr)
	{
		// TODO Do the error logging (did not create the instance of importer)
		return result;
	}

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.

	// Use SetPropertyInteger to modify config of importer
	//Assimp::Importer::SetProperty###();

	const aiScene* scene = impoter->ReadFile(
		path,
		aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
		aiProcess_MakeLeftHanded |
		aiProcess_Triangulate |
		aiProcess_RemoveComponent |
		aiProcess_GenSmoothNormals |
		aiProcess_SplitLargeMeshes |
		/*aiProcess_ImproveCacheLocality |*/
		aiProcess_RemoveRedundantMaterials |
		aiProcess_FixInfacingNormals |
		aiProcess_SortByPType |
		aiProcess_FindInvalidData |
		aiProcess_GenUVCoords |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_FlipUVs |
		aiProcess_FlipWindingOrder |
		aiProcess_GenBoundingBoxes);

	// If the import failed, report it
	if (scene == nullptr)
	{
		// TODO Do the error logging (importer.GetErrorString())
		impoter->FreeScene();
		return result;
	}

	if (!scene->HasMeshes())
	{
		result = CassimpReadFileState::ASSIMP_READ_FILE_STATE_ERROR;
		// TODO Scene does not contain meshes
		impoter->FreeScene();
		return result;
	}

	// Now we can access the file's contents.
	// Only access first mesh in scene.
	const CustomStruct::CRenderInputLayoutDesc* inputLayoutDesc; UINT inputLayoutNum;
	CustomStruct::CRenderInputLayoutDesc::GetEngineDefaultMeshInputLayouts(inputLayoutDesc, inputLayoutNum);
	_GTranslateDefaultMeshData(scene, subMesh, vertexStride, vertices, numVertices, indices, numIndices, inputLayoutDesc, inputLayoutNum);

	result = CassimpReadFileState::ASSIMP_READ_FILE_STATE_SUCCEED;
	// We're done. Everything will be cleaned up by the importer destructor
	impoter->FreeScene();
	return result;
}
//void CopyNodesWithMeshes(aiNode node, SceneObject targetParent, Matrix4x4 accTransform)
//{
//	SceneObject parent;
//	Matrix4x4 transform;
//
//	// if node has meshes, create a new scene object for it
//	if (node.mNumMeshes > 0) {
//		SceneObjekt newObject = new SceneObject;
//		targetParent.addChild(newObject);
//		// copy the meshes
//		CopyMeshes(node, newObject);
//
//		// the new object is the parent for all child nodes
//		parent = newObject;
//		transform.SetUnity();
//	}
//	else {
//		// if no meshes, skip the node, but keep its transformation
//		parent = targetParent;
//		transform = node.mTransformation * accTransform;
//	}
//
//	// continue for all child nodes
//	for (all node.mChildren) {
//		CopyNodesWithMeshes(node.mChildren[a], parent, transform);
//	}
//}
CassimpManager::CassimpReadFileState CassimpManager::ReadSkeletonMeshAndBoneFile(const std::string& path, std::vector<CustomStruct::CSubMeshInfo>& subMesh, UINT& vertexStride, CHAR*& vertices, UINT& numVertices, std::vector<UINT>& indices, UINT& numIndices, std::vector<CustomStruct::CGameBoneNodeInfo>& skeleton, std::map<std::string, SHORT>& boneIndexMap, std::vector<USHORT>& boneList)
{
	if (vertices != nullptr)
	{
		delete[]vertices;
		vertices = nullptr;
	}
	if (indices.size() > 0)
	{
		indices.clear();
	}
	if (subMesh.size() > 0)
	{
		subMesh.clear();
	}
	vertexStride = 0u;
	numVertices = 0u;
	numIndices = 0u;

	if (skeleton.size() > 0)
	{
		skeleton.clear();
	}
	if (boneIndexMap.size() > 0)
	{
		boneIndexMap.clear();
	}
	if (boneList.size() > 0)
	{
		boneList.clear();
	}

	CassimpReadFileState result = CassimpReadFileState::ASSIMP_READ_FILE_STATE_FAILED;

	Assimp::Importer* impoter = _GAssetImporter;
	if (impoter == nullptr)
	{
		// TODO Do the error logging (did not create the instance of importer)
		return result;
	}

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.

	// Use SetPropertyInteger to modify config of importer
	//Assimp::Importer::SetProperty###();

	//const aiScene* scene = impoter->ReadFile(
	//	path,
	//	aiProcess_CalcTangentSpace |
	//	aiProcess_JoinIdenticalVertices |
	//	aiProcess_MakeLeftHanded |
	//	aiProcess_Triangulate |
	//	aiProcess_RemoveComponent |
	//	aiProcess_GenSmoothNormals |
	//	aiProcess_SplitLargeMeshes |
	//	/*aiProcess_LimitBoneWeights |*/
	//	aiProcess_RemoveRedundantMaterials |
	//	aiProcess_FixInfacingNormals |
	//	aiProcess_PopulateArmatureData |
	//	aiProcess_SortByPType |
	//	aiProcess_FindInvalidData |
	//	aiProcess_GenUVCoords |
	//	aiProcess_OptimizeMeshes |
	//	aiProcess_FlipUVs |
	//	aiProcess_FlipWindingOrder |
	//	/*aiProcess_SplitByBoneCount |*/
	//	/*aiProcess_Debone |*/
	//	aiProcess_GenBoundingBoxes);

	const aiScene* scene = impoter->ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);

	// If the import failed, report it
	if (scene == nullptr)
	{
		// TODO Do the error logging (importer.GetErrorString())
		impoter->FreeScene();
		return result;
	}

	if (!scene->HasMeshes())
	{
		result = CassimpReadFileState::ASSIMP_READ_FILE_STATE_ERROR;
		// TODO Do the error logging (importer.GetErrorString())
		impoter->FreeScene();
		return result;
	}

	// Now we can access the file's contents.
	const CustomStruct::CRenderInputLayoutDesc* inputLayoutDesc; UINT inputLayoutNum;
	CustomStruct::CRenderInputLayoutDesc::GetEngineSkeletonMeshInputLayouts(inputLayoutDesc, inputLayoutNum);
#if 0
	INT tempRootNode = -1;
	result = _GGatherBoneDatas(scene, skeleton, boneList, tempRootNode) == TRUE ? CassimpReadFileState::ASSIMP_READ_FILE_STATE_SUCCEED : CassimpReadFileState::ASSIMP_READ_FILE_STATE_ERROR;

	if (result == CassimpReadFileState::ASSIMP_READ_FILE_STATE_SUCCEED)
	{
		rootNode = static_cast<UINT>(tempRootNode);
		_GTranslateSkeletonMeshData(scene, subMesh, vertexStride, vertices, numVertices, indices, numIndices, skeleton, boneList, inputLayoutDesc, inputLayoutNum);
	}
#else
	result = _GGatherSkeletonMeshAllNodeStructures(scene, subMesh, vertexStride, vertices, numVertices, indices, numIndices, skeleton, boneIndexMap, boneList, inputLayoutDesc, inputLayoutNum) == TRUE ? CassimpReadFileState::ASSIMP_READ_FILE_STATE_SUCCEED : CassimpReadFileState::ASSIMP_READ_FILE_STATE_ERROR;
#endif
	// We're done. Everything will be cleaned up by the importer destructor
	impoter->FreeScene();
	return result;
}
CassimpManager::CassimpReadFileState CassimpManager::ReadSkeletonAnimationFile(const std::string& path, std::map<std::string, std::map<std::string, CustomStruct::CGameAnimationInfo>>& animationDatas)
{
	CassimpReadFileState result = CassimpReadFileState::ASSIMP_READ_FILE_STATE_FAILED;

	if (animationDatas.find(path) != animationDatas.end())
	{
		return result;
	}

	Assimp::Importer* impoter = _GAssetImporter;
	if (impoter == nullptr)
	{
		// TODO Do the error logging (did not create the instance of importer)
		return result;
	}

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll
	// probably to request more postprocessing than we do in this example.

	// Use SetPropertyInteger to modify config of importer
	//Assimp::Importer::SetProperty###();

	//const aiScene* scene = impoter->ReadFile(
	//	path,
	//	aiProcess_MakeLeftHanded |
	//	aiProcess_RemoveComponent |
	//	aiProcess_RemoveRedundantMaterials |
	//	aiProcess_PopulateArmatureData);

	const aiScene* scene = impoter->ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_MakeLeftHanded);

	// If the import failed, report it
	if (scene == nullptr)
	{
		// TODO Do the error logging (importer.GetErrorString())
		impoter->FreeScene();
		return result;
	}

	if (!scene->HasAnimations())
	{
		result = CassimpReadFileState::ASSIMP_READ_FILE_STATE_ERROR;
		// TODO Do the error logging (importer.GetErrorString())
		impoter->FreeScene();
		return result;
	}

	// Skeleton reader
	//std::vector<CustomStruct::CGameBoneNodeInfo> sceneSkeleton;
	//std::vector<UINT> sceneBoneList;
	//UINT rootNode = 0u;
	//INT tempRootNode = -1;
	//result = _GGatherBoneDatas(scene, sceneSkeleton, sceneBoneList, tempRootNode);
	//rootNode = static_cast<UINT>(tempRootNode);

	// Now we can access the file's contents.
	auto tempInsertResult = animationDatas.insert_or_assign(path, std::map<std::string, CustomStruct::CGameAnimationInfo>());
	std::map<std::string, CustomStruct::CGameAnimationInfo>& tempAnimationContainer = tempInsertResult.first->second;

	result = _GGatherAllAnimationDatas(path, scene, tempAnimationContainer) == TRUE ? CassimpReadFileState::ASSIMP_READ_FILE_STATE_SUCCEED : CassimpReadFileState::ASSIMP_READ_FILE_STATE_ERROR;

	// We're done. Everything will be cleaned up by the importer destructor
	impoter->FreeScene();
	return result;
}
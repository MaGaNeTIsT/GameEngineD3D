#pragma once

#include <CoreMinimal.h>
#include <Base/DataStructure/Text/String.h>
#include <Base/DataStructure/Container/Array.h>
#include <Base/DataStructure/Container/Map.h>
#include <RenderCommon.h>
#include <MeshAsset/MeshAsset.h>
#include <SkeletonAsset/SkeletonAsset.h>
#include <AnimationAsset/SkeletonAnimationAsset.h>

class aiScene;
class aiMesh;
class aiBone;

namespace PigeonEngine
{
	class CAssimpManager : public EManagerBase
	{

		EClass(CAssimpManager, EManagerBase)

	public:
		enum CReadFileStateType
		{
			ASSIMP_READ_FILE_STATE_SUCCEED,
			ASSIMP_READ_FILE_STATE_FAILED,
			ASSIMP_READ_FILE_STATE_ERROR
		};
	private:
		struct StoredMeshLayoutDesc
		{
			EVertexLayoutType	BaseVertexLayout;
			UINT				TryStoredLayoutNum;
			UINT				TryStoredLayoutSlot[EMesh::MeshVertexLayoutPartMaxNum];
		};
		BOOL FindMeshesAndVertexLayouts(const aiScene* InScene, TArray<const aiMesh*>& OutMeshes, TArray<TArray<RShaderSemanticType>>& OutLayouts, TArray<BOOL>& OutIsSkeletonMesh);

		template<typename TDataType, typename TMeshType>
		void TryAddMeshVertexPart(const TArray<const TDataType*>& InDatas, const TArray<UINT>& InDataElementNum, const EVertexLayoutType InStoredLayoutBaseType, const UINT* InStoredLayoutSlots, const UINT InStoredLayoutNum, const UINT InStrideIn32Bits, const UINT InSuccessAddMaxNum, TMeshType& OutMesh);
		void TryAddMeshSkinPart(const TArray<const aiBone*>& InBones, const UINT InVertexNum, const EVertexLayoutType InStoredLayoutBaseType, const UINT* InStoredLayoutSlots, const UINT InStoredLayoutNum, const UINT InSuccessAddMaxNum, ESkinnedMesh& OutMesh);
		TArray<StoredMeshLayoutDesc> GetShouldStoredMeshLayoutDescriptions(const RShaderSemanticType* InLayouts, const UINT InLayoutNum);
		void TranslateAssimpMeshToEngineMeshInternal(const RShaderSemanticType* InEngineLayouts, const UINT InEngineLayoutNum, const TArray<const aiMesh*>& InMeshes, TArray<EStaticMesh>& OutMeshes);
		void TranslateAssimpMeshToEngineMeshInternal(const RShaderSemanticType* InEngineLayouts, const UINT InEngineLayoutNum, const TArray<const aiMesh*>& InMeshes, TArray<ESkinnedMesh>& OutMeshes);
		BOOL GatherAllBoneNodeDatas(const aiScene* InScene, TArray<EBoneData>& OutBones, TMap<EString, SHORT>& OutBoneIndices);
	public:
		virtual void Initialize()override;
		virtual void ShutDown()override;
	public:
		CReadFileStateType ReadStaticMeshFile(const EString& InPath, TArray<EStaticMesh>& OutMeshes);
		CReadFileStateType ReadSkeletonFile(const EString& InPath, ESkeleton& OutSkeleton);
		CReadFileStateType ReadSkinnedMeshFile(const EString& InPath, TArray<ESkinnedMesh>& OutMeshes);
		CReadFileStateType ReadSkeletonAnimationFile(const EString& path, TArray<ESkeletonAnimationClip>& OutSkeletonAnimationClips);
		//CReadFileStateType ReadSkeletonMeshAndBoneFile(const EString& path, EMesh::ESubmeshPart& subMesh, UINT& vertexStride, CHAR*& vertices, UINT& numVertices, TArray<UINT>& indices, UINT& numIndices, ESkeletonInfo& skeleton, TMap<EString, SHORT>& boneIndexMap, TArray<USHORT>& boneList);

		CLASS_MANAGER_VIRTUAL_SINGLETON_BODY(CAssimpManager)

	};
};
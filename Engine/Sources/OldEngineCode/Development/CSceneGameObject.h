/*
#pragma once

#include "../../../../Entry/EngineMain.h"
#include "../../EngineGame/Headers/CGameObject.h"

class CSceneGameObject : public CGameObject
{
public:
	virtual void	Init()override;
	virtual void	Uninit()override;
	virtual void	Update()override;
	virtual void	FixedUpdate()override;
#ifdef _DEVELOPMENT_EDITOR
public:
	class CMeshRendererComponent*	GetMeshRendererComponentNotConst();
	class CMeshComponent*			GetMeshComponentNotConst();
public:
	virtual void	SelectedEditorUpdate()override;
protected:
	enum DefaultMaterialType
	{
		DefaultMaterialType_DefaultLit				= 0,
		DefaultMaterialType_DefaultLit_SkeletonMesh	= 1,
		DefaultMaterialType_Anisotropic				= 2,
		DefaultMaterialType_ClearCoat				= 3,
		DefaultMaterialType_Cloth					= 4,
		DefaultMaterialType_ClothAnisotropic		= 5,
		DefaultMaterialType_Count
	};
protected:
	class CMeshRendererComponent*	m_MeshRendererComponent;
	class CMeshComponent*			m_MeshComponent;
	INT32								m_CurrentMaterialType;
	INT32								m_PreviousMaterialType;
	BOOL32							m_LoadBaseModel;
	CHAR							m_LoadCustomModelPath[512];
	INT32								m_CurrentMeshType;
	INT32								m_PreviousMeshType;
	CHAR							m_SaveMaterialPath[512];
	CHAR							m_LoadMaterialPath[512];
#endif
public:
	CSceneGameObject(const BOOL32& active, const class CScene* scene);
	virtual ~CSceneGameObject();
};
*/
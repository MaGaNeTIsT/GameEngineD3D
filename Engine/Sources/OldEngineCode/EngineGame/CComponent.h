/*
#pragma once

#include "../../../../Entry/EngineMain.h"
#include "../../EngineBase/Headers/CBaseType.h"
#include "../../EngineRender/RenderBase/Headers/CRenderStructCommon.h"
#include "./CObjectManager.h"

class CTransform : public CObjectBase
{
public:
	void	InitTransform(const CustomType::Vector3& worldPosition, const CustomType::Quaternion& worldRotation, const CustomType::Vector3& worldScale);
	void	InitTransform(const CustomType::Vector3& worldPosition);
	void	InitTransform(const CustomType::Quaternion& worldRotation);
	void	InitTransform(const CustomType::Vector3& worldPosition, const CustomType::Quaternion& worldRotation);
	void	InitTransform(const CustomType::Vector3& worldPosition, const CustomType::Vector3& worldScale);
public:
	class CGameObjectTransformBase*			GetGameObject()const;
	CTransform*								GetParent()const;
	UINT32									GetChildrenNum()const;
	CTransform*								GetChildByUniqueID(const ULONGLONG& id)const;
	std::vector<CTransform*>				GetChildrenList()const;
	std::map<ULONGLONG, CTransform*>		GetChildrenMap()const;
public:
	void	SetParent(CTransform* parent);
	void	AddChild(CTransform* child);
public:
	void	RemoveParent();
	void	RemoveChild(CTransform* child);
	void	RemoveChildByUniqueID(const ULONGLONG& id);
	void	RemoveAllChildren();
public:
	BOOL32	IsBelongGameObject(const ULONGLONG& gameObjectID)const;
	BOOL32	HasGameObject()const;
	BOOL32	HasParent()const;
	BOOL32	HasChild()const;
	BOOL32	IsParent(const ULONGLONG& parentID)const;
	BOOL32	IsChild(const ULONGLONG& childID)const;
protected:
	void	BaseAddChild(CTransform* child);
	void	BaseRemoveChildByUniqueID(const ULONGLONG& id);
	BOOL32	BaseFindChildByUniqueID(const ULONGLONG& id);
	BOOL32	BaseModifyChildByUniqueID(const ULONGLONG& id, CTransform*& output);
	void	ConnectParentAndChild(CTransform* parent, CTransform* child);
	void	DisconnectParentAndChild(CTransform* parent, CTransform* child);
	void	CalculateCurrentLocalTransform(CTransform* newParent);
protected:
	class CGameObjectTransformBase*		m_GameObject;
	CTransform*							m_Parent;
	std::map<ULONGLONG, CTransform*>	m_Children;
public:
	const CustomType::Vector3&		GetLocalPosition()const;
	const CustomType::Quaternion&	GetLocalRotation()const;
	const CustomType::Vector3&		GetLocalScale()const;
public:
	void	SetLocalPosition(const CustomType::Vector3& localPosition);
	void	SetLocalRotation(const CustomType::Quaternion& localRotation);
	void	SetLocalScale(const CustomType::Vector3& localScale);
public:
	void	SetWorldPosition(const CustomType::Vector3& worldPosition);
	void	SetWorldRotation(const CustomType::Quaternion& worldRotation);
	void	SetWorldScale(const CustomType::Vector3& worldScale);
public:
	CustomType::Vector3		GetWorldPosition()const;
	CustomType::Quaternion	GetWorldRotation()const;
	CustomType::Vector3		GetWorldScale()const;
protected:
	void	RecursionWorldPosition(const CTransform* parent, CustomType::Vector3& position)const;
	void	RecursionWorldRotation(const CTransform* parent, CustomType::Quaternion& rotation)const;
	void	RecursionWorldScale(const CTransform* parent, CustomType::Vector3& scale)const;
protected:
	CustomType::Vector3			m_LocalPosition;
	CustomType::Quaternion		m_LocalRotation;
	CustomType::Vector3			m_LocalScale;
public:
	CustomType::Vector3		GetForwardVector()const;
	CustomType::Vector3		GetUpVector()const;
	CustomType::Vector3		GetRightVector()const;
	CustomType::Matrix4x4	GetLocalToWorldMatrix()const;
	CustomType::Matrix4x4	GetWorldToLocalMatrix()const;
#ifdef _DEVELOPMENT_EDITOR
public:
	void	SetEditorRotation(const CustomType::Vector3& v);
#endif
#ifdef _DEVELOPMENT_EDITOR
public:
	void	SelectedEditorUpdate();
protected:
	BOOL32	m_RealTimeChangeValue;
	FLOAT	m_EditorRotation[3];
#endif
public:
	CTransform();
	virtual ~CTransform() {}
protected:
	friend class CGameObjectTransformBase;
};

class CBaseComponent : public CObjectBase
{
public:
	virtual void	Init()			= 0;
	virtual void	Uninit()		= 0;
	virtual void	Update() {}
	virtual void	FixedUpdate() {}
#ifdef _DEVELOPMENT_EDITOR
public:
	virtual void	SelectedEditorUpdate() {}
#endif
public:
	const BOOL32&		NeedUpdate()const { return (this->m_NeedUpdate); }
	const BOOL32&		NeedFixedUpdate()const { return (this->m_NeedFixedUpdate); }
public:
	const BOOL32&		IsActive()const { return (this->m_Active); }
	void			Active() { this->m_Active = TRUE; }
	void			Inactive() { this->m_Active = FALSE; }
protected:
	BOOL32	m_NeedUpdate;
	BOOL32	m_NeedFixedUpdate;
	BOOL32	m_Active;
public:
	const class CGameObject* GetGameObject()const { return (this->m_GameObject); }
	void SetGameObject(const class CGameObject* gameObject)
	{
		if (gameObject != nullptr)
		{
			this->m_GameObject = gameObject;
		}
	}
protected:
	const class CGameObject*	m_GameObject;
public:
	CBaseComponent(const BOOL32& active, const BOOL32& needUpdate, const BOOL32& needFixedUpdate)
	{
		this->m_NeedUpdate		= needUpdate;
		this->m_NeedFixedUpdate	= needFixedUpdate;
		this->m_Active			= active;
		this->m_GameObject		= nullptr;
	}
	virtual ~CBaseComponent() {}
};

class CRenderComponent : public CBaseComponent
{
public:
	virtual void	Init()override {}
	virtual void	Uninit()override {}
	virtual void	Update()override {}
	virtual void	FixedUpdate()override {}
public:
	virtual void	Draw()const			= 0;
	virtual void	DrawExtra()const	= 0;
public:
	CRenderComponent(const BOOL32& active, const BOOL32& needUpdate, const BOOL32& needFixedUpdate) : CBaseComponent(active, needUpdate, needFixedUpdate) {}
	virtual ~CRenderComponent() {}
};
*/
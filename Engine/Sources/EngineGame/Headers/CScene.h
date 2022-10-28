#pragma once

#include "../../../../Entry/EngineMain.h"
#include "./CGameObject.h"
//
//class CSceneNode
//{
//
//public:
//
//	BOOL IsHiddenInGame()const;
//	void SetHiddenInGame(BOOL HiddenInGame);
//
//	std::string GetNodeName()const;
//	void SetNodeName(const std::string& NewNodeName);
//
//private:
//
//	BOOL bHiddenInGame;
//	std::string Name;
//
//};

class CScene
{
public:
	enum SceneLayout
	{
		LAYOUT_TERRAIN		= 0,
		LAYOUT_OPAQUE		= 1,
		LAYOUT_TRANSPARENT	= 2,
		LAYOUT_COUNT
	};
protected:
	CGameObject*						m_MainCamera;
	std::map<ULONGLONG, CGameObject*>	m_Lights;
	std::map<ULONGLONG, CGameObject*>	m_GameObjects[SceneLayout::LAYOUT_COUNT];
	std::map<ULONGLONG, CGameObject*>	m_AllObjectList;
public:
	CScene();
	virtual ~CScene();
public:
	template <typename T>
	T* AddCamera()
	{
		ULONGLONG id;
		if (this->m_MainCamera)
		{
			id = this->m_MainCamera->GetGameObjectID();
			this->m_MainCamera->Uninit();
			{
				this->m_AllObjectList[id] = NULL;
				this->m_AllObjectList.erase(id);
			}
			delete (this->m_MainCamera);
		}
		this->m_MainCamera = new T();
		this->m_MainCamera->SetScene(this);
		this->m_MainCamera->Active();
		this->m_MainCamera->Init();
		id = this->m_MainCamera->GetGameObjectID();
		(this->m_AllObjectList)[id] = this->m_MainCamera;
		return  (reinterpret_cast<T*>(this->m_MainCamera));
	}
	template <typename T>
	T* AddLight()
	{
		CGameObject* gameObject = new T();
		gameObject->SetScene(this);
		gameObject->Active();
		gameObject->Init();
		ULONGLONG id = gameObject->GetGameObjectID();
		(this->m_Lights)[id] = gameObject;
		(this->m_AllObjectList)[id] = gameObject;
		return  (reinterpret_cast<T*>(gameObject));
	}
	template <typename T>
	T* AddGameObject(const UINT& layout)
	{
		if (layout >= SceneLayout::LAYOUT_COUNT)
			return NULL;
		CGameObject* gameObject = new T();
		gameObject->SetScene(this);
		gameObject->Active();
		gameObject->Init();
		ULONGLONG id = gameObject->GetGameObjectID();
		(this->m_GameObjects[layout])[id] = gameObject;
		(this->m_AllObjectList)[id] = gameObject;
		return  (reinterpret_cast<T*>(gameObject));
	}
	template <typename T>
	T* GetGameObjectFirst(const UINT& layout)const
	{
		if (layout >= SceneLayout::LAYOUT_COUNT)
			return NULL;
		if (this->m_GameObjects[layout].size() < 1)
			return NULL;
		for (const auto& obj : (this->m_GameObjects[layout]))
		{
			if ((obj.second) != NULL)
			{
				if (typeid(*(obj.second)) == typeid(T))
					return (reinterpret_cast<T*>(obj.second));
			}
		}
		return NULL;
	}
	template <typename T>
	T* GetGameObjectByIndex(const UINT& layout, const UINT& idx)const
	{
		if (layout >= SceneLayout::LAYOUT_COUNT)
			return NULL;
		if (this->m_GameObjects[layout].size() < 1)
			return NULL;
		UINT number = idx;
		for (const auto& obj : (this->m_GameObjects[layout]))
		{
			if ((obj.second) != NULL)
			{
				if (typeid(*(obj.second)) == typeid(T))
				{
					if (number == 0)
						return (reinterpret_cast<T*>(obj.second));
					else
						number -= 1;
				}
			}
		}
		return NULL;
	}
	template <typename T>
	std::vector<T*>	GetGameObjectAll(const UINT& layout)const
	{
		if (layout >= SceneLayout::LAYOUT_COUNT)
			return NULL;
		std::vector<T*> listObj;
		if (this->m_GameObjects[layout].size() < 1)
			return listObj;
		for (const auto& obj : (this->m_GameObjects[layout]))
		{
			if ((obj.second) != NULL)
			{
				if (typeid(*(obj.second)) == typeid(T))
					listObj.push_back((reinterpret_cast<T*>(obj.second)));
			}
		}
		return listObj;
	}
public:
	virtual void	Init();
	virtual void	Uninit();
	virtual void	Update();
	virtual void	FixedUpdate();
private:
	friend class CRenderPipeline;
};
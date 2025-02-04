/*
#pragma once

#include "../../../../Entry/EngineMain.h"
#include "./CComponent.h"
#include "./CGameBoundComponent.h"
#include "./CGameObject.h"
#include "./CScene.h"

namespace GameObjectUtility
{
	BOOL32 GetObjectHasBoundingBoxInView(CScene* scene, const UINT32& layout, std::vector<std::pair<CGameObject*, CGameBoundBaseComponent*>>& list)
	{
		if (layout >= CScene::SceneLayout::LAYOUT_COUNT)
		{
			return FALSE;
		}
		if (list.size() > 0)
		{
			list.clear();
		}
		std::vector<CGameObjectBase*> objects = scene->GetGameObjectAll(layout);
		for (const auto& obj : objects)
		{
			if ((obj) != nullptr)
			{
				CGameObject* tempGameObjectPtr = reinterpret_cast<CGameObject*>(obj);
				if (tempGameObjectPtr && tempGameObjectPtr->HasGameBoundComponent())
				{
					CGameBoundBaseComponent* boundComponent = tempGameObjectPtr->GetGameBoundComponent<CGameBoundBaseComponent>();
					std::pair<CGameObject*, CGameBoundBaseComponent*> tempObj(tempGameObjectPtr, boundComponent);
					list.push_back(tempObj);
				}
			}
		}
		return (list.size() > 0);
	}

	std::vector<std::pair<CGameObject*, CGameBoundBaseComponent*>> GetGameObjectsHasBoundingBoxInView(CScene* scene)
	{
		std::vector<std::pair<CGameObject*, CGameBoundBaseComponent*>> listObj;
		for (UINT32 i = 0; i < CScene::LAYOUT_COUNT; i++)
		{
			std::vector<std::pair<CGameObject*, CGameBoundBaseComponent*>> listObjTemp;
			if (GetObjectHasBoundingBoxInView(scene, i, listObjTemp))
			{
				auto iterStart = listObj.end();
				listObj.resize(listObj.size() + listObjTemp.size());
				listObj.insert(iterStart, listObjTemp.begin(), listObjTemp.end());
			}
		}
		return listObj;
	}

};
*/
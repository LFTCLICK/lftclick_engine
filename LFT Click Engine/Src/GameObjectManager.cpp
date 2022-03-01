// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameObjectManager.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Manages GameObjects
// History			:
// 2021/12/01		-	Added collider handeling and message sending
// ---------------------------------------------------------------------------
#include "pch.h"
#include "GameObjectManager.h"
#include "Components\Drawable.h"
#include "Components\Collider.h"
#include "Components\MeshCollider.h"
#include "Components\SquareCollider.h"
#include "Components\CircleCollider.h"

GameObjectManager::GameObjectManager()
{
	gameObjectList = std::list<GameObject*>();
}

void GameObjectManager::Update()
{
	//std::cout << "Size: " << gameObjectList.size() << std::endl;
	std::list<GameObject*>::iterator objIt = gameObjectList.begin();

	while (objIt != gameObjectList.end()) {
		GameObject* g = *objIt;

		if (g->isDeletable) {
			EventManager::getInstance().UnsubscribeFromAllEvents(g);
			objIt = gameObjectList.erase(objIt);
			delete g;
		}
		else {
			if (g->isActive) g->Update();
			++objIt;
		}
	}
}

void GameObjectManager::Start()
{
	//note that even if the object is inactive start is called
	for (GameObject* g : gameObjectList)
		g->Start();
}

void GameObjectManager::Draw()
{
	for (GameObject* g : gameObjectList)
	{
		if (g->isActive)
		{
			Drawable* s = g->getComponent<Drawable>();
			if (s != nullptr)
			{
				s->Draw();
			}
		}
	}
}

void GameObjectManager::DoCollision(GameObject* toCheckWith)
{
	for (GameObject* g : gameObjectList)
	{
		if (g->isActive && !g->isDeletable && g != toCheckWith)
		{
			Collider* s = dynamic_cast<Collider*>(g->getComponent<MeshCollider>());
			if (s == nullptr) 
				s = dynamic_cast<Collider*>(g->getComponent<SquareCollider>());
			if (s == nullptr)
				s = dynamic_cast<Collider*>(g->getComponent<CircleCollider>());
			if (s != nullptr) 
				s->CollisionCheck(toCheckWith);
		}
	}
}

void GameObjectManager::Deserialize(GameObjectFactory * gof, json j, bool isPrefab)
{
	this->gof = gof;
	json prefabsJSON = j["Prefabs"];
	for (json::iterator prefabs = prefabsJSON.begin(); prefabs != prefabsJSON.end(); ++prefabs)
	{
		GameObject* returned = gof->CreateObject(prefabs.value());
		returned->isActive = false;
		prefabList.push_back(returned);
	}
	prefabsJSON = j["Level"];
	for (json::iterator currentObj = prefabsJSON.begin(); currentObj != prefabsJSON.end(); ++currentObj)
	{
		GameObject* newOne = ClonePrefabOfTag(gof, currentObj.value()["object"], true);
		std::string newObjectTag = currentObj.value()["object"];
		json overrideList = currentObj.value()["overrides"];
		for (json::iterator c = overrideList.begin(); c != overrideList.end(); ++c)
		{
			json actualOverride = c.value();
			for (json::iterator realOverrides = actualOverride.begin(); realOverrides != actualOverride.end(); ++realOverrides)
			{
				int val = std::stoi(realOverrides.key());
				newOne->getRawComponentPointer(std::stoi(realOverrides.key()))->Deserialize(realOverrides.value(), newOne);
			}
		}
		newOne->Start();

	}
}

void GameObjectManager::AddGameObject(GameObject * go)
{
	gameObjectList.push_back(go);
}

void GameObjectManager::DeleteAll()
{
	for (GameObject* g : gameObjectList)
		delete g;
	for (GameObject* g : prefabList)
		delete g;
	gameObjectList.clear();
	prefabList.clear();
}

void GameObjectManager::DeleteObjectOfTag(std::string tag) 
{
	std::list<GameObject*>::iterator i = gameObjectList.begin();
	while (i != gameObjectList.end()) {
		if ((*i)->tag == tag)
			gameObjectList.erase(i++);  // alternatively, i = items.erase(i);
		else
			++i;
	}
}

GameObject * GameObjectManager::ClonePrefabOfTag(GameObjectFactory * gof, std::string tag, bool skipStart)
{
	for (GameObject* g : prefabList)
	{
		if (g->tag == tag)
		{
			GameObject* go = gof->CloneObject(g);
			if(!skipStart)
				go->Start();
			gameObjectList.push_back(go);
			return go;
		}
	}
	return nullptr;
}

GameObject * GameObjectManager::CloneObject(GameObject * go)
{
	GameObject* toReturn = gof->CloneObject(go);
	toReturn->tag = go->tag;
	gameObjectList.push_back(toReturn);
	toReturn->Start();
	return toReturn;

}

GameObject * GameObjectManager::FindObjectOfTag(std::string tag)
{
	for (GameObject* g : gameObjectList)
	{
		if (g->tag == tag)
		{
			return g;
		}
	}
	return nullptr;
}

void GameObjectManager::BroadcastMessage(Message * m)
{
	for (GameObject* g : gameObjectList)
	{
		g->HandleMessage(m);
	}
}

GameObjectManager::~GameObjectManager()
{
	DeleteAll();

	gameObjectList.clear();
}

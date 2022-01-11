// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameObjectManager.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Manages GameObjects
// History			:
// 2021/12/01		-	Added collider handeling and message sending
// ---------------------------------------------------------------------------
#include "GameObjectManager.h"
#include <iostream>
#include "Drawable.h"
#include "Collider.h"
GameObjectManager::GameObjectManager()
{
	gameObjectList = std::list<GameObject*>();
}

void GameObjectManager::Update()
{
	for (GameObject* g : gameObjectList)
	{
		if (g->isActive)
			g->Update();
	}
}

void GameObjectManager::Start()
{
	for (GameObject* g : gameObjectList)
		g->Start();
}

void GameObjectManager::Draw()
{
	for (GameObject* g : gameObjectList)
	{
		if (g->isActive)
		{
			Drawable* s = dynamic_cast<Drawable*>(g->hasComp(Component::DRAWABLE));
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
		if (g->isActive)
		{
			Collider* s = dynamic_cast<Collider*>(g->hasComp(Component::MESH_COLLIDER));
			if (s == nullptr)
				s = dynamic_cast<Collider*>(g->hasComp(Component::SQUARE_COLLLIDER));
			if (s != nullptr)
			{
				s->CollisionCheck(toCheckWith);
			}
		}
	}
}

void GameObjectManager::Deserialize(GameObjectFactory * gof, json j, bool isPrefab)
{
	this->gof = gof;
	for (json::iterator level = j.begin(); level != j.end(); ++level)
	{
		for (json::iterator currentObj = level.value().begin(); currentObj != level.value().end(); ++currentObj)
		{
			GameObject* returned = gof->CreateObject(currentObj.value());
			if (isPrefab)//make it inactive if it's a prefab
				returned->isActive = false;
			gameObjectList.push_back(returned);
		}
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
	gameObjectList.clear();
}

GameObject * GameObjectManager::CloneObjectOfTag(GameObjectFactory * gof, std::string tag)
{
	for (GameObject* g : gameObjectList)
	{
		if (g->tag == tag)
		{
			GameObject* go = gof->CloneObject(g);
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
	gameObjectList.clear();
}

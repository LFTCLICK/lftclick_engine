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
#include "Components\SquareCollider.h"
#include "Components\CircleCollider.h"
#include "CollisionResolution.h"
#include "GameManager.h"

const int COLLIDER_IDS[] = { Component::SQUARE_COLLLIDER, Component::CIRCLE_COLLIDER };
const int NUMBER_OF_COLLIDERS = 2;

GameObjectManager::GameObjectManager()
{
	gameObjectList = std::list<GameObject*>();
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

void GameObjectManager::Update()
{
	//std::cout << "Size: " << gameObjectList.size() << std::endl;
	auto objIt = gameObjectList.begin();

	while (objIt != gameObjectList.end()) {
		GameObject* g = *objIt;

		if (g->isDeletable) {
			g_EventManager->UnsubscribeFromAllEvents(g);
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
#ifdef _DEBUG
	static bool debugDraw = false;
	ImGui::Checkbox("Draw Colliders", &debugDraw);
#endif

	for (GameObject* g : gameObjectList)
	{
		if (g->isActive)
		{
#ifdef _DEBUG
			if (!debugDraw)
				continue;

			CircleCollider* c = g->getComponent<CircleCollider>();
			
			if (c != nullptr)
			{
				c->DebugDraw();
			}

			SquareCollider* sq = g->getComponent<SquareCollider>();

			if (sq != nullptr)
			{
				sq->DebugDraw();
			}
#endif
		}
	}
}

void GameObjectManager::ProcessCollision()
{
	TriggerCollisionMessage* toPassTrigger = new TriggerCollisionMessage();
	CollisionMessage* toPassCollider = new CollisionMessage();

	for (auto mainListItt = gameObjectList.begin(); mainListItt != gameObjectList.end(); ++mainListItt)
	{
		if (!(*mainListItt)->isActive || (*mainListItt)->isDeletable)
			continue;
		if (!(*mainListItt)->hasNonStaticCollider)
			break;
		Transform* mainTrans = (*mainListItt)->trans;
		for (int mainColliderIndex=0; mainColliderIndex<NUMBER_OF_COLLIDERS; ++mainColliderIndex)
		{
			Collider* s = (Collider* )(*mainListItt)->getRawComponentPointer(COLLIDER_IDS[mainColliderIndex]);
			if (s == nullptr)
				continue;
			for (auto innerLoop = std::next(mainListItt); innerLoop != gameObjectList.end(); ++innerLoop)
			{
				if (!(*innerLoop)->isActive || (*innerLoop)->isDeletable)
					continue;
				Transform* toCheckTrans = (*innerLoop)->trans;
				for (Collider* toCheckWith : (*innerLoop)->colliders)
				{
					//Collider* toCheckWith = (Collider*)(*innerLoop)->getRawComponentPointer(COLLIDER_IDS[innerColliderIndex]);
					//if (toCheckWith == nullptr)
					//	continue;
					//if (toCheckWith->isStatic && s->isStatic) //this should never happen now
					//	continue;
					bool didCollide = false;
					DirectX::SimpleMath::Vector2 deltaPos = CollisionResolution::ProcessCollision(s, mainTrans->position, toCheckWith, toCheckTrans->position, didCollide);
					if (didCollide)
					{
						if (s->isTrigger || toCheckWith->isTrigger)
						{
							*toPassTrigger = TriggerCollisionMessage(toCheckWith);
							(*mainListItt)->HandleMessage(toPassTrigger);
							*toPassTrigger = TriggerCollisionMessage(s);
							(*innerLoop)->HandleMessage(toPassTrigger);
						}
						else
						{
							if (!s->isStatic && !toCheckWith->isStatic)
								deltaPos *= .5f;
							*toPassCollider = CollisionMessage(toCheckWith, -deltaPos);
							(*mainListItt)->HandleMessage(toPassCollider);
							*toPassCollider = CollisionMessage(s, deltaPos);
							(*innerLoop)->HandleMessage(toPassCollider);
						}
					}
				}
			}
		}
	}
	delete toPassCollider;
	delete toPassTrigger;
}

void GameObjectManager::DoCollision(GameObject* toCheckWith)
{
	/*for (std::list<GameObject*>::iterator it = gameObjectList.begin(); it != gameObjectList.end();)
	{
		GameObject* toCheck = *it;
		++it;
		for (std::list<GameObject*>::iterator innerIt = it; innerIt != gameObjectList.end(); ++innerIt)
		{
			CollisionResolution
		}
	}*/
	for (GameObject* g : gameObjectList)
	{
		if (g->isActive && !g->isDeletable && g != toCheckWith)
		{
			Collider* s = dynamic_cast<Collider*>(g->getComponent<SquareCollider>());
			if (s != nullptr)
				s->CollisionCheck(toCheckWith);
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
	}

	if (j.contains("Map")) 
	{
		json map = j["Map"];
		int objectSize = map["objectSize"];

		std::wstring mapFilePath = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(map["path"]);
		Gdiplus::Bitmap img(mapFilePath.c_str());

		Gdiplus::Color currentColor;
		int width = img.GetWidth(), height = img.GetHeight();
		
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				img.GetPixel(x, y, &currentColor);
				if (currentColor.GetValue() != Gdiplus::Color::Black) {
					
					std::stringstream stream;
					stream << std::hex << (int)currentColor.GetValue();
					std::string colorHexString = stream.str().substr(2, 6);

					if (map["key"].contains(colorHexString)) {
						GameObject* obj = ClonePrefabOfTag(gof, map["key"][colorHexString]);
						int mapX = (x - (width / 2)) * objectSize, mapY = (y - (height / 2)) * objectSize * -1;
						obj->getComponent<Transform>()->SetPos(mapX, mapY);
					}
				}
			}
		}
	}

	for (GameObject* g : gameObjectList)
		g->Start();
}

void GameObjectManager::AddGameObject(GameObject * go)
{
	if(go->hasNonStaticCollider)
		gameObjectList.push_front(go);
	else
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
	refGameObjListByPrefabAsKey.clear();
}

void GameObjectManager::DeleteObjectOfTag(std::string tag) 
{
	auto i = gameObjectList.begin();
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

			refGameObjListByPrefabAsKey[tag].push_back(go);

			AddGameObject(go);
			return go;
		}
	}
	return nullptr;
}

GameObject * GameObjectManager::CloneObject(GameObject * go)
{
	GameObject* toReturn = gof->CloneObject(go);
	toReturn->tag = go->tag;
	AddGameObject(toReturn);
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

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
#include "Components\Enemy.h"
#include "Components\EnemySpawner.h"
#include "CollisionResolution.h"

const int COLLIDER_IDS[] = { Component::SQUARE_COLLLIDER, Component::CIRCLE_COLLIDER };
const int NUMBER_OF_COLLIDERS = 2;

GameObjectManager::GameObjectManager() : gameObjectList(std::list<GameObject*>())
{
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

void GameObjectManager::Update()
{
	//std::cout << "Size: " << gameObjectList.size() << std::endl;
	auto objIt = gameObjectList.begin();

	while (objIt != gameObjectList.end()) {
		GameObject* g = *objIt;
		bool finished = false;

		if (g->isDeletable) {
			finished = true;
			for (auto comp : g->comps) {
				if (comp.second != nullptr && !(comp.second->isFinishedDeleting)) {
					finished = false;
					break;
				}
			}
		}

		if (finished) {
			if (g->tag == "zombie" || g->tag == "ghost" || g->tag == "enemy")
				g_GameManager->MonsterCountMinus();

			//EnemySpawner* es = g->getComponent<EnemySpawner>();
			//if (es != nullptr && !es->spawnAroundPlayer && g_GameManager->totalSpawners > 0)
				//g_GameManager->totalSpawners--;

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
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::Checkbox("Debug Drawing", &debugDraw);
		ImGui::EndMainMenuBar();
	}
#endif

	for (GameObject* g : gameObjectList)
	{
		if (g->isActive && g->isOnScreen)
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
	DirectX::SimpleMath::Vector2 cameraPos = DirectX::SimpleMath::Vector2(g_GameManager->mainCamera->getPos());

	DirectX::SimpleMath::Vector2 distanceFromCenter;
	float maxDistance;
	float maxScreenSize = g_Renderer->GetWidth() > g_Renderer->GetHeight() ? (float)g_Renderer->GetWidth() : (float)g_Renderer->GetHeight();
	maxScreenSize /= 2;
	maxScreenSize += 5;
	bool firstItt = true;
	auto gameObjectListEnd = gameObjectList.end();
	for (auto mainListItt = gameObjectList.begin(); mainListItt != gameObjectListEnd; ++mainListItt)
	{
		if (!(*mainListItt)->isActive || (*mainListItt)->isDeletable)
			continue;
		if (!(*mainListItt)->hasNonStaticCollider)
			break;
		if (firstItt)
		{
			(*mainListItt)->isOnScreen = false;
			distanceFromCenter = cameraPos - (*mainListItt)->myTransform->position;
			distanceFromCenter.x = std::fabsf(distanceFromCenter.x);
			distanceFromCenter.y = std::fabsf(distanceFromCenter.y);
			maxDistance = distanceFromCenter.x > distanceFromCenter.y ? distanceFromCenter.x : distanceFromCenter.y;
			if (maxDistance - std::max((*mainListItt)->myTransform->scale.x, (*mainListItt)->myTransform->scale.y) < maxScreenSize)
				(*mainListItt)->isOnScreen = true;
		}

		Transform* mainTrans = (*mainListItt)->myTransform;
		for (int mainColliderIndex = 0; mainColliderIndex < NUMBER_OF_COLLIDERS; ++mainColliderIndex)
		{
			Collider* s = (Collider*)(*mainListItt)->getRawComponentPointer(COLLIDER_IDS[mainColliderIndex]);
			if (s == nullptr)
				continue;
			for (auto innerLoop = std::next(mainListItt); innerLoop != gameObjectListEnd; ++innerLoop)
			{
				if (!(*innerLoop)->isActive || (*innerLoop)->isDeletable)
					continue;
				Transform* toCheckTrans = (*innerLoop)->myTransform;
				if (firstItt)
				{
					(*innerLoop)->isOnScreen = false;
					distanceFromCenter = cameraPos - toCheckTrans->position;
					distanceFromCenter.x = std::fabsf(distanceFromCenter.x);
					distanceFromCenter.y = std::fabsf(distanceFromCenter.y);
					maxDistance = distanceFromCenter.x > distanceFromCenter.y ? distanceFromCenter.x : distanceFromCenter.y;
					if (maxDistance - std::max(toCheckTrans->scale.x, toCheckTrans->scale.y) < maxScreenSize)
					{
						(*innerLoop)->isOnScreen = true;
					}
				}

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
		firstItt = false;
	}
	delete toPassCollider;
	delete toPassTrigger;
}

void GameObjectManager::DoCollision(GameObject* toCheckWith)
{
	/*for (std::list<GameObject*>::iterator it = gameObjectList.begin(); it != gameObjectList.gameObjectListEnd();)
	{
		GameObject* toCheck = *it;
		++it;
		for (std::list<GameObject*>::iterator innerIt = it; innerIt != gameObjectList.gameObjectListEnd(); ++innerIt)
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

void GameObjectManager::Deserialize(GameObjectFactory* gof, json j, bool isPrefab)
{
	this->gof = gof;

	json prefabsJSON = j["Prefabs"];
	for (json::iterator prefabs = prefabsJSON.begin(); prefabs != prefabsJSON.end(); ++prefabs)
	{
		GameObject* returned = gof->CreateObject(prefabs.value());
		returned->isActive = false;
		prefabList.push_back(returned);
	}


	if (j.contains("Map"))
	{
		json map = j["Map"];
		int objectSize = map["objectSize"], doubleObjectSize = objectSize * 2, halfObjectSize = objectSize / 2;

		std::wstring mapFilePath = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(map["path"]);
		Gdiplus::Bitmap img(mapFilePath.c_str());

		Gdiplus::Color currentColor;
		int clientWidth = img.GetWidth(), clientHeight = img.GetHeight();
		g_GameManager->mapHeight = (clientHeight * objectSize / 2) + objectSize;
		g_AStarTerrain->clientWidth = clientWidth;
		g_AStarTerrain->clientHeight = clientHeight;
		g_AStarTerrain->nodeMap = new Node * *[clientHeight];
		g_AStarTerrain->tileSize = objectSize;
		g_AStarTerrain->terrain = new int* [clientHeight];
		for (int y = 0; y < clientHeight; y++)
		{
			g_AStarTerrain->nodeMap[y] = new Node * [clientWidth];
			g_AStarTerrain->terrain[y] = new int[clientWidth];
			for (int x = 0; x < clientWidth; x++)
			{
				Node* current = new Node{};
				current->pos = { y,x };
				current->vec3 = DirectX::SimpleMath::Vector3((x - (clientWidth / 2)) * objectSize, (y - (clientHeight / 2)) * objectSize * -1, 0.0f);
				g_AStarTerrain->nodeMap[y][x] = current;
				g_AStarTerrain->terrain[y][x] = 0;
			}
		}

		float zHelper = g_GameManager->mapHeight / 2.0f;
		for (int y = 0; y < clientHeight; y++) {
			for (int x = 0; x < clientWidth; x++) {
				img.GetPixel(x, y, &currentColor);
				if (currentColor.GetValue() != Gdiplus::Color::Black) {

					std::stringstream stream;
					stream << std::hex << (int)currentColor.GetValue();
					std::string colorHexString = stream.str().substr(2, 6);

					if (map["key"].contains(colorHexString)) {
						GameObject* obj = ClonePrefabOfTag(gof, map["key"][colorHexString], true);

						Transform* myTransform = obj->getComponent<Transform>();
						SquareCollider* collider = obj->getComponent<SquareCollider>();

						int mapX = (x - (clientWidth / 2)) * objectSize, mapY = (y - (clientHeight / 2)) * objectSize * -1, scaleX = myTransform->scale.x;

						if (collider && !collider->isTrigger)
						{
							g_AStarTerrain->terrain[y][x] = -1;
							//obj->getComponent<SquareCollider>()->isTrigger = true;
						}

						if (scaleX % doubleObjectSize == 0) {
							mapX += halfObjectSize;
							mapY += halfObjectSize;

							if (collider && !collider->isTrigger)
							{
								g_AStarTerrain->terrain[y - 1][x] = -1;
								g_AStarTerrain->terrain[y - 1][x + 1] = -1;
								g_AStarTerrain->terrain[y][x + 1] = -1;
							}
						}

						myTransform->SetPos(mapX, mapY);
						if(map["key"][colorHexString]!="rug")
							myTransform->zPos = 5 + ((myTransform->position.y + g_GameManager->mapHeight) / zHelper);

						// Think I basically handled this up above, but leaving this code in just in case

						/*if (map["key"][colorHexString] == "junk")
						{
							obj->getComponent<Transform>()->Move(objectSize / 2, objectSize / 2);
							g_AStarTerrain->terrain[y-1][x] = -1;
							g_AStarTerrain->terrain[y-1][x+1] = -1;
							g_AStarTerrain->terrain[y][x+1] = -1;
							//obj->getComponent<SquareCollider>()->isTrigger = true;

						}*/
					}
				}
			}
		}
		g_AStarTerrain->Init();
	}

	if (j.contains("Level"))
	{
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
	}
	for (GameObject* g : gameObjectList) {
		g->Start();
	}
}

void GameObjectManager::AddGameObject(GameObject* go)
{
	if (go->hasNonStaticCollider)
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

GameObject* GameObjectManager::ClonePrefabOfTag(GameObjectFactory* gof, std::string tag, bool skipStart)
{
	// Cull most distant enemy if one is being spawned and there are too many
	if (tag == "zombie" || tag == "ghost" || tag == "enemy") {
		g_GameManager->MonsterCountPlus();
		if (g_GameManager->monsterCount > MAX_ENEMIES) {
			GameObject* furthestEnemy = nullptr;
			auto playerPos = g_GameManager->playerTrans->CurrentPos();
			float furthestDistance = 0;
			for (GameObject* g : gameObjectList) {
				if (g->tag == "zombie" || g->tag == "ghost" || g->tag == "enemy") {
					Transform* myTransform = g->getComponent<Transform>();
					float distance = DirectX::SimpleMath::Vector2::DistanceSquared(playerPos, myTransform->CurrentPos());
					if (furthestDistance < distance) {
						furthestDistance = distance;
						furthestEnemy = g;
					}
				}
			}
			if (furthestDistance != 0) {
				furthestEnemy->isDeletable = true;
			}
		}
	}


	for (GameObject* g : prefabList)
	{
		if (g->tag == tag)
		{
			GameObject* go = gof->CloneObject(g);

			if (!skipStart)
				go->Start();

			refGameObjListByPrefabAsKey[tag].push_back(go);

			AddGameObject(go);
			return go;
		}
	}

	return nullptr;
}

GameObject* GameObjectManager::CloneObject(GameObject* go)
{
	GameObject* toReturn = gof->CloneObject(go);
	toReturn->tag = go->tag;
	AddGameObject(toReturn);
	toReturn->Start();
	return toReturn;

}

GameObject* GameObjectManager::FindObjectOfTag(std::string tag)
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

void GameObjectManager::BroadcastMessage(Message* m)
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

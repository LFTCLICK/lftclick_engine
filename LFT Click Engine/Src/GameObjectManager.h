// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameObjectManager.h
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Manages GameObjects
// History			:
// 2021/12/01		-	Added collider handeling and message sending
// ---------------------------------------------------------------------------
#pragma once

#include <list>
#include <json.hpp>
#include "Components\GameObject.h"
#include "GameObjectFactory.h"
#include <SDL.h>
#include <codecvt>
#include <iomanip>
#include <sstream>

// Need GDIPlus for map loading, and GDIPlus needs min/max functions, which <windows.h> hates.
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#pragma comment(lib,"Gdiplus.lib")
#include <gdiplus.h>
#undef max
#undef min

using json = nlohmann::json;

class GameObjectManager
{
public:
	GameObjectManager();

	void Update();
	void Start();
	void Draw();
	void ProcessCollision();
	void DoCollision(GameObject * toCheckWith);
	void Deserialize(GameObjectFactory * gof, json j, bool isPrefab = false);
	void AddGameObject(GameObject* go);
	void DeleteAll();
	void DeleteObjectOfTag(std::string tag);
	GameObject* ClonePrefabOfTag(GameObjectFactory * gof, std::string tag, bool skipStart = false);
	GameObject* CloneObject(GameObject* go);
	GameObject* FindObjectOfTag(std::string tag);
	void BroadcastMessage(Message* m);
	~GameObjectManager();

private:
	GameObjectFactory* gof;

	std::list<GameObject*> gameObjectList;
	std::list<GameObject*> prefabList;

	// Gdiplus startup nonsense
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
};

extern std::unique_ptr<GameObjectManager> g_GameObjManager;
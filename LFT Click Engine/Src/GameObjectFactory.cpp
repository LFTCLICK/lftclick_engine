// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	GameObjectFactory.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/10/28
// Purpose			:	Virtual functions for GameObject class
// History			:
// 2021/10/29		-	Added Drawable comp
// 2021/12/01		-	Added collider
// ---------------------------------------------------------------------------
#include "pch.h"
#include "GameObjectFactory.h"
#include "Components/Component.h"
#include "Components/Transform.h"
#include "Components/Drawable.h"
#include "Components/MeshCollider.h"
#include "Components/SquareCollider.h"
#include "Components/Camera.h"
#include "Components/Audible.h"
#include "Components/SpriteAnimator.h"
#include "Components/Player.h"
#include "Components/Bullet.h"
#include <json.hpp>
#include <string>
#include <fstream>
#include <iostream>
using json = nlohmann::json;

GameObjectFactory::GameObjectFactory()
{
}


GameObject * const GameObjectFactory::CreateObject(json curentObj)
{
	GameObject* toReturn = new GameObject();
	std::string path = curentObj["prefab"];

	std::fstream pefabFile(path);
	json prefab;
	pefabFile >> prefab;
	pefabFile.close();
	for (json::iterator type = prefab.begin(); type != prefab.end(); ++type)
	{
		toReturn->Add(CreateComp(type, toReturn));
	}
	toReturn->tag = curentObj["tag"];
	return toReturn;
}

GameObject * const GameObjectFactory::CloneObject(GameObject* toClone)
{
	return toClone->Clone();
}

Component * const GameObjectFactory::CreateComp(json::iterator type, GameObject* currentObj)
{
	//Not sure if there's a nicer way to do this, perhaps a parallel array/map?
	Component * currentComp = nullptr;
	int compID = std::stoi(type.key());
	std::cout << compID << std::endl;
	if (compID == Component::TRANSFORM)
		currentComp = new Transform();
	else if (compID == Component::DRAWABLE)
		currentComp = new Drawable();
	else if (compID == Component::MESH_COLLIDER)
		currentComp = new MeshCollider();
	else if (compID == Component::SQUARE_COLLLIDER)
		currentComp = new SquareCollider();
	else if (compID == Component::CAMERA)
		currentComp = new Camera();
	else if (compID == Component::AUDIBLE)
		currentComp = new Audible();
	else if (compID == Component::SPRITE_ANIMATOR)
		currentComp = new SpriteAnimator();
	else if (compID == Component::PLAYER)
		currentComp = new Player();
	else if (compID == Component::BULLET)
		currentComp = new Bullet();
	currentComp->Deserialize(type.value(), currentObj);
	return currentComp;
}

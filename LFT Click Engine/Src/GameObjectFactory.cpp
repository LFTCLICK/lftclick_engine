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
#include "Components/Gun.h"
#include "Components/FollowMouse.h"
#include <json.hpp>
#include <string>
#include <fstream>
#include <iostream>
using json = nlohmann::json;

GameObjectFactory::GameObjectFactory()
{
}


GameObject * const GameObjectFactory::CreateObject(json currentObj)
{
	GameObject* toReturn = new GameObject();
	std::string path = currentObj["prefab"];

	std::fstream prefabFile(path);
	json prefab;
	prefabFile >> prefab;
	prefabFile.close();
	for (json::iterator type = prefab.begin(); type != prefab.end(); ++type)
	{
		toReturn->Add(CreateComp(type, toReturn));
	}
	toReturn->tag = currentObj["tag"];
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
		currentComp = new Transform(type.value(), currentObj);
	else if (compID == Component::DRAWABLE)
		currentComp = new Drawable(type.value(), currentObj);
	else if (compID == Component::MESH_COLLIDER)
		currentComp = new MeshCollider(type.value(), currentObj);
	else if (compID == Component::SQUARE_COLLLIDER)
		currentComp = new SquareCollider(type.value(), currentObj);
	else if (compID == Component::CAMERA)
		currentComp = new Camera(type.value(), currentObj);
	else if (compID == Component::AUDIBLE)
		currentComp = new Audible(type.value(), currentObj);
	else if (compID == Component::SPRITE_ANIMATOR)
		currentComp = new SpriteAnimator(type.value(), currentObj);
	else if (compID == Component::PLAYER)
		currentComp = new Player(type.value(), currentObj);
	else if (compID == Component::BULLET)
		currentComp = new Bullet(type.value(), currentObj);
	else if (compID == Component::GUN)
		currentComp = new Gun(type.value(), currentObj);
	else if (compID == Component::FOLLOW_MOUSE)
		currentComp = new FollowMouse(type.value(), currentObj);
	return currentComp;
}

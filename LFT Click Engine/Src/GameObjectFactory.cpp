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
#include "Components/SquareCollider.h"
#include "Components/CircleCollider.h"
#include "Components/Camera.h"
#include "Components/Audible.h"
#include "Components/SpriteAnimator.h"
#include "Components/Player.h"
#include "Components/Bullet.h"
#include "Components/Gun.h"
#include "Components/FollowMouse.h"
#include "Components/Damageable.h"
#include "Components/Enemy.h"
#include "Components/Door.h"
#include "Components/EnemySpawner.h"
#include "Components/WoodPile.h"
#include "Components/LevelGenerator.h"
#include "Components/Interactable.h"
#include "Components/Vortex.h"
#include "Components/ScrollingCredits.h"

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

	std::ifstream prefabFile(path);
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

	if (compID == Component::TRANSFORM)
		currentComp = new Transform();
	else if (compID == Component::DRAWABLE)
		currentComp = new Drawable();
	//else if (compID == Component::MESH_COLLIDER)
	//	currentComp = new MeshCollider();
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
	else if (compID == Component::GUN)
		currentComp = new Gun();
	else if (compID == Component::FOLLOW_MOUSE)
		currentComp = new FollowMouse();
	else if (compID == Component::DAMAGEABLE)
		currentComp = new Damageable();
	else if (compID == Component::CIRCLE_COLLIDER)
		currentComp = new CircleCollider();
	else if (compID == Component::ENEMY)
		currentComp = new Enemy();
	else if (compID == Component::DOOR_OBJ)
		currentComp = new Door();
	else if (compID == Component::ENEMY_SPAWNER)
		currentComp = new EnemySpawner();
	else if (compID == Component::WOOD_PILE)
		currentComp = new WoodPile();
	else if (compID == Component::LEVEL_GENERATOR)
		currentComp = new LevelGenerator();
	else if (compID == Component::INTERACTABLE)
		currentComp = new Interactable();
	else if (compID == Component::VORTEX)
		currentComp = new Vortex();
	else if (compID == Component::CREDITS)
		currentComp = new ScrollingCredits();

	currentComp->Deserialize(type.value(), currentObj);

	return currentComp;
}

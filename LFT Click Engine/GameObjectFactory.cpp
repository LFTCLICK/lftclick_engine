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
#include "GameObjectFactory.h"
#include "Components/Component.h"
#include "Components/Transform.h"
#include "Components/Drawable.h"
#include "Components/MeshCollider.h"
#include "Components/SquareCollider.h"
#include "Components/Camera.h"
#include "Components/AnimationHelper.h"
#include <json.hpp>
#include <string> 
#include <fstream>
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
	//commenting old stuff out in case the friend change isn't liked
	//GameObject* toReturn = new GameObject();
	//toReturn->tag = toClone->tag;
	//Component* currentComp = toClone->getComponent<Transform>();
	//if (currentComp != nullptr)
	//	toReturn->Add(((Transform*)currentComp)->Clone(toReturn));
	//currentComp = toClone->getComponent<Drawable>();
	//if (currentComp != nullptr)
	//	toReturn->Add(((Drawable*)currentComp)->Clone(toReturn));
	//currentComp = toClone->getComponent<Drawable>();
	//if (currentComp != nullptr)
	//	toReturn->Add(((MeshCollider*)currentComp)->Clone(toReturn));
	//currentComp = toClone->getComponent<SquareCollider>();
	//if (currentComp != nullptr)
	//	toReturn->Add(((SquareCollider*)currentComp)->Clone(toReturn));
	//currentComp = toClone->getComponent<Camera>();
	//if (currentComp != nullptr)
	//	toReturn->Add(((Camera*)currentComp)->Clone(toReturn));
	//currentComp = toClone->getComponent<AnimationHelper>();
	//if (currentComp != nullptr)
	//	toReturn->Add(((AnimationHelper*)currentComp)->Clone(toReturn));
	return toClone->Clone();
}

Component * const GameObjectFactory::CreateComp(json::iterator type, GameObject* currentObj)
{
	Component * currentComp = nullptr;
	int compID = std::stoi(type.key());
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
	else if (compID == Component::ANIMATION_HELPER)
		currentComp = new AnimationHelper(type.value(), currentObj);
	return currentComp;
}

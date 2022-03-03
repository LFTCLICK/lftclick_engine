#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	LevelGenerator.h
// Author			:	Chris Fitch
// Creation Date	:	2022/03/02
// Purpose			:	Generates the level
// History			: 
// ---------------------------------------------------------------------------

#include "GameObject.h"
#include "Transform.h"
#include "Component.h"
#include "GameObjectManager.h"
#include <json.hpp>

#define OBJ_DISTANCE 80

using json = nlohmann::json;
class LevelGenerator : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::BULLET; };
	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;

	virtual Component* Clone(GameObject* newParent);
	LevelGenerator() {
		gom = &GameObjectManager::getInstance();
		gof = &GameObjectFactory::getInstance();
	};

public:
	void Create(std::string tag, int row, int column);

private:
	GameObject* parent;
	GameObjectManager* gom;
	GameObjectFactory* gof;

	std::vector<std::vector<int>> layout;
	std::map<int, std::string> key;
};
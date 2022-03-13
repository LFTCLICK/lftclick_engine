// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	LevelGenerator.cpp
// Author			:	Chris Fitch
// Creation Date	:	2022/03/02
// Purpose			:	Generates the level
// History			: 
// ---------------------------------------------------------------------------
#include "pch.h"
#include "LevelGenerator.h"
#include "FrameRateController.h"

void LevelGenerator::Start()
{
	int i = layout.size() * OBJ_DISTANCE / 2;
	for (auto row : layout) {
		i -= OBJ_DISTANCE;
		int j = 0;
		for (auto objectTypeNum : row) {
			j += OBJ_DISTANCE;
			if (objectTypeNum > 0)
				Create(key[objectTypeNum], j, i);
		}
	}
	parent->isDeletable = true;
}

void LevelGenerator::Update()
{
}

void LevelGenerator::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
	//for (auto row = std::begin(j["layout"]); row != std::end(j["layout"]); row++) {
	for (auto row : j["layout"]) {
		std::vector<int> currentRow;
		//for (auto objectTypeNum = std::begin(row.value()); objectTypeNum != std::end(row.value()); objectTypeNum++) {
		for (auto objectTypeNum : row) {
			currentRow.push_back(objectTypeNum);
		}
		layout.push_back(currentRow);
	}

	int i = 0;
	for (auto tag : j["key"]) key[i++] = tag;
}

void LevelGenerator::Create(std::string tag, int x, int y) {
	GameObject* obj = gom->ClonePrefabOfTag(gof, tag);
	obj->getComponent<Transform>()->SetPos(x - 2500, y);
}

Component* LevelGenerator::Clone(GameObject* newParent)
{
	LevelGenerator* toReturn = new LevelGenerator();
	toReturn->parent = newParent;
	toReturn->layout = layout;
	toReturn->key = key;
	return toReturn;
}
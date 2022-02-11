// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Audible.h
// Author			:	Christian Fitch
// Creation Date	:	2022/02/10
// Purpose			:	Component to play sounds using audio manager
// History			:
// ---------------------------------------------------------------------------
#pragma once

#include "GameObject.h"
#include "Component.h"
#include <json.hpp>
#include "../AudioManager.h"
#include "../EventManager.h"
#include <string>
#include <iostream>

class Audible : public Component
{
public:
	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::AUDIBLE; };
	virtual Component* Clone(GameObject* newParent);

	Audible() {}
	Audible(json j, GameObject* parent);

private:
	GameObject* parent;
	std::string backgroundMusic;
};
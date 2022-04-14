#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	ScrollingCredits.h
// Author			:	Vance Howald
// Creation Date	:	2022/04/14
// Purpose			:	Main player
// History			:
// ---------------------------------------------------------------------------

#include "GameObject.h"
#include "Component.h"
#include <json.hpp>
#include "Drawable.h"
#include "Audible.h"
#include "Camera.h"
#include "Gun.h"
#include "Enemy.h"
#include "AudioManager.h"
#include "GameManager.h"
#include "FrameRateController.h"
#include "../LuaManager.h"
#include "Components/Collider.h"
#include <sol/sol.hpp>
#include <cassert>
#include "AStarTerrain.h"

using json = nlohmann::json;
class ScrollingCredits : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::CREDITS; };
	static int getStaticCompId() { return ComponentType::CREDITS; };

	virtual Component* Clone(GameObject* newParent);
	ScrollingCredits(){};

	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;
public:
	void reset();

private:
	Transform* myTransform;
	DirectX::SimpleMath::Vector2 startingPos;
	float scrollSpeed;
	float endingY;
};
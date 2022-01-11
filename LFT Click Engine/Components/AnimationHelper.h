#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	AnimationHelper.h
// Author			:	Vance Howald
// Creation Date	:	2021/12/10
// Purpose			:	Animates grabables
// History			: 
// ---------------------------------------------------------------------------

#include "GameObject.h"
#include "Component.h"
#include <json.hpp>
#include "Drawable.h"

using json = nlohmann::json;
class AnimationHelper : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::ANIMATION_HELPER; };
	
	Component* clone(GameObject* newParent);
	AnimationHelper() {};
	AnimationHelper(json j, GameObject* parent);

private:
	GameObject* parent;
	Drawable* draw;
	float animationTime, offsetDenominator, timer;
};
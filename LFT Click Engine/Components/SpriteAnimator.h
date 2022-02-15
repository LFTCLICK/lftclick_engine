#pragma once
// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	SpriteAnimator.h
// Author			:	Vance Howald
// Creation Date	:	2021/2/13
// Purpose			:	Animator
// History			: 
// ---------------------------------------------------------------------------

#include "GameObject.h"
#include "Component.h"
#include <json.hpp>
#include "Drawable.h"

using json = nlohmann::json;
class SpriteAnimator : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::SPRITE_ANIMATOR; };

	virtual Component* Clone(GameObject* newParent);
	SpriteAnimator() {};
	SpriteAnimator(json j, GameObject* parent);

private:
	GameObject* parent;
	Drawable* draw;
	float frameTime, offset, timer;
	int spriteSheetWidth, spriteSheetHeight;
};
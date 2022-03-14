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

struct AnimationInfo {
	std::string name;
	int length, row;
	float frameDuration = 1.f;
};

class SpriteAnimator : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::SPRITE_ANIMATOR; };

	virtual Component* Clone(GameObject* newParent);
	SpriteAnimator() :
		wasMoving(false),
		isDamaged(false),
		wasDamaged(false),
		isDead(false),
		wasDead(false),
		currentFrame(0),
		currentAnimationIndex(0),
		direction("right")
	{}
	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;

public:
	void UpdateDirection();
	void UpdateState();
	void UpdateFrame();
	void SwitchAnimation(int index);
	void SwitchAnimation(std::string name);

	void Damage(float time);
	void Die(float time);
	void Revive();

private:
	GameObject* parent;
	Drawable* draw;
	Transform* trans;

	DirectX::SimpleMath::Vector2 oldPosition, position;

	std::map<int, AnimationInfo> animations;
	std::map<std::string, int> moveAnimationIndices;
	std::map<std::string, int> idleAnimationIndices;
	int damageAnimationIndex, deathAnimationIndex;

	std::string oldDirection, direction;

	float xOffset, yOffset, timer, damageTimer, damageTimeout, deathTimer, deathTimeout;
	int spriteSheetWidth, spriteSheetHeight, currentFrame, currentAnimationIndex;
	bool wasMoving, isDamaged, wasDamaged, isDead, wasDead;
};
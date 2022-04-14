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
	static int getStaticCompId() { return ComponentType::SPRITE_ANIMATOR; };

	virtual Component* Clone(GameObject* newParent);
	SpriteAnimator() :
		wasMoving(false),
		isDamaged(false),
		wasDamaged(false),
		isDead(false),
		wasDead(false),
		isInRange(false),
		wasInRange(false),
		currentFrame(0),
		currentAnimationIndex(0),
		direction("right"),
		currentPhase(0)
	{}
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;

public:
	void UpdateDirection();
	void UpdateState(bool forceUpdate = false);
	void UpdateFrame();
	void SwitchAnimation(int index);
	void SwitchAnimation(std::string name);

	void Damage(float time);
	void Die(float time);
	void Revive();
	void InRange();
	void OutOfRange();
	void SwitchPhase(int phase);

private:
	Drawable* drawable;
	Transform* myTransform;

	DirectX::SimpleMath::Vector2 oldPosition, position;

	std::map<int, AnimationInfo> animations;
	std::map<std::string, int> moveAnimationIndices;
	std::map<std::string, int> idleAnimationIndices;
	std::map<std::string, int> damagedIdleAnimationIndices;
	std::map<std::string, int> damagedMoveAnimationIndices;
	std::map<std::string, int> deathAnimationIndices;
	int damageAnimationIndex, deathAnimationIndex, interactRangeAnimationIndex, currentPhase;

	std::string oldDirection, direction;

	float xOffset, yOffset, timer, damageTimer, damageTimeout, deathTimer, deathTimeout;
	int spriteSheetWidth, spriteSheetHeight, currentFrame, currentAnimationIndex;
	bool wasMoving, isDamaged, wasDamaged, isDead, wasDead, isInRange, wasInRange;
};
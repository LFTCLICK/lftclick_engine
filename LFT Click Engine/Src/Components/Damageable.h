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
#include "Transform.h"
#include "SpriteAnimator.h"
#include "Audible.h"
#include "Component.h"
#include "GameObjectManager.h"
#include "EventManager.h"
#include "FrameRateController.h"
#include "Collider.h"
#include <json.hpp>

using json = nlohmann::json;
class Damageable : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::DAMAGEABLE; };
	static int getStaticCompId() { return ComponentType::DAMAGEABLE; };
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;
	virtual void HandleMessage(Message* e) override;

	virtual Component* Clone(GameObject* newParent);
	Damageable() : destroyOnDeath(true), playerHealth(1), maxHealth(1), damageTime(1.f), 
		deathTime(0.75f), timer(0), knockbackMod(0), inertiaMod(0.95), healthPreview(false),
		healthPreviewTime(2.0f) {};

public:
	void TakeDamage(int damage);

	int playerHealth;
	int maxHealth;

	// For knockback (not working yet)
	DirectX::SimpleMath::Vector2 velocity;
	float inertiaMod;
	bool destroyOnDeath;

private:
	Transform* myTransform;
	SpriteAnimator* anim;
	Audible* audio;
	Drawable* drawable;
	float damageTime, deathTime, timer, knockbackMod;

	bool healthPreview;
	float healthPreviewTime;
};
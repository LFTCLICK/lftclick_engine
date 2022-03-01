// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	AnimationHelper.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/12/10
// Purpose			:	Animates grabables
// History			:
// ---------------------------------------------------------------------------
#include "pch.h"
#include "Damageable.h"


void Damageable::Start()
{
	trans = parent->getComponent<Transform>();
	EventManager::getInstance().Subscribe(Message::DAMAGE_COLLISION, parent);
}

void Damageable::Update()
{
	if (destroyOnDeath && health < 1) parent->isDeletable = true;
}

void Damageable::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
	health = j["health"];
	if (j.contains("destroyOnDeath")) destroyOnDeath = j["destroyOnDeath"];
}

Component* Damageable::Clone(GameObject* newParent)
{
	Damageable* toReturn = new Damageable();
	toReturn->parent = newParent;
	toReturn->health = health;
	toReturn->acceleration = acceleration;
	toReturn->speed = speed;
	toReturn->direction = direction;
	toReturn->trans = trans;
	return toReturn;
}

void Damageable::HandleMessage(Message* e) {
	if (e->sourceObjectTag=="bullet" && e->id == Message::DAMAGE_COLLISION && health<10000000) {
		--health;
	}
}

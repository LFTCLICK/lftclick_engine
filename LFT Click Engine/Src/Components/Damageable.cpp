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
#include "Collider.h"

const Audible::SoundEvent AUDIO_ON_DAMAGE = Audible::SoundEvent::AUDIO_ON_DAMAGE;
const Audible::SoundEvent AUDIO_ON_DEATH = Audible::SoundEvent::AUDIO_ON_DEATH;

void Damageable::Start()
{
	trans = parent->getComponent<Transform>();
	anim = parent->getComponent<SpriteAnimator>();
	audio = parent->getComponent<Audible>();
	EventManager::getInstance().Subscribe(Message::TRIGGER_COLLISION, parent);
}

void Damageable::Update()
{
	if (destroyOnDeath && health < 1 && !parent->isDeletable) {
		if (anim != nullptr) anim->Die(deathTime);
		if (audio != nullptr) audio->PlaySoundsOnEvent(AUDIO_ON_DEATH);
		parent->isDeletable = true;
	}
	if (knockbackMod != 0 && (velocity.x != 0 || velocity.x != 0)) {
		trans->Move(velocity.x * knockbackMod, velocity.y * knockbackMod);
		velocity.x *= inertiaMod;
		velocity.y *= inertiaMod;
	}

}

void Damageable::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
	health = j["health"];
	if (j.contains("destroyOnDeath")) destroyOnDeath = j["destroyOnDeath"];
	if (j.contains("knockbackMod")) knockbackMod = j["knockbackMod"];
}

Component* Damageable::Clone(GameObject* newParent)
{
	Damageable* toReturn = new Damageable();
	toReturn->parent = newParent;
	toReturn->health = health;
	toReturn->inertiaMod = inertiaMod;
	toReturn->velocity = velocity;
	//toReturn->trans = trans;
	toReturn->knockbackMod = knockbackMod;
	toReturn->destroyOnDeath = destroyOnDeath;
	return toReturn;
}

void Damageable::HandleMessage(Message* e) {
	if (e->otherObject->parent->tag == "bullet") {
		//Transform* bulletTrans = e->otherObject != nullptr ? e->otherObject->getComponent<Transform>() : nullptr;
		//if (bulletTrans != nullptr) velocity = bulletTrans->lastMovement;
		TakeDamage(1);
	}
}

void Damageable::TakeDamage(int damage) {
	health -= damage;
	if (anim != nullptr) anim->Damage(damageTime);
	if (audio != nullptr) audio->PlaySoundsOnEvent(AUDIO_ON_DAMAGE);
}
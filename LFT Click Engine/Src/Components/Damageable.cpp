// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Damageable.cpp
// Author			:	Christian Fitch
// Creation Date	:	2021/12/10
// Purpose			:	Comp for handling damage
// History			:
// ---------------------------------------------------------------------------
#include "pch.h"
#include "Damageable.h"
#include "Collider.h"

const Audible::SoundEvent AUDIO_ON_DAMAGE = Audible::SoundEvent::AUDIO_ON_DAMAGE;
const Audible::SoundEvent AUDIO_ON_DEATH = Audible::SoundEvent::AUDIO_ON_DEATH;

void Damageable::Start()
{
	trans = componentOwner->getComponent<Transform>();
	anim = componentOwner->getComponent<SpriteAnimator>();
	audio = componentOwner->getComponent<Audible>();
	g_EventManager->Subscribe(Message::TRIGGER_COLLISION, componentOwner);
}

void Damageable::Update()
{
	if (destroyOnDeath && health < 1 && !componentOwner->isDeletable) {
		if (anim != nullptr) anim->Die(deathTime);
		if (audio != nullptr) audio->PlaySoundsOnEvent(AUDIO_ON_DEATH);
		componentOwner->isDeletable = true;
	}
	if (knockbackMod != 0 && (velocity.x != 0 || velocity.x != 0)) {
		trans->Move(velocity.x * knockbackMod, velocity.y * knockbackMod);
		velocity.x *= inertiaMod;
		velocity.y *= inertiaMod;
	}

}

void Damageable::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
	health = j["health"];
	if (j.contains("destroyOnDeath")) destroyOnDeath = j["destroyOnDeath"];
	if (j.contains("knockbackMod")) knockbackMod = j["knockbackMod"];
}

Component* Damageable::Clone(GameObject* newParent)
{
	Damageable* toReturn = new Damageable();
	toReturn->componentOwner = newParent;
	toReturn->health = health;
	toReturn->inertiaMod = inertiaMod;
	toReturn->velocity = velocity;
	//toReturn->trans = trans;
	toReturn->knockbackMod = knockbackMod;
	toReturn->destroyOnDeath = destroyOnDeath;
	return toReturn;
}

void Damageable::HandleMessage(Message* e) {
	if (e->otherObject->componentOwner->tag == "bullet") {
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
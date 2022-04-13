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

using namespace DirectX::SimpleMath;

void Damageable::Start()
{
	myTransform = componentOwner->getComponent<Transform>();
	anim = componentOwner->getComponent<SpriteAnimator>();
	audio = componentOwner->getComponent<Audible>();
	drawable = componentOwner->getComponent<Drawable>();

	g_EventManager->Subscribe(Message::TRIGGER_COLLISION, componentOwner);
}

void Damageable::Update()
{
	if (destroyOnDeath && playerHealth < 1 && !componentOwner->isDeletable) {
		if (anim != nullptr) anim->Die(deathTime);
		if (audio != nullptr) audio->PlaySoundsOnEvent(AUDIO_ON_DEATH);
		componentOwner->isDeletable = true;
	}
	if (knockbackMod != 0 && (velocity.x != 0 || velocity.x != 0)) {
		myTransform->Move(velocity.x * knockbackMod, velocity.y * knockbackMod);
		
		velocity *= inertiaMod;
	}


	if (drawable != nullptr && healthPreview && playerHealth >= 0)
	{
		healthPreviewTime -= g_FrameRateController->DeltaTime();

		if (healthPreviewTime <= 0.0f)
		{
			healthPreviewTime = 2.0f;
			healthPreview = false;
		}
		
		Color healthTextColor = Color::Lerp(Color(1, 0, 0, 1), Color(0, 1, 0, 1), 
			static_cast<float>(playerHealth) / maxHealth);
		
		drawable->HUD_DrawTextCenter(std::to_string(playerHealth), {0.0f, -50.0f}, healthTextColor);
	}

}

void Damageable::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
	playerHealth = j["health"];
	maxHealth = j["health"];
	if (j.contains("destroyOnDeath")) destroyOnDeath = j["destroyOnDeath"];
	if (j.contains("knockbackMod")) knockbackMod = j["knockbackMod"];
}

Component* Damageable::Clone(GameObject* newParent)
{
	Damageable* toReturn = new Damageable();
	toReturn->componentOwner = newParent;
	toReturn->playerHealth = playerHealth;
	toReturn->maxHealth = maxHealth;
	toReturn->inertiaMod = inertiaMod;
	toReturn->velocity = velocity;
	//toReturn->trans = trans;
	toReturn->knockbackMod = knockbackMod;
	toReturn->destroyOnDeath = destroyOnDeath;
	return toReturn;
}

void Damageable::HandleMessage(Message* e) {
	if (e->otherObject->componentOwner->tag == "bullet") {
		TakeDamage(1);
	}
}

void Damageable::TakeDamage(int damage) {
	playerHealth -= damage;
	if (anim != nullptr) anim->Damage(damageTime);
	if (audio != nullptr) audio->PlaySoundsOnEvent(AUDIO_ON_DAMAGE);

	healthPreview = true;
}
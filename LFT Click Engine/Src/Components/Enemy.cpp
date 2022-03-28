
#include "pch.h"
#include "Enemy.h"
#include "FrameRateController.h"
#include "EventManager.h"

void Enemy::Start()
{
	trans = componentOwner->getComponent<Transform>();
	g_EventManager->Subscribe(Message::COLLISION, componentOwner);
	switchToPlayer = false;
}

void Enemy::Update()
{
	DirectX::SimpleMath::Vector2 targetVector = -trans->CurrentPos();
	if (!switchToPlayer)
	{
		if(fabsf(trans->CurrentPos().y-targetBeforePlayer.y)<=20)
		{
			switchToPlayer = true;
		}
		else
		{
			targetVector += targetBeforePlayer;
		}
	}
	if (switchToPlayer)
	{
		targetVector += g_GameObjManager->FindObjectOfTag("player")->getComponent<Transform>()->CurrentPos();
	}
	targetVector = speed * g_FrameRateController->DeltaTime() / targetVector.Length() * targetVector;
	//if (hanginWithTheHomies)
	//	targetVector *= .1f;
	trans->Move(targetVector.x, targetVector.y);
	hanginWithTheHomies = false;
}

void Enemy::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
	attackTimer = j["attackTimer"];
	damage = j["damage"];
	speed = j["speed"];
}

Component* Enemy::Clone(GameObject * newParent)
{
	Enemy* toReturn = new Enemy();
	toReturn->attackTimer = attackTimer;
	toReturn->damage = damage;
	toReturn->speed = speed;
	toReturn->componentOwner = newParent;
	return toReturn;
}



void Enemy::HandleMessage(Message* e)
{
	if (e->id == Message::COLLISION)
	{
		CollisionMessage* cm = (CollisionMessage*)e;
		trans->Move(cm->deltaPos.x, cm->deltaPos.y);
	}
}


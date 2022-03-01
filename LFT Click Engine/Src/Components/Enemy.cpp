
#include "pch.h"
#include "Enemy.h"
#include "FrameRateController.h"

void Enemy::Start()
{
	trans = parent->getComponent<Transform>();
	EventManager::getInstance().Subscribe(Message::COLLISION, parent);
}

void Enemy::Update()
{
	DirectX::SimpleMath::Vector2 targetVector = GameObjectManager::getInstance().FindObjectOfTag("player")->getComponent<Transform>()->CurrentPos()-trans->CurrentPos();
	targetVector = (speed * FrameRateController::getInstance().DeltaTime())/(DirectX::SimpleMath::Vector2::Distance(DirectX::SimpleMath::Vector2(0, 0), targetVector)) * targetVector;
	trans->Move(targetVector.x, targetVector.y);
}

void Enemy::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;
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
	toReturn->parent = newParent;
	return toReturn;
}



void Enemy::HandleMessage(Message* e)
{
	if (e->id == Message::COLLISION)
	{
		CollisionMessage* cm = (CollisionMessage*)e;
		trans->Move(cm->deltaPos.x, cm->deltaPos.y);
		/*if (cm->deltaPos.y >= 0 && (cm->deltaPos.x<0.0000001 && cm->deltaPos.x > -0.00001))
		{
			isGrounded = true;
		}*/
		//std::cout << transform->GetPosXMVector().m128_f32[0] - lastGroundPos << std::endl;
		//lastGroundPos = transform->GetPosXMVector().m128_f32[0];
	}
}


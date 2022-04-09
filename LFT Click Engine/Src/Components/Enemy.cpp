#include "pch.h"
#include "Enemy.h"
#include "GameManager.h"
#include "GameObjectManager.h"
#include "FrameRateController.h"

using namespace DirectX::SimpleMath;

void Enemy::Start() 
{
	myTransform = parent->getComponent<Transform>();
	gun = parent->getComponent<Gun>();
	EventManager::getInstance().Subscribe(Message::COLLISION, parent);
	timer = 0.0;
	radius = 350.0;
	bulletCount = 5;
	targetPosition = Vector2();
}

void Enemy::Update()
{
	// Firing behaviours
	GameObject* playerObj = GameObjectManager::getInstance().FindObjectOfTag("player");
	if (playerObj == nullptr)
	{
		return;
	}
	Transform* playerTransform = playerObj->getComponent<Transform>();

	Vector2 playerPos = playerTransform->CurrentPos();
	Vector2 myPosition = myTransform->CurrentPos();

	float distance = Vector2::Distance(playerPos, myPosition);

	if (distance < radius) //distance < fov
	{
		int result = RandomGenerator(1, 2);
		timer = timer - FrameRateController::getInstance().DeltaTime();
		Vector2 target;

		switch (result)
		{
		case 1:
			target = playerTransform->CurrentPos() + Vector2(50, 50);

			if (timer < 0.0) //timer < 0.0
			{
				gun->Fire(0, target.x, target.y);
				timer = 0.2;
			}
			break;

		case 2:
			target = playerTransform->CurrentPos();

			if (timer < 0.0) //timer < 0.0
			{
				gun->Fire(0, target.x, target.y);
				timer = 0.2;
			}
			break;

		case 3:
			target = playerTransform->CurrentPos();

			if (timer < 0.0) //timer < 0.0
			{
				gun->FireSine(0, target.x, target.y);
				timer = 1.5;
			}
			break;
		}
		
	}
	else
	{
		
		while (Vector2::DistanceSquared(targetPosition, myPosition) < 10.0f * 10.0f)
		{
			targetPosition = Vector2(RandomGenerator(-1000, 1000), RandomGenerator(-1000, 1000));
		}
		
		Vector2 targetVector = targetPosition - myPosition;
		targetVector.Normalize();

		myPosition += enemySpeed * targetVector * FrameRateController::getInstance().DeltaTime();

		myTransform->SetPos(myPosition.x, myPosition.y);
	}
}

Component* Enemy::Clone(GameObject* newParent)
{
	Enemy* toReturn = new Enemy();
	toReturn->enemySpeed = enemySpeed;
	toReturn->parent = newParent;
	return(Component*)toReturn;
}

void Enemy::Deserialize(nlohmann::json j, GameObject* parent)
{
	enemySpeed = j["enemySpeed"];
	this->parent = parent;
}

void Enemy::HandleMessage(Message* e)
{

}

float Enemy::RandomGenerator(float lowest, float highest)
{
	return lowest + static_cast <float> (std::rand() / (static_cast <float> (RAND_MAX / (highest - lowest))));
}


#include "pch.h"
#include "Enemy.h"
#include "FrameRateController.h"
#include "EventManager.h"

void Enemy::Start()
{
	trans = componentOwner->getComponent<Transform>();
	playerTrans = g_GameObjManager->FindObjectOfTag("player")->getComponent<Transform>();
	g_EventManager->Subscribe(Message::COLLISION, componentOwner);
	switchToPlayer = false;
	zHelper = g_GameManager->mapHeight * 2.0f / 4.0f;
}

void Enemy::Update()
{
	if (pathTimer <= 0)
	{
		GridPos start = g_AStarTerrain->WorldToGridPos(trans->position);
		GridPos goal = g_AStarTerrain->WorldToGridPos(playerTrans->position);
		path.clear();
		path.push_back(playerTrans->position);
		int pathSize = g_AStarTerrain->ComputePath(&start, &goal, path);
		currentPathPos = path.begin();
		pathTimer = .2;
		float distance = DirectX::SimpleMath::Vector2::Distance(trans->position, playerTrans->position);
		if (distance > 400)
		{
			distance -= 400;
			pathTimer += .5 * distance / 400;
		}
	}

	DirectX::SimpleMath::Vector2 targetVector = *currentPathPos -trans->CurrentPos();

	float mag = (DirectX::SimpleMath::Vector2::Distance(DirectX::SimpleMath::Vector2(0, 0), targetVector));
	if (mag <= speed * g_FrameRateController->DeltaTime())
	{
		currentPathPos++;
		if (currentPathPos == path.end())
			pathTimer = 0;
		mag = (DirectX::SimpleMath::Vector2::Distance(DirectX::SimpleMath::Vector2(0, 0), targetVector));
	}
	targetVector = (speed * g_FrameRateController->DeltaTime())/mag * targetVector;
	//if (hanginWithTheHomies)
	//	targetVector *= .1f;
	trans->Move(targetVector.x, targetVector.y);
	pathTimer -= g_FrameRateController->DeltaTime();
	hanginWithTheHomies = false;

	//trans->zPos = trans->position.y / 1000.0f;
	//trans->zPos = (trans->position.y + g_GameManager->mapHeight) / 1000.0f;
	trans->zPos = (trans->position.y + g_GameManager->mapHeight) / zHelper;
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
		/*if (cm->deltaPos.y >= 0 && (cm->deltaPos.x<0.0000001 && cm->deltaPos.x > -0.00001))
		{
			isGrounded = true;
		}*/
		//std::cout << transform->GetPosXMVector().m128_f32[0] - lastGroundPos << std::endl;
		//lastGroundPos = transform->GetPosXMVector().m128_f32[0];
	}
}


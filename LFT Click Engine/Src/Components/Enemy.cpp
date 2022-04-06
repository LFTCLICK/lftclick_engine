
#include "pch.h"
#include "Enemy.h"
#include "FrameRateController.h"
#include "EventManager.h"
#include "Collider.h"
#include "SquareCollider.h"

void Enemy::Start()
{
	trans = componentOwner->getComponent<Transform>();
	playerTrans = g_GameObjManager->FindObjectOfTag("player")->getComponent<Transform>();
	g_EventManager->Subscribe(Message::COLLISION, componentOwner);
	switchToPlayer = false;
	zHelper = g_GameManager->mapHeight * 2.0f / 4.0f;
	reEvaluateStratTimer = -2;
	daylightDeathTime = Helpers::randWithinRange(DAYLIGHT_DEATH_TIME_MIN, DAYLIGHT_DEATH_TIME_MAX);
	daylightDeathChance = Helpers::randFloat0to1();
}

void Enemy::Update()
{
	if (!ignoreWalls)
	{
		if (reEvaluateStratTimer <= 0.0f)
		{
			DirectX::SimpleMath::Rectangle b = DirectX::SimpleMath::Rectangle(playerTrans->position.x, playerTrans->position.y, 1.0f, 1.0f);
			DirectX::SimpleMath::Rectangle c = DirectX::SimpleMath::Rectangle(trans->position.x, trans->position.y, 1.0f, 1.0f);
			if (!cabinRect.Intersects(b))//player is outside
			{
				if (!cabinRect.Intersects(c))//enemy is outside
				{
					doAstar = false;
					pathTimer = -1.0f;
					//do avoidance
				}
				else
				{
					doAstar = true;
				}
			}
			else
			{
				doAstar = true;
			}
			reEvaluateStratTimer = .15f;
		}
		DirectX::SimpleMath::Vector2 targetVector;
		if (doAstar)
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

			targetVector = *currentPathPos - trans->CurrentPos();

			float mag = (DirectX::SimpleMath::Vector2::Distance(DirectX::SimpleMath::Vector2(0, 0), targetVector));
			if (mag <= speed * g_FrameRateController->DeltaTime())
			{
				currentPathPos++;
				if (currentPathPos == path.end())
					pathTimer = 0;
				mag = (DirectX::SimpleMath::Vector2::Distance(DirectX::SimpleMath::Vector2(0, 0), targetVector));
			}
			targetVector = (speed * g_FrameRateController->DeltaTime()) / mag * targetVector;
		}
		else
		{
			targetVector = playerTrans->position - trans->CurrentPos();
			float mag = (DirectX::SimpleMath::Vector2::Distance(DirectX::SimpleMath::Vector2(0, 0), targetVector));
			targetVector = (speed * g_FrameRateController->DeltaTime()) / mag * targetVector;
		}

		//if (hanginWithTheHomies)
		//	targetVector *= .1f;
		trans->Move(targetVector.x, targetVector.y);
		pathTimer -= g_FrameRateController->DeltaTime();
		reEvaluateStratTimer -= g_FrameRateController->DeltaTime();
		hanginWithTheHomies = false;

		//trans->zPos = trans->position.y / 1000.0f;
		//trans->zPos = (trans->position.y + g_GameManager->mapHeight) / 1000.0f;
		trans->zPos = (trans->position.y + g_GameManager->mapHeight) / zHelper;

	} else {
		DirectX::SimpleMath::Vector2 targetVector;
		targetVector = playerTrans->CurrentPos() - trans->CurrentPos();
		targetVector.Normalize();
		trans->Move(targetVector.x, targetVector.y);
	}

	if (g_GameManager->harshLightOfDay == g_GameManager->day && daylightDeathChance < DAYLIGHT_DEATH_CHANCE) {
		float oldDaylightDeathTime = daylightDeathTime;
		std::cout << daylightDeathTime << " " << componentOwner->isDeletable << std::endl;
		daylightDeathTime -= g_FrameRateController->DeltaTime();
		if (daylightDeathTime <= 0 && oldDaylightDeathTime >= 0)
			componentOwner->getComponent<Damageable>()->health = 0;
	}
}

void Enemy::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
	attackTimer = j["attackTimer"];
	damage = j["damage"];
	speed = j["speed"];
	if (j.contains("ignoreWalls"))
		ignoreWalls = j["ignoreWalls"];
}

Component* Enemy::Clone(GameObject * newParent)
{
	Enemy* toReturn = new Enemy();
	toReturn->attackTimer = attackTimer;
	toReturn->damage = damage;
	toReturn->speed = speed;
	toReturn->ignoreWalls = ignoreWalls;
	toReturn->componentOwner = newParent;
	return toReturn;
}



void Enemy::HandleMessage(Message* e)
{
	if (e->id == Message::COLLISION)
	{
		if (!ignoreWalls)
		{
			CollisionMessage* cm = (CollisionMessage*)e;
			trans->Move(cm->deltaPos.x, cm->deltaPos.y);
		}
	}
}


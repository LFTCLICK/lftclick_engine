#include "pch.h"
#include "EnemySpawner.h"
#include "./FrameRateController.h"
#include "Enemy.h"
#include "GameManager.h"

void EnemySpawner::Start()
{
	positiveBounds = Vector2(1280, 7680);
	negativeBounds = Vector2(-1280, -7680);
	spawnTimer = timeBetweenPhases;
	myPos = componentOwner->getComponent<Transform>()->CurrentPos();
}

void EnemySpawner::Update()
{
	spawnTimer -= g_FrameRateController->DeltaTime();

	if (spawnTimer <= 0.0f)
	{
		spawnTimer = timeBetweenPhases;

		if (objectSpawnName == "zombie")
		{
			int toSpawn;
			if (g_GameManager->time > 0 && g_GameManager->time < 16.0f
				|| g_GameManager->time > 22.0 && g_GameManager->time < 24.0f)
				toSpawn = 20.0f;
			else
				toSpawn = 50.0f;

			for (int i = 0; i < toSpawn; i++)
			{
				GameObject* newObj = g_GameObjManager->ClonePrefabOfTag(g_GameObjFactory.get(), objectSpawnName);
				newObj->getComponent<Transform>()->SetPos(randomNumberGetter(), randomNumberGetter());
			}
		}
		
		if (objectSpawnName == "enemy")
		{
			GameObject* newObj = g_GameObjManager->ClonePrefabOfTag(g_GameObjFactory.get(), objectSpawnName);
			newObj->getComponent<Transform>()->SetPos(1.0f, 1.0f);
			//newObj->getComponent<Transform>()->SetPos(randomNumberGetter(), randomNumberGetter());
		}

		++x;
	}
}

void EnemySpawner::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	this->componentOwner = componentOwner;
	slope = j["slope"];
	c = j["c"];
	timeBetweenPhases = j["timeBetweenPhases"];
	if (j.contains("objectSpawnName")) objectSpawnName = j["objectSpawnName"];
}

float EnemySpawner::randomNumberGetter()
{
	int value = rand() % 2;
	if (value == 0) return positiveBounds.x + static_cast <float> (std::rand() / (static_cast <float> (RAND_MAX / (positiveBounds.y - positiveBounds.x))));
	else if (value == 1)
		return negativeBounds.y + static_cast <float> (std::rand() / (static_cast <float> (RAND_MAX / (negativeBounds.x - negativeBounds.y))));
}

Component* EnemySpawner::Clone(GameObject* newParent)
{
	EnemySpawner* toReturn = new EnemySpawner();
	toReturn->slope = slope;
	toReturn->c = c;
	toReturn->timeBetweenPhases = timeBetweenPhases;
	toReturn->objectSpawnName = objectSpawnName;
	toReturn->componentOwner = newParent;
	return toReturn;
}

//newObj->getComponent<Transform>()->SetPos(myPos.x + (rand() % (int)bounds.x) - (bounds.x / 2), myPos.y +
//	(rand() % (int)bounds.y) - (bounds.y / 2));

//slope* x + c
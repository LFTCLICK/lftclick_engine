#include "pch.h"
#include "EnemySpawner.h"
#include "FrameRateController.h"
#include "Components/Enemy.h"


EnemySpawner::EnemySpawner()
{
	gom = &GameObjectManager::getInstance();
	gof = &GameObjectFactory::getInstance();


	spawnTimer = 5.0f;
	spawnLocations.push_back(DirectX::SimpleMath::Vector2(1000, 1000));
	spawnLocations.push_back(DirectX::SimpleMath::Vector2(-1000, -1000));
	spawnLocations.push_back(DirectX::SimpleMath::Vector2(1000, -1000));
	spawnLocations.push_back(DirectX::SimpleMath::Vector2(-1000, 1000));
}

void EnemySpawner::Update()
{
	spawnTimer -= FrameRateController::getInstance().DeltaTime();
	if (spawnTimer < 0.0)
	{
		int number = enemy->RandomGenerator(0, 4);

		GameObject* enemy = gom->ClonePrefabOfTag(gof, "enemy");
		Transform* enemyTransform = enemy->getComponent<Transform>();

		enemyTransform->SetPos(spawnLocations[number].x, spawnLocations[number].y);
		spawnTimer = 1.5;
	}
}

void EnemySpawner::Start()
{

}


#pragma once

#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Player.h"
#include <json.hpp>


using json = nlohmann::json;
class Vortex : public Component
{
public:

	// Inherited via Component
	virtual void Start() override;
	virtual void Update() override;
	virtual int getCompId() override { return ComponentType::VORTEX; };
	static int getStaticCompId() { return ComponentType::VORTEX; };
	virtual Component* Clone(GameObject* newParent);
	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;

	void HandleMessage(Message* e);

	Vortex() {};


public:
	float pullingRadius;

private:
	Transform* ownerTrans;
	Transform* playerTrans;
	//SquareCollider* sqCollider;
	//Drawable* draw;
	bool playerInRange;
	bool playerExited;
	float internalTimer;
	int currentPhase, hpPerPhase;
};
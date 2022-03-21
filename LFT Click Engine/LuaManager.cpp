#include "pch.h"
#include "LuaManager.h"
#include "InputManager.h"
#include "GameObjectManager.h"
#include "GameManager.h"
#include "Components/Player.h"
#include "Components//Camera.h"
#include "Components/Gun.h"
#include "Components/Transform.h"
#include "FrameRateController.h"

// constructor
LuaManager::LuaManager() {

}

// registers input functions from the input manager
void LuaManager::RegGlobals(sol::state& state) 
{
	/*state.set_function("is_pressed", &InputManager::isKeyPressed, InputManager::getInstance());
	state.set_function("is_released", &InputManager::isKeyReleased, InputManager::getInstance());*/
}

void LuaManager::RegObjectFunctions(sol::state& state, GameObject* obj)
{
	if (obj->tag == "player")
	{
		Player* playerComponent = g_GameObjManager->FindObjectOfTag("player")->getComponent<Player>();
		Gun* gunComponent = g_GameObjManager->FindObjectOfTag("player")->getComponent<Gun>();
		Camera* cameraComponent = g_GameObjManager->FindObjectOfTag("player")->getComponent<Camera>();
		Transform* transformComponent = g_GameObjManager->FindObjectOfTag("player")->getComponent<Transform>();

		state.set_function("move", &Player::Move, playerComponent);
		state.set_function("dash", &Player::Dash, playerComponent);
		state.set_function("player_is_dead", &Player::ChangePlayerState, playerComponent);
		state.set_function("fire", &Gun::Fire, gunComponent);

		state.set_function("xPos_of_cam", &Camera::xPos, cameraComponent);
		state.set_function("yPos_of_cam", &Camera::yPos, cameraComponent);

		state.set_function("last_pos_x", &Transform::GetlastMovementX, transformComponent);
		state.set_function("last_pos_y", &Transform::GetlastMovementY, transformComponent);
	}
}

//void LuaManager::RegEvents(sol::state& state, TimedEvent* p_event) {
//	if (p_event == nullptr) {
//		state["received_event"] = false;
//	}
//	else {
//		state["received_event"] = true;
//		state["hit_event"] = false;
//		state["impact_event"] = false;
//		switch (p_event->event_id) {
//		case EventID::hit:
//			state["hit_event"] = true;
//			state["hit_direction"] = static_cast<HitEvent*>(p_event)->direction;
//			break;
//		case EventID::impact:
//			state["impact_event"] = true;
//			break;
//		}
//	}
//}

//// reads and loads scripts for all gameobjects with the behavior component
//void LuaManager::LoadBehaviorScripts() {
//	for (auto& obj : p_game_obj_manager->game_object_list)
//	{
//		Component* bhv_comp = obj->HasComponent("BEHAVIOR");
//
//		if (bhv_comp != nullptr)
//		{
//			Behavior* move = dynamic_cast<Behavior*>(bhv_comp);
//			move->LoadScript();
//		}
//	}
//}

// update
void LuaManager::Update() 
{
}

// destructor
LuaManager::~LuaManager() {

}

//void LuaManager::LogMessage(std::string log_str) {
//	SDL_Log(log_str.c_str());
//}
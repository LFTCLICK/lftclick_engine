// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	LuaManager.cpp
// Author			:	Rohit Punjabi
// Creation Date	:	03/13/2022
// Purpose			:	Takes care of binding LUA functions
// History			: 
// ---------------------------------------------------------------------------
#include "pch.h"
#include "LuaManager.h"

// constructor
LuaManager::LuaManager() {

}

// registers input functions from the input manager
void LuaManager::RegGlobals(sol::state& state) 
{
	state.set_function("deltaTime", &FrameRateController::DeltaTime, g_FrameRateController.get());

	state.set_function("is_pressed", &InputManager::isKeyPressed, g_InputManager.get());
	state.set_function("is_released", &InputManager::isKeyReleased, g_InputManager.get());
	state.set_function("is_triggered", &InputManager::isKeyTriggered, g_InputManager.get());
	state.set_function("is_mouse_button_triggered", &InputManager::isMouseButtonTriggered, g_InputManager.get());
	state.set_function("is_mouse_button_pressed", &InputManager::isMouseButtonPressed, g_InputManager.get());
	state.set_function("mouse_pos_x", &InputManager::mouseX, g_InputManager.get());
	state.set_function("mouse_pos_y", &InputManager::mouseY, g_InputManager.get());

	state.set_function("screen_width", &Renderer::GetWidth, g_Renderer.get());
	state.set_function("screen_height", &Renderer::GetHeight, g_Renderer.get());

	state.set_function("current_level", &GameManager::currentLevel, g_GameManager.get());
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
		state.set_function("player_is_dead", &Player::DamagePlayer, playerComponent);

		state.set_function("fire", &Gun::Fire, gunComponent);
		state.set_function("fire_rate_timer", &Gun::ReadyToFire, gunComponent);

		state.set_function("xPos_of_cam", &Camera::xPos, cameraComponent);
		state.set_function("yPos_of_cam", &Camera::yPos, cameraComponent);

		state.set_function("last_pos_x", &Transform::GetlastMovementX, transformComponent);
		state.set_function("last_pos_y", &Transform::GetlastMovementY, transformComponent);
	}
}

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
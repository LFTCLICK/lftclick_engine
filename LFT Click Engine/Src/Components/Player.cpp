#include "pch.h"
#include "Player.h"
#include "GameManager.h"
#include "FrameRateController.h"
#include "../LuaManager.h"

void Player::Start()
{
	//LuaManager::getInstance().RegGlobals(lua_player_state);
	InputManager::getInstance().RegGlobals(lua_player_state);
	FrameRateController::getInstance().RegGlobals(lua_player_state);
	LuaManager::getInstance().RegObjectFunctions(lua_player_state, parent);
	lua_player_state.open_libraries(sol::lib::base, sol::lib::package);

	player_script_update = lua_player_state.load_file(script);

	// setting variables from LUA
	player_script_update();
	sol::function returnHp = lua_player_state["returnPlayerHp"];
	hp = returnHp();
	sol::function dashSpeedMultiplier = lua_player_state["dashSpeedMultiplyer"];
	dashSpeed = dashSpeedMultiplier();

	trans = parent->getComponent<Transform>();
	cam = parent->getComponent<Camera>();
	timer = damageCooldownTimer;
}

void Player::Update()
{
	player_script_update();

	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::Begin("2ndWindow", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | 
		ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | 
		ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize | 
		ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground);
	ImGui::Text("Wood: %i", wood);
	ImGui::Text("hp: %i", hp);
	ImGui::End();

	if (im.isKeyPressed(SDL_SCANCODE_W)) Move(0, playerSpeed * deltaTime);
	if (im.isKeyPressed(SDL_SCANCODE_S)) Move(0, -playerSpeed * deltaTime);
	if (!autopilot && im.isKeyPressed(SDL_SCANCODE_D)) Move(playerSpeed * deltaTime, 0);
	if (!autopilot && im.isKeyPressed(SDL_SCANCODE_A)) Move(-playerSpeed * deltaTime, 0);

	if (im.isKeyPressed(SDL_SCANCODE_UP)) cam->Move(0.0f, playerSpeed * deltaTime);
	if (im.isKeyPressed(SDL_SCANCODE_DOWN)) cam->Move(0.0f, -playerSpeed * deltaTime);
	if (im.isKeyPressed(SDL_SCANCODE_RIGHT)) cam->Move(playerSpeed * deltaTime, 0.0f);
	damageCooldownTimer -= FrameRateController::getInstance().DeltaTime();
}
		if (dashTimer > dashTime) {
			isDashing = false;
			dashTimer = 0;
		}
		trans->Move(dashVelocity.x, dashVelocity.y);
	}

	if (autopilot) Sidescroll(deltaTime);

	if (badTouch && timer <= 0)
void Player::Deserialize(nlohmann::json j, GameObject* parent) {
	if (j.contains("script")) script = j["script"];
	badTouch = false;
	if (hp <= 0)
	{
		GameManager::getInstance().playerDead = true;
	}
}

Component* Player::Clone(GameObject* newParent) {
	Player* toReturn = new Player();
	toReturn->script = script;
	toReturn->parent = newParent;

	toReturn->script = script;
	return (Component*)toReturn;
}

void Player::Deserialize(nlohmann::json j, GameObject* parent)
{
	if (j.contains("playerSpeed")) playerSpeed = j["playerSpeed"];
	if (j.contains("dashSpeedMultiplier")) dashSpeedMultiplier = j["dashSpeedMultiplier"];
	if (j.contains("dashTime")) dashTime = j["dashTime"];
	if (j.contains("autopilot")) autopilot = j["autopilot"];
	if (j.contains("maxHp")) maxHp = j["maxHp"];
	if (j.contains("damageCooldownTimer")) damageCooldownTimer = j["damageCooldownTimer"];
	this->parent = parent;
}

void Player::HandleMessage(Message* e)
{
	if (e->id == Message::COLLISION)
	{
		CollisionMessage* cm = (CollisionMessage*)e;

		Move(cm->deltaPos.x, cm->deltaPos.y);

		if (e->sourceObjectTag == "enemy")
		{
			if (damageCooldownTimer < 0)
			{
				sol::function PlayerCollidedWithEnemy = lua_player_state["PlayerCollidedWithEnemy"];
				PlayerCollidedWithEnemy();
				damageCooldownTimer = 2;
			}
		}





//if (autopilot) Sidescroll(deltaTime);

//void Player::Sidescroll(float deltaTime) 
//{
//	auto pos = trans->CurrentPos();
//	Move((playerSpeed + (cam->xPos - pos.x > AUTOPILOT_START_DISTANCE ? 60 : 0)) * deltaTime, 0);
//}

/*if (badTouch && timer <= 0)
	{
		hp -= 15;
		timer = damageCooldownTimer;
	}
	else if (timer > 0)
	{
		timer -= FrameRateController::getInstance().DeltaTime();
	}*/
	//badTouch = false;
	/*if (hp <= 0)
	{
		GameManager::getInstance().playerDead = true;
	}*/



//cam = parent->getComponent<Camera>();


// LUA input checking code
//sol::protected_function_result script2result = player_script_update();
	//// optionally, check if it worked
	//if (script2result.valid()) {
	//	printf("if condition");
	//}
	//else {
	//	printf("else condition");
	//}


//if (j.contains("playerSpeed")) playerSpeed = j["playerSpeed"];
//if (j.contains("dashSpeedMultiplier")) dashSpeedMultiplier = j["dashSpeedMultiplier"];
//if (j.contains("dashTime")) dashTime = j["dashTime"];
//if (j.contains("autopilot")) autopilot = j["autopilot"];
//if (j.contains("maxHp")) maxHp = j["maxHp"];
//if (j.contains("damageCooldownTimer")) damageCooldownTimer = j["damageCooldownTimer"];

//toReturn->dashSpeedMultiplier = dashSpeedMultiplier;
//toReturn->dashTime = dashTime;
//toReturn->dashTimer = dashTimer;
//toReturn->dashVelocity = dashVelocity;
//toReturn->deadZone = deadZone;
//toReturn->playerSpeed = playerSpeed;
//toReturn->isDashing = isDashing;
//toReturn->autopilot = autopilot;
//toReturn->maxHp = maxHp;
//toReturn->damageCooldownTimer = damageCooldownTimer;

//if (autopilot) cam->SetAutopilotVelocity("right", playerSpeed);
		//std::cout << transform->GetPosXMVector().m128_f32[0] - lastGroundPos << std::endl;
		//lastGroundPos = transform->GetPosXMVector().m128_f32[0];
	}
}

void Player::Move(float deltaX, float deltaY) {
	if (!isDashing) trans->Move(deltaX, deltaY);
}

void Player::Dash() {
	isDashing = true;
	dashVelocity.x = trans->lastMovement.x * dashSpeed;
	dashVelocity.y = trans->lastMovement.y * dashSpeed;
}

void Player::ChangePlayerState() {
	hp -= 15;
	if (hp <= 0){GameManager::getInstance().playerDead = true;}
}

void Player::Sidescroll(float deltaTime) {
	auto pos = trans->CurrentPos();
	Move((playerSpeed + (cam->xPos - pos.x > AUTOPILOT_START_DISTANCE ? 60 : 0)) * deltaTime, 0);
}
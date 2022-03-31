#include "pch.h"
#include "Player.h"
#include "GameManager.h"
#include "FrameRateController.h"
#include "Collider.h"
#include "SquareCollider.h"

using namespace DirectX::SimpleMath;

void Player::Start()
{
	g_LuaManager->RegGlobals(lua_player_state);
	g_LuaManager->RegObjectFunctions(lua_player_state, componentOwner);
	lua_player_state.open_libraries(sol::lib::base, sol::lib::package);

	player_script_update = lua_player_state.load_file(script);

	// setting variables from LUA
	player_script_update();
	sol::function returnHp = lua_player_state["returnPlayerHp"];
	hp = returnHp();
	sol::function dashSpeedMultiplier = lua_player_state["dashSpeedMultiplyer"];
	dashSpeed = dashSpeedMultiplier();

	trans = componentOwner->getComponent<Transform>();
	g_EventManager->Subscribe(Message::COLLISION, componentOwner);

	drawable = componentOwner->getComponent<Drawable>();
	squareCollider = componentOwner->getComponent<SquareCollider>();

	/*if (autopilot) cam->SetAutopilotVelocity("right", playerSpeed);
	wood = 0;
	parts = 0;
	hp = maxHp;
	timer = damageCooldownTimer;*/
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
	ImGui::Text("Motorcycle Parts: %i/8", parts);
	ImGui::Text("");
	ImGui::Text("Day: %i", g_GameManager->day);
	ImGui::Text("Time: %.2f", g_GameManager->time);
	ImGui::End();

	drawable->HUD_DrawTextCenter("Player", Vector2(0, -squareCollider->height / 2.0f - 15.0f), Color(0.0f, 0.0f, 1.0f));

	if (isDashing) {
		dashTimer += g_FrameRateController->DeltaTime();
		if (dashTimer > dashTime) {
			isDashing = false;
			dashTimer = 0;
		}
		trans->Move(dashVelocity.x, dashVelocity.y);
	}

	damageCooldownTimer -= g_FrameRateController->DeltaTime();
}

Component* Player::Clone(GameObject* newParent) {
	Player* toReturn = new Player();
	toReturn->script = script;
	toReturn->componentOwner = newParent;
	return (Component*)toReturn;
}

void Player::Deserialize(nlohmann::json j, GameObject* parent) {
	if (j.contains("script")) script = j["script"];
	this->componentOwner = componentOwner;
}

void Player::HandleMessage(Message* e)
{
	if (e->id == Message::COLLISION)
	{
		CollisionMessage* cm = (CollisionMessage*)e;

		Move(cm->deltaPos.x, cm->deltaPos.y);

		if (e->otherObject->componentOwner->tag == "zombie")
		{
			if (damageCooldownTimer < 0)
			{
				sol::function PlayerCollidedWithEnemy = lua_player_state["PlayerCollidedWithEnemy"];
				PlayerCollidedWithEnemy();
				damageCooldownTimer = 2;
			}
		}
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
	if (hp <= 0) { g_GameManager->playerDead = true; }
}
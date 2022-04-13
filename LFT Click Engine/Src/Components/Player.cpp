#include "pch.h"
#include "Player.h"
#include "GameManager.h"
#include "FrameRateController.h"
#include "Collider.h"
#include "SquareCollider.h"
#include <WICTextureLoader.h>

using namespace DirectX::SimpleMath;
using namespace DirectX;

constexpr auto DAMAGE_FLASH_DURATION = 0.3f; //seconds


void Player::Start()
{
	cam = componentOwner->getComponent<Camera>();

	g_LuaManager->RegGlobals(lua_player_state);
	g_LuaManager->RegObjectFunctions(lua_player_state, componentOwner);
	lua_player_state.open_libraries(sol::lib::base, sol::lib::package);

	player_script_update = lua_player_state.load_file(script);

	// setting variables from LUA
	player_script_update();
	sol::function returnHp = lua_player_state["returnPlayerHp"];
	health = returnHp();
	sol::function dashSpeedMultiplier = lua_player_state["dashSpeedMultiplyer"];
	dashSpeed = dashSpeedMultiplier();
	sol::function isAutopilot = lua_player_state["isAutopilot"];
	autopilot = isAutopilot();
	sol::function getPlayerSpeed = lua_player_state["getPlayerSpeed"];
	playerSpeed = getPlayerSpeed();

	myTransform = componentOwner->getComponent<Transform>();
	g_EventManager->Subscribe(Message::COLLISION, componentOwner);

	drawable = componentOwner->getComponent<Drawable>();
	squareCollider = componentOwner->getComponent<SquareCollider>();

	DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(g_Renderer->GetDevice(), L"Resources\\images\\wood_icon.png", nullptr, &woodSRV));
	DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(g_Renderer->GetDevice(), L"Resources\\images\\health_icon.png", nullptr, &healthSRV));
	DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(g_Renderer->GetDevice(), L"Resources\\images\\motorcycle_icon.png", nullptr, &bikepartsSRV));

	if (autopilot) cam->SetAutopilotVelocity("right", playerSpeed);
	zHelper = g_GameManager->mapHeight / 2.0f;
	introTimer = 0;
}

void Player::Update()
{
	player_script_update();

	XMFLOAT2 origin = XMFLOAT2(0, 0);
	SpriteBatch* spriteBatch = g_Renderer->GetSpriteBatch();
	SpriteFont* spriteFont = g_Renderer->GetSpriteFont();
	Color red = Color(1, 0, 0, 1);
	Color green = Color(0, 1, 0, 1);


#ifdef _DEBUG
	ImGui::DragFloat2("Wood HUD Pos", reinterpret_cast<float*>(&woodHUDPos), 1.0f, -50.0f, 200.0f);
	ImGui::DragFloat2("Wood HUD Text Pos", reinterpret_cast<float*>(&woodTextPos), 1.0f, -50.0f, 200.0f);
	ImGui::DragFloat2("Wood HUD Scale", reinterpret_cast<float*>(&woodHUDScale), 1.0f, -50.0f, 200.0f);

	ImGui::DragFloat2("Bike HUD Pos", reinterpret_cast<float*>(&bikeHUDPos), 1.0f, -50.0f, 200.0f);
	ImGui::DragFloat2("Bike HUD Text Pos", reinterpret_cast<float*>(&bikeTextPos), 1.0f, -50.0f, 200.0f);
	ImGui::DragFloat2("Bike HUD Scale", reinterpret_cast<float*>(&bikeHUDScale), 1.0f, -50.0f, 200.0f);

	ImGui::DragFloat2("Health HUD Pos", reinterpret_cast<float*>(&healthHUDPos), 1.0f, -50.0f, 200.0f);
	ImGui::DragFloat2("Health HUD Text Pos", reinterpret_cast<float*>(&healthTextPos), 1.0f, -50.0f, 200.0f);
	ImGui::DragFloat2("Health HUD Scale", reinterpret_cast<float*>(&healthHUDScale), 1.0f, -50.0f, 200.0f);
#endif

	//Wood count HUD
	spriteBatch->Draw(woodSRV.Get(), woodHUDPos, nullptr, Colors::White, 0.0f, origin, woodHUDScale);
	spriteFont->DrawString(spriteBatch, std::to_string(wood).c_str(), woodTextPos);

	//Bike parts count HUD
	spriteBatch->Draw(bikepartsSRV.Get(), bikeHUDPos, nullptr, Colors::White, 0.0f, origin, bikeHUDScale);
	spriteFont->DrawString(spriteBatch, (std::to_string(parts) + "/8").c_str(), bikeTextPos);

	//Player health HUD
	Color healthTextColor = Color::Lerp(red, green, static_cast<float>(health) / 100);
	float healthTextScale = std::lerp(1.0f, 3.0f, g_GameManager->rednessFactor);
	Vector2 healthSpriteScale = Vector2::Lerp(healthHUDScale, healthHUDScale * 2.0f, g_GameManager->rednessFactor);

	spriteBatch->Draw(healthSRV.Get(), healthHUDPos, nullptr, Colors::White, 0.0f, origin, healthSpriteScale);
	spriteFont->DrawString(spriteBatch, std::to_string(health).c_str(), healthTextPos, healthTextColor, 0.0f, origin, healthTextScale);

	static float flashOutTimer = 0.0f;
	static float flashInTimer = 0.0f;
	if (damageFlashing)
	{
		flashOutTimer = 0.0f;
		flashInTimer += g_FrameRateController->DeltaTime();

		if (flashInTimer > DAMAGE_FLASH_DURATION)
		{
			damageFlashing = false;
		}
		
		g_GameManager->rednessFactor = std::lerp(0.0f, 1.0f, flashInTimer / DAMAGE_FLASH_DURATION);
	}
	else
	{
		flashInTimer = 0.0f;
		flashOutTimer += g_FrameRateController->DeltaTime();
		
		g_GameManager->rednessFactor = std::lerp(g_GameManager->rednessFactor, 0.0f, std::clamp(flashOutTimer / DAMAGE_FLASH_DURATION, 0.0f, 1.0f));
	}

	if (isDashing) 
	{
		dashTimer += g_FrameRateController->DeltaTime();

		if (dashTimer > dashTime) 
		{
			isDashing = false;
			dashTimer = 0;
		}
		myTransform->Move(dashVelocity.x, dashVelocity.y);
	}

	/*if (parts == 0 && introTimer < 10.0f)
	{
		drawable->HUD_DrawTextCenter("I need to find 8 motorcycle parts to fix my bike.", { 0.0f, -70.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	}*/

	if (parts >= 8)
	{
		g_GameManager->playerDead = true;
		g_GameManager->playerWon = true;
	}

	if (autopilot)
		Sidescroll(g_FrameRateController->DeltaTime());

	if (currentMessage.timeout > 0) {
		currentMessage.timeout -= g_FrameRateController->DeltaTime();
		drawable->HUD_DrawTextCenter(currentMessage.message, { 0.0f, -50.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	}
	else if (!g_GameManager->messageQueue.empty()) {
		currentMessage = g_GameManager->GetPlayerMessage();
	}


	damageCooldownTimer -= g_FrameRateController->DeltaTime();

	g_AudioManager->SetPlayerSpatialPosition(myTransform->CurrentPos() / 100);
	myTransform->zPos = 5 + ((myTransform->position.y + g_GameManager->mapHeight) / zHelper);
	introTimer += g_FrameRateController->DeltaTime();
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

		if (e->otherObject->componentOwner->tag == "zombie" || e->otherObject->componentOwner->tag == "zombie")
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
	if (!isDashing) myTransform->Move(deltaX, deltaY);
}

void Player::Dash() {
	isDashing = true;
	dashVelocity = myTransform->lastMovement * dashSpeed;
}

void Player::DamagePlayer() {
	damageFlashing = true;
	health -= 15;
	if (health <= 0) { g_GameManager->playerDead = true; }
}

void Player::Sidescroll(float deltaTime) {
	Move((playerSpeed + (cam->xPos - myTransform->CurrentPos().x > AUTOPILOT_START_DISTANCE ? 60 : 0)) * deltaTime, 0);
}
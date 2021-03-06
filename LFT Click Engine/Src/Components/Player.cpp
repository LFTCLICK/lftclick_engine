// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Player.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/2/13
// Purpose			:	Main player
// History			:
// ---------------------------------------------------------------------------
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

const Audible::SoundEvent AUDIO_ON_DAMAGE = Audible::SoundEvent::AUDIO_ON_DAMAGE;
const Audible::SoundEvent AUDIO_ON_DEATH = Audible::SoundEvent::AUDIO_ON_DEATH;

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
	playerHealth = returnHp();
	sol::function dashSpeedMultiplier = lua_player_state["dashSpeedMultiplyer"];
	dashSpeed = dashSpeedMultiplier();
	sol::function isAutopilot = lua_player_state["isAutopilot"];
	autopilot = isAutopilot();
	sol::function getPlayerSpeed = lua_player_state["getPlayerSpeed"];
	playerSpeed = getPlayerSpeed();
	sol::function getPlayerSpeedForSideScroller = lua_player_state["getPlayerSpeedForSideScroller"];
	playerSpeedForSideScroller = getPlayerSpeedForSideScroller();
	sol::function getCamSpeed = lua_player_state["getCamerSpeed"];
	camSpeed = getCamSpeed();
	PlayerCollidedWithEnemy = lua_player_state["PlayerCollidedWithEnemy"];

	playerInPuddle = false;

	myTransform = componentOwner->getComponent<Transform>();
	g_EventManager->Subscribe(Message::COLLISION, componentOwner);

	drawable = componentOwner->getComponent<Drawable>();
	audio = componentOwner->getComponent<Audible>();
	anim = componentOwner->getComponent<SpriteAnimator>();
	squareCollider = componentOwner->getComponent<SquareCollider>();

	DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(g_Renderer->GetDevice(), L"Resources\\images\\wood_icon.png", nullptr, &woodSRV));
	DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(g_Renderer->GetDevice(), L"Resources\\images\\health_icon.png", nullptr, &healthSRV));
	DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(g_Renderer->GetDevice(), L"Resources\\images\\motorcycle_icon.png", nullptr, &bikepartsSRV));

	if (autopilot) cam->SetAutopilotVelocity("right", camSpeed);
	zHelper = g_GameManager->mapHeight / 2.0f;
	introTimer = 0;

	positiveBound = { POSITIVE_BOUND_X, POSITIVE_BOUND_Y };
	negativeBound = { NEGATIVE_BOUND_X, NEGATIVE_BOUND_Y };

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
	ImGui::DragFloat("Wood HUD Scale", &woodHUDScale, 1.0f, -50.0f, 200.0f);

	ImGui::DragFloat2("Bike HUD Pos", reinterpret_cast<float*>(&bikeHUDPos), 1.0f, -50.0f, 200.0f);
	ImGui::DragFloat2("Bike HUD Text Pos", reinterpret_cast<float*>(&bikeTextPos), 1.0f, -50.0f, 200.0f);
	ImGui::DragFloat("Bike HUD Scale", &bikeHUDScale, 1.0f, -50.0f, 200.0f);

	ImGui::DragFloat2("Health HUD Pos", reinterpret_cast<float*>(&healthHUDPos), 1.0f, -50.0f, 200.0f);
	ImGui::DragFloat2("Health HUD Text Pos", reinterpret_cast<float*>(&healthTextPos), 1.0f, -50.0f, 200.0f);
	ImGui::DragFloat("Health HUD Scale", &healthHUDScale, 1.0f, -50.0f, 200.0f);
#endif

	constexpr auto HUD_ANIM_LENGTH = 0.3f; //seconds


	//Wood count HUD
	static float woodInAnimTimer = 0.0f;
	static float woodOutAnimTimer = 0.0f;
	if (playCollectedAnimWood)
	{
		woodOutAnimTimer = 0.0f;
		woodInAnimTimer += g_FrameRateController->DeltaTime();

		if (woodInAnimTimer > HUD_ANIM_LENGTH)
			playCollectedAnimWood = false;

		currentWoodHUDScale = std::lerp(woodHUDScale, woodHUDScale * 2.0f, woodInAnimTimer / HUD_ANIM_LENGTH);
		currentWoodHUDTextScale = std::lerp(woodHUDTextScale, woodHUDTextScale * 3.0f, woodInAnimTimer / HUD_ANIM_LENGTH);
	}
	else
	{
		woodInAnimTimer = 0.0f;
		woodOutAnimTimer += g_FrameRateController->DeltaTime();

		currentWoodHUDScale = std::lerp(currentWoodHUDScale, woodHUDScale, std::clamp(woodOutAnimTimer / HUD_ANIM_LENGTH, 0.0f, 1.0f));
		currentWoodHUDTextScale = std::lerp(currentWoodHUDTextScale, woodHUDTextScale, std::clamp(woodOutAnimTimer / HUD_ANIM_LENGTH, 0.0f, 1.0f));
	}

	spriteBatch->Draw(woodSRV.Get(), woodHUDPos, nullptr, Colors::White, 0.0f, origin, currentWoodHUDScale);
	spriteFont->DrawString(spriteBatch, std::to_string(collectibleWood).c_str(), woodTextPos, Colors::White, 0.0f, origin, currentWoodHUDTextScale);

	//Bike parts count HUD
	static float bikeInAnimTimer = 0.0f;
	static float bikeOutAnimTimer = 0.0f;
	if (playCollectedAnimParts)
	{
		bikeOutAnimTimer = 0.0f;
		bikeInAnimTimer += g_FrameRateController->DeltaTime();

		if (bikeInAnimTimer > HUD_ANIM_LENGTH)
			playCollectedAnimParts = false;

		currentBikeHUDScale = std::lerp(bikeHUDScale, bikeHUDScale * 2.0f, bikeInAnimTimer / HUD_ANIM_LENGTH);
		currentBikeHUDTextScale = std::lerp(bikeHUDTextScale, bikeHUDTextScale * 3.0f, bikeInAnimTimer / HUD_ANIM_LENGTH);
	}
	else
	{
		bikeInAnimTimer = 0.0f;
		bikeOutAnimTimer += g_FrameRateController->DeltaTime();

		currentBikeHUDScale = std::lerp(currentBikeHUDScale, bikeHUDScale, std::clamp(bikeOutAnimTimer / HUD_ANIM_LENGTH, 0.0f, 1.0f));
		currentBikeHUDTextScale = std::lerp(currentBikeHUDTextScale, bikeHUDTextScale, std::clamp(bikeOutAnimTimer / HUD_ANIM_LENGTH, 0.0f, 1.0f));
	}
	spriteBatch->Draw(bikepartsSRV.Get(), bikeHUDPos, nullptr, Colors::White, 0.0f, origin, currentBikeHUDScale);
	spriteFont->DrawString(spriteBatch, (std::to_string(collectibleparts) + "/8").c_str(), bikeTextPos, Colors::White, 0.0f, origin, currentBikeHUDTextScale);

	//Player health HUD
	Color healthTextColor = Color::Lerp(red, green, static_cast<float>(playerHealth) / 100);
	float healthTextScale = std::lerp(1.0f, 3.0f, g_GameManager->rednessFactor);
	float healthSpriteScale = std::lerp(healthHUDScale, healthHUDScale * 2.0f, g_GameManager->rednessFactor);

	spriteBatch->Draw(healthSRV.Get(), healthHUDPos, nullptr, Colors::White, 0.0f, origin, healthSpriteScale);
	spriteFont->DrawString(spriteBatch, std::to_string(playerHealth).c_str(), healthTextPos, healthTextColor, 0.0f, origin, healthTextScale);


	//Flashing redscreen when player gets damaged
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

	/*if (isDashing)
	{
		dashTimer += g_FrameRateController->DeltaTime();

		if (dashTimer > dashTime) 
		{
			isDashing = false;
			dashTimer = 0;
		}
		myTransform->Move(dashVelocity.x, dashVelocity.y);
	}*/

	/*if (parts == 0 && introTimer < 10.0f)
	{
		drawable->HUD_DrawTextCenter("I need to find 8 motorcycle parts to fix my bike.", { 0.0f, -70.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	}*/

	if (collectibleparts >= 8)
	{
		g_GameManager->playerDead = true;
		g_GameManager->playerWon = true;
	}

	if (autopilot)
	{
		if (playerInPuddle)
		{
			Move((100 + (cam->xPos - myTransform->CurrentPos().x > AUTOPILOT_START_DISTANCE ? 60 : 0)) * g_FrameRateController->DeltaTime(), 0);
			playerInPuddle = false;
		}
		else
			Move((playerSpeedForSideScroller + (cam->xPos - myTransform->CurrentPos().x > AUTOPILOT_START_DISTANCE ? 60 : 0)) * g_FrameRateController->DeltaTime(), 0);
	}

	if (myTransform->CurrentPos().x < cam->xPos - 800)
		g_GameManager->playerDead = true;

	if (!autopilot) {
		if (currentMessage.timeout > 0)
		{
			currentMessage.timeout -= g_FrameRateController->DeltaTime();
			drawable->HUD_DrawTextCenter(currentMessage.message, { 0.0f, -50.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
		}
		else if (!g_GameManager->messageQueue.empty())
		{
			currentMessage = g_GameManager->GetPlayerMessage();
		}
	}

	if (hitSpeed > 0) {
		DirectX::SimpleMath::Vector2 movement = hitDirection * hitSpeed * g_FrameRateController->DeltaTime();
		Move(movement.x, movement.y);
		hitSpeed -= inertiaMod;
	}

	damageCooldownTimer -= g_FrameRateController->DeltaTime();

	g_AudioManager->SetPlayerSpatialPosition(myTransform->CurrentPos());
	audio->SetPosition(myTransform->CurrentPos());
	myTransform->zPos = 5 + ((myTransform->position.y + g_GameManager->mapHeight) / zHelper);
	introTimer += g_FrameRateController->DeltaTime();

	// Keep player within bounds of map.
	if (!autopilot) {
		auto pos = myTransform->CurrentPos();
		if (pos.x > positiveBound.x)
			myTransform->Move(positiveBound.x - pos.x, 0);
		else if (pos.x < negativeBound.x)
			myTransform->Move(negativeBound.x - pos.x, 0);
		if (pos.y > positiveBound.y)
			myTransform->Move(0, positiveBound.y - pos.y);
		else if (pos.y < negativeBound.y)
			myTransform->Move(0, negativeBound.y - pos.y);
	}

	// The rest of the cheats are in GameManager, in UpdateCheats.
	if (g_InputManager->isKeyTriggered(SDL_SCANCODE_F1)) {
		collectibleparts++;
	}
	if (g_InputManager->isKeyTriggered(SDL_SCANCODE_F2)) {
		collectibleWood++;
	}
	if (g_InputManager->isKeyTriggered(SDL_SCANCODE_F7)) {
		collectibleparts = 8;
	}
	if (g_InputManager->isKeyTriggered(SDL_SCANCODE_F8)) {
		playerHealth = 0;
		g_GameManager->playerDead = true;
	}
}

Component* Player::Clone(GameObject* newParent) 
{
	Player* toReturn = new Player();
	toReturn->script = script;
	toReturn->inertiaMod = inertiaMod;
	toReturn->componentOwner = newParent;
	return (Component*)toReturn;
}

void Player::Deserialize(nlohmann::json j, GameObject* parent) {
	if (j.contains("script")) 
		script = j["script"];
	if (j.contains("inertiaMod")) 
		inertiaMod = j["inertiaMod"];

	this->componentOwner = componentOwner;
}

void Player::HandleMessage(Message* e)
{
	if (e->id == Message::COLLISION)
	{
		CollisionMessage* cm = (CollisionMessage*)e;

		if (g_GameManager->currentLevel == EGameLevel::SideScrollerLevel)
		{
			if (e->otherObject->componentOwner->tag == "couch")
				g_GameManager->playerDead = true;
		}

		Enemy* enemyComp = e->otherObject->componentOwner->getComponent<Enemy>();
		if (enemyComp != nullptr && damageCooldownTimer < 0 && !g_GameManager->godMode)
		{
			hitDirection = cm->deltaPos;
			hitDirection.Normalize();
			hitSpeed = enemyComp->speed * 6;

			PlayerCollidedWithEnemy();
			damageCooldownTimer = 2;

			DirectX::SimpleMath::Vector2 movement = hitDirection * hitSpeed * g_FrameRateController->DeltaTime();
			Move(movement.x, movement.y);
		}
		else {
			Move(cm->deltaPos.x, cm->deltaPos.y);
		}
	}

	if (e->id == Message::TRIGGER_COLLISION)
	{
		if (e->otherObject->componentOwner->tag == "water_puddle")
			playerInPuddle = true;
	}
}

void Player::Move(float deltaX, float deltaY) 
{
	if (!isDashing) 
		myTransform->Move(deltaX, deltaY);
}

void Player::Dash() 
{
	isDashing = true;
	dashVelocity = myTransform->lastMovement * dashSpeed;
}

void Player::DamagePlayer() 
{
	damageFlashing = true;

	playerHealth -= 15;
	anim->Damage(0.5f);
	audio->PlaySoundsOnEvent(AUDIO_ON_DAMAGE);

	if (playerHealth <= 0) 
		g_GameManager->playerDead = true; 
}

void Player::Sidescroll(float deltaTime) 
{
	Move((playerSpeed + (cam->xPos - myTransform->CurrentPos().x > AUTOPILOT_START_DISTANCE ? 60 : 0)) * deltaTime, 0);
}
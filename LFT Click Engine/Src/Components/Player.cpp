#include "pch.h"
#include "Player.h"
#include "GameManager.h"
#include "FrameRateController.h"
#include "Collider.h"
#include "SquareCollider.h"
#include <WICTextureLoader.h>

using namespace DirectX::SimpleMath;
using namespace DirectX;

void Player::Start()
{
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

	trans = componentOwner->getComponent<Transform>();
	g_EventManager->Subscribe(Message::COLLISION, componentOwner);

	drawable = componentOwner->getComponent<Drawable>();
	squareCollider = componentOwner->getComponent<SquareCollider>();

	DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(g_Renderer->GetDevice(), L"Resources\\images\\wood_icon.png", nullptr, &woodSRV));
	DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(g_Renderer->GetDevice(), L"Resources\\images\\health_icon.png", nullptr, &healthSRV));
	DX::ThrowIfFailed(DirectX::CreateWICTextureFromFile(g_Renderer->GetDevice(), L"Resources\\images\\motorcycle_icon.png", nullptr, &bikepartsSRV));
}

void Player::Update()
{
	player_script_update();

	//Wood count HUD
	g_Renderer->GetSpriteBatch()->Draw(woodSRV.Get(), XMVectorSet(4, 20, 0, 0), nullptr, 
		Colors::White, 0.0f, XMVectorZero(), 2.6f);
	g_Renderer->GetSpriteFont()->DrawString(g_Renderer->GetSpriteBatch(), std::to_string(wood).c_str(),
		XMFLOAT2(50, 25), Colors::White, 0.0f, XMFLOAT2(0,0));

	//Bike parts count HUD
	g_Renderer->GetSpriteBatch()->Draw(bikepartsSRV.Get(), XMVectorSet(4, 50, 0, 0), nullptr,
		Colors::White, 0.0f, XMVectorZero(), 2.6f);
	g_Renderer->GetSpriteFont()->DrawString(g_Renderer->GetSpriteBatch(), std::to_string(parts).c_str(),
		XMFLOAT2(50, 60), Colors::White, 0.0f, XMFLOAT2(0, 0));

	//Player health HUD
	Color healthTextColor = Color::Lerp(Color(1, 0, 0, 1), Color(0, 1, 0, 1),
		static_cast<float>(health) / 100);

	g_Renderer->GetSpriteBatch()->Draw(healthSRV.Get(), XMVectorSet(4, 90, 0, 0), nullptr,
		Colors::White, 0.0f, XMVectorZero(), 2.6f);
	g_Renderer->GetSpriteFont()->DrawString(g_Renderer->GetSpriteBatch(), std::to_string(health).c_str(),
		XMFLOAT2(50, 100), healthTextColor, 0.0f, XMFLOAT2(0, 0));


	if (isDashing) {
		dashTimer += g_FrameRateController->DeltaTime();
		if (dashTimer > dashTime) {
			isDashing = false;
			dashTimer = 0;
		}
		trans->Move(dashVelocity.x, dashVelocity.y);
	}

	damageCooldownTimer -= g_FrameRateController->DeltaTime();

	if (trans->isMoving) {
		g_AudioManager->SetPlayerSpatialPosition(trans->CurrentPos() / 100);
	}
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
	/*dashVelocity.x = trans->lastMovement.x * dashSpeed;
	dashVelocity.y = trans->lastMovement.y * dashSpeed;*/

	dashVelocity = trans->lastMovement * dashSpeed;
}

void Player::ChangePlayerState() {
	health -= 15;
	if (health <= 0) { g_GameManager->playerDead = true; }
}
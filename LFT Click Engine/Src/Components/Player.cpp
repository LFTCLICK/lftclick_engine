#include "pch.h"
#include "Player.h"
#include "GameManager.h"
#include "FrameRateController.h"

using namespace DirectX::SimpleMath;

void Player::Start()
{
	trans = parent->getComponent<Transform>();
	gun = parent->getComponent<Gun>();
	cam = parent->getComponent<Camera>();
	EventManager::getInstance().Subscribe(Message::COLLISION, parent);

	if (autopilot) cam->SetAutopilotVelocity("right", playerSpeed);
	wood = 0;
	hp = maxHp;
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


	InputManager& im = InputManager::getInstance();
	float deltaTime = FrameRateController::getInstance().DeltaTime();

	if (im.isKeyPressed(SDL_SCANCODE_W)) Move(0, playerSpeed * deltaTime);
	if (im.isKeyPressed(SDL_SCANCODE_S)) Move(0, -playerSpeed * deltaTime);
	if (!autopilot && im.isKeyPressed(SDL_SCANCODE_D)) Move(playerSpeed * deltaTime, 0);
	if (!autopilot && im.isKeyPressed(SDL_SCANCODE_A)) Move(-playerSpeed * deltaTime, 0);

	if (im.isKeyPressed(SDL_SCANCODE_UP)) cam->Move(0.0f, playerSpeed * deltaTime);
	if (im.isKeyPressed(SDL_SCANCODE_DOWN)) cam->Move(0.0f, -playerSpeed * deltaTime);
	if (im.isKeyPressed(SDL_SCANCODE_RIGHT)) cam->Move(playerSpeed * deltaTime, 0.0f);
	if (im.isKeyPressed(SDL_SCANCODE_LEFT)) cam->Move(-playerSpeed * deltaTime, 0.0f);

	if (im.isKeyTriggered(SDL_SCANCODE_SPACE)) Dash();

	if (im.isMouseButtonTriggered(0)) {
		float targetX = (float)(im.mouseX() - 400) + GameManager::getInstance().mainCamera->xPos;
		float targetY = -1 * (float)(im.mouseY() - 400) + GameManager::getInstance().mainCamera->yPos;
		gun->Fire(0, targetX, targetY);
	}

	if (im.isJoyStickMovedUp(SDL_CONTROLLER_AXIS_LEFTY)) Move(0, playerSpeed * deltaTime);
	if (im.isJoyStickMovedDown(SDL_CONTROLLER_AXIS_LEFTY)) Move(0, -playerSpeed * deltaTime);
	if (!autopilot && im.isJoyStickMovedRight(SDL_CONTROLLER_AXIS_LEFTX)) Move(playerSpeed * deltaTime, 0);
	if (!autopilot && im.isJoyStickMovedLeft(SDL_CONTROLLER_AXIS_LEFTX)) Move(-playerSpeed * deltaTime, 0);

	if (isDashing) {
		dashTimer += FrameRateController::getInstance().DeltaTime();
		if (dashTimer > dashTime) {
			isDashing = false;
			dashTimer = 0;
		}
		trans->Move(dashVelocity.x, dashVelocity.y);
	}

	if (autopilot) Sidescroll(deltaTime);

	if (badTouch && timer <= 0)
	{
		hp -= 15;
		timer = damageCooldownTimer;
	}
	else if (timer > 0)
	{
		timer -= FrameRateController::getInstance().DeltaTime();
	}
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
	return (Component*)toReturn;
}

void Player::Deserialize(nlohmann::json j, GameObject* parent) {
	if (j.contains("script")) script = j["script"];
	this->parent = parent;
}

void Player::HandleMessage(Message* e)
{
	if (e->id == Message::COLLISION)
	{
		CollisionMessage* cm = (CollisionMessage*)e;

		Move(cm->deltaPos.x, cm->deltaPos.y);

		if (e->otherObject->componentOwner->tag == "enemy")
		{
			if (damageCooldownTimer < 0)
			{
				sol::function PlayerCollidedWithEnemy = lua_player_state["PlayerCollidedWithEnemy"];
				PlayerCollidedWithEnemy();
				damageCooldownTimer = 2;
			}
		}

		/*if (cm->deltaPos.y >= 0 && (cm->deltaPos.x<0.0000001 && cm->deltaPos.x > -0.00001))
		{
			isGrounded = true;
		}*/
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
	if (hp <= 0) { GameManager::getInstance().playerDead = true; }
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

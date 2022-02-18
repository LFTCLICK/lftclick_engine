#include "Player.h"
#include "../GameManager.h"
#include "../FrameRateController.h"

void Player::Start()
{
	myTransform = parent->getComponent<Transform>();
	cam = GameManager::getInstance().mainCamera;
	EventManager::getInstance().Subscribe(Message::COLLISION, parent);
}

void Player::Update()
{
	if (Input_Manager::getInstance().isKeyPressed(SDL_SCANCODE_W))
		myTransform->Move(0, playerSpeed * FrameRateControler::getInstance().DeltaTime());
	if (Input_Manager::getInstance().isKeyPressed(SDL_SCANCODE_S))
		myTransform->Move(0, -playerSpeed * FrameRateControler::getInstance().DeltaTime());
	if (Input_Manager::getInstance().isKeyPressed(SDL_SCANCODE_D))
		myTransform->Move(playerSpeed * FrameRateControler::getInstance().DeltaTime(), 0);
	if (Input_Manager::getInstance().isKeyPressed(SDL_SCANCODE_A))
		myTransform->Move(-playerSpeed * FrameRateControler::getInstance().DeltaTime(), 0);

	if (Input_Manager::getInstance().isKeyPressed(SDL_SCANCODE_UP))
		cam->Move(0.0f, playerSpeed * FrameRateControler::getInstance().DeltaTime());
	if (Input_Manager::getInstance().isKeyPressed(SDL_SCANCODE_DOWN))
		cam->Move(0.0f, -playerSpeed * FrameRateControler::getInstance().DeltaTime());
	if (Input_Manager::getInstance().isKeyPressed(SDL_SCANCODE_RIGHT))
		cam->Move(playerSpeed * FrameRateControler::getInstance().DeltaTime(), 0.0f);
	if (Input_Manager::getInstance().isKeyPressed(SDL_SCANCODE_LEFT))
		cam->Move(-playerSpeed * FrameRateControler::getInstance().DeltaTime(), 0.0f);

	if (Input_Manager::getInstance().isJoyStickMovedUp(SDL_CONTROLLER_AXIS_LEFTY))
		myTransform->Move(0, playerSpeed * FrameRateControler::getInstance().DeltaTime());
	if (Input_Manager::getInstance().isJoyStickMovedDown(SDL_CONTROLLER_AXIS_LEFTY))
		myTransform->Move(0, -playerSpeed * FrameRateControler::getInstance().DeltaTime());
	if (Input_Manager::getInstance().isJoyStickMovedRight(SDL_CONTROLLER_AXIS_LEFTX))
		myTransform->Move(playerSpeed * FrameRateControler::getInstance().DeltaTime(), 0);
	if (Input_Manager::getInstance().isJoyStickMovedLeft(SDL_CONTROLLER_AXIS_LEFTX))
		myTransform->Move(-playerSpeed * FrameRateControler::getInstance().DeltaTime(), 0);
}

Component* Player::Clone(GameObject* newParent)
{
	Player* toReturn = new Player();
	toReturn->playerSpeed = playerSpeed;
	return (Component*)toReturn;
}

Player::Player(json j, GameObject* parent)
{
	playerSpeed = j["playerSpeed"];
	this->parent = parent;
}

void Player::HandleMessage(Message* e)
{
	if (e->id == Message::COLLISION)
	{
		CollisionMessage* cm = (CollisionMessage*)e;
		myTransform->Move(cm->deltaPos.x, cm->deltaPos.y);
		/*if (cm->deltaPos.y >= 0 && (cm->deltaPos.x<0.0000001 && cm->deltaPos.x > -0.00001))
		{
			isGrounded = true;
		}*/
		//std::cout << transform->GetPosXMVector().m128_f32[0] - lastGroundPos << std::endl;
		//lastGroundPos = transform->GetPosXMVector().m128_f32[0];
	}
}

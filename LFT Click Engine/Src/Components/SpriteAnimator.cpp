#include "pch.h"
#include "SpriteAnimator.h"
#include "FrameRateController.h"

void SpriteAnimator::Start()
{
	draw = parent->getComponent<Drawable>();
	trans = parent->getComponent<Transform>();
	timer = 0;
	xOffset = 1.0f / spriteSheetWidth;
	yOffset = 1.0f / spriteSheetHeight;
}

void SpriteAnimator::UpdateDirection()
{
	oldDirection = direction;
	auto directionVec = trans->lastMovement;
	if (directionVec.x != 0 || directionVec.y != 0) {
		if (abs(directionVec.x) > abs(directionVec.y) && moveAnimationIndices["right"] > -1) {
			if (directionVec.x > 0 && moveAnimationIndices["right"] > -1) {
				if (direction != "right") direction = "right";
			}
			else if (directionVec.x < 0 && moveAnimationIndices["left"] > -1) {
				if (direction != "left") direction = "left";
			}
		}
		else {
			if (directionVec.y > 0 && moveAnimationIndices["up"] > -1) {
				if (direction != "up") direction = "up";
			}
			else if (directionVec.y < 0 && moveAnimationIndices["down"] > -1) {
				if (direction != "down") direction = "down";
			}
		}
	}
}
void SpriteAnimator::UpdateState()
{
	if (trans->isMoving != wasMoving || direction != oldDirection || isDamaged != wasDamaged || isDead != wasDead) {
		if (isDead) {
			SwitchAnimation(deathAnimationIndex);
		}
		else if (isDamaged) {
			SwitchAnimation(damageAnimationIndex);
		}
		else {
			if (moveAnimationIndices["right"] > -1 && trans->isMoving)
				SwitchAnimation(moveAnimationIndices[moveAnimationIndices[direction] > -1 ? direction : "right"]);
			if (idleAnimationIndices["right"] > -1 && !trans->isMoving)
				SwitchAnimation(idleAnimationIndices[idleAnimationIndices[direction] > -1 ? direction : "right"]);
		}
		wasMoving = trans->isMoving;
		wasDamaged = isDamaged;
		wasDead = isDead;
	}
}
void SpriteAnimator::UpdateFrame()
{
	if (timer >= animations[currentAnimationIndex].frameDuration && !(isDead && (currentFrame == animations[currentAnimationIndex].length - 1))) {
		timer = 0;
		// don't continue cycling if at end of death animation
		++currentFrame;

		if (currentFrame < animations[currentAnimationIndex].length) {
			draw->xOffset += xOffset;
		}
		else {
			draw->xOffset = 0;
			currentFrame = 0;
		}
	}

	if (isDamaged && (damageTimer >= damageTimeout)) {
		isDamaged = false;
	}

	if (isDead && (deathTimer >= deathTimeout)) {
		isFinishedDeleting = true;
	}

}
void SpriteAnimator::Update()
{
	if (spriteSheetHeight > 1) {
		UpdateDirection();
		UpdateState();
	}

	UpdateFrame();

	float deltaTime = g_FrameRateController->DeltaTime();
	timer += deltaTime;
	if (isDamaged) damageTimer += deltaTime;
	if (isDead) deathTimer += deltaTime;
}

Component* SpriteAnimator::Clone(GameObject* newParent)
{
	SpriteAnimator* toReturn = new SpriteAnimator();
	toReturn->parent = newParent;
	toReturn->spriteSheetWidth = spriteSheetWidth;
	toReturn->spriteSheetHeight = spriteSheetHeight;
	toReturn->currentAnimationIndex = currentAnimationIndex;
	toReturn->idleAnimationIndices = idleAnimationIndices;
	toReturn->moveAnimationIndices = moveAnimationIndices;
	toReturn->damageAnimationIndex = damageAnimationIndex;
	toReturn->deathAnimationIndex = deathAnimationIndex;
	toReturn->xOffset = xOffset;
	toReturn->yOffset = yOffset;
	toReturn->timer = timer;
	toReturn->currentFrame = currentFrame;
	toReturn->animations = animations;
	toReturn->direction = direction;
	toReturn->oldDirection = oldDirection;
	toReturn->isDamaged = isDamaged;
	toReturn->damageTimer = damageTimer;
	toReturn->damageTimeout = damageTimeout;
	return toReturn;
}

void SpriteAnimator::Deserialize(nlohmann::json j, GameObject* parent)
{
	this->parent = parent;

	std::vector<std::string> directions = { "right", "left", "up", "down" };
	for (auto d : directions) {
		idleAnimationIndices[d] = -1;
		moveAnimationIndices[d] = -1;
	}
	damageAnimationIndex = -1;
	deathAnimationIndex = -1;

	spriteSheetWidth = j["spriteSheetWidth"];
	spriteSheetHeight = j["spriteSheetHeight"];

	if (j.contains("animations")) {
		int index = 0;
		for (auto it = std::begin(j["animations"]); it != std::end(j["animations"]); it++, index++) {
			AnimationInfo animationInfo;
			auto animation = *it;

			animationInfo.name = animation["name"];
			animationInfo.length = animation["length"];
			animationInfo.frameDuration = animation["frameDuration"];

			if (animation.contains("row"))
				animationInfo.row = animation["row"];
			else
				animationInfo.row = index;

			animations[index] = animationInfo;

			if (animation.contains("onIdle") && animation["onIdle"])
				idleAnimationIndices[animation.contains("direction") ? animation["direction"] : "right"] = index;
			else if (animation.contains("onMove") && animation["onMove"])
				moveAnimationIndices[animation.contains("direction") ? animation["direction"] : "right"] = index;
			else if (animation.contains("onDamage") && animation["onDamage"])
				damageAnimationIndex = index;
			else if (animation.contains("onDeath") && animation["onDeath"])
				deathAnimationIndex = index;
		}

		if (idleAnimationIndices["right"] > -1) currentAnimationIndex = idleAnimationIndices["right"];
	}
}

void SpriteAnimator::SwitchAnimation(int index) {
	currentAnimationIndex = index;
	draw->yOffset = animations[index].row * yOffset;
	currentFrame = 0;
	timer = 0;
}
void SpriteAnimator::SwitchAnimation(std::string name) {
	for (int i = 0; i < animations.size(); i++) {
		if (animations[i].name == name) {
			SwitchAnimation(i);
		}
	}
}

void SpriteAnimator::Damage(float time) {
	if (damageAnimationIndex > -1) {
		isDamaged = true;
		damageTimeout = time;
		damageTimer = 0;
	}
}

void SpriteAnimator::Die(float time) {
	std::cout << "DEAD" << std::endl;
	if (deathAnimationIndex > -1) {
		isFinishedDeleting = false;
		isDead = true;
		deathTimeout = time;
		deathTimer = 0;
	}
}

void SpriteAnimator::Revive() {
	isDead = false;
	isFinishedDeleting = true;
}
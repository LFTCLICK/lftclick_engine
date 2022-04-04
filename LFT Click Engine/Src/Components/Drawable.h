// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Drawable.h
// Author			:	Vance Howald
// Creation Date	:	2021/11/14
// Purpose			:	DX11 drawing info and draw function header
// History			:
// 2021/12/01		-	Added messaging
// ---------------------------------------------------------------------------
#pragma once

#include "pch.h"
#include "GameObject.h"
#include "Transform.h"
#include "InputManager.h"
#include "EventManager.h"
#include "Renderer.h"
#include "ConstantBuffer.h"

using json = nlohmann::json;

class Drawable : public Component
{
public:
	Drawable();
	~Drawable() = default;

	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;
	virtual Component* Clone(GameObject* newParent);
	virtual void Update() override;

	virtual int getCompId() override {return Component::DRAWABLE;};
	static int getStaticCompId() {return Component::DRAWABLE;};

	void HUD_DrawTextCenter(std::string text, DirectX::SimpleMath::Vector2 offset = { 0, 0 },
		DirectX::SimpleMath::Color color = DirectX::SimpleMath::Color(1.0f, 0.0f, 0.0f, 1.0f));
public:
	float xOffset, yOffset;
	float xScale, yScale;
	float xFlip;
	float darknessFactor;
	int drawSize;
	bool useTextures;
	bool scaleAlphaWithLight;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;
private:
	float speed;

	Transform* transformComp;
};

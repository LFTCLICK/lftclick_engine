// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Drawable.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/11/14
// Purpose			:	DX11 drawing info and draw function
// History			:
// ---------------------------------------------------------------------------
#define no_init_all

#include "pch.h"
#include "Drawable.h"
#include <d3dcompiler.h>
#include <WICTextureLoader.h>
#include "Camera.h"
#include "../GameManager.h"
#include "Helpers.h"
#include "SquareCollider.h"



using namespace Microsoft::WRL;
using namespace DirectX;

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string& str)
{
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

Drawable::Drawable()
{
	xFlip = 1;
	xOffset = yOffset = 0;
	scaleAlphaWithLight = false;
}

void Drawable::Update() {
	if (scaleAlphaWithLight) { 
		darknessFactor = g_GameManager->GetDarknessLevel();
	}
}

void Drawable::Deserialize(nlohmann::json j, GameObject* componentOwner)
{
	xScale = j["xScale"];
	yScale = j["yScale"];
	if (j["onlyUVOverrides"])
		return;

	darknessFactor = 1;
	xOffset = yOffset = 0;
	xFlip = 1;
	if (j.contains("scaleAlphaWithLight")) scaleAlphaWithLight = j["scaleAlphaWithLight"];

	this->componentOwner = componentOwner;

	std::wstring sprite = utf8_decode(j["image"]);
	//force DTK to not load the texture as srgb
	DirectX::CreateWICTextureFromFileEx(g_Renderer->GetDevice(), sprite.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, DirectX::WIC_LOADER_IGNORE_SRGB, nullptr, &textureSRV);
	//	DirectX::CreateWICTextureFromFile(g_Renderer->GetDevice(), sprite.c_str(), &texture, &shaderResourceView);


	transformComp = componentOwner->getComponent<Transform>();
	assert(transformComp != nullptr);
}

Component* Drawable::Clone(GameObject* newParent)
{
	Drawable* toReturn = new Drawable();

	toReturn->xScale = xScale;
	toReturn->yScale = yScale;
	toReturn->drawSize = drawSize;
	toReturn->speed = speed;
	toReturn->componentOwner = newParent;
	toReturn->transformComp = newParent->getComponent<Transform>();
	toReturn->darknessFactor = darknessFactor;
	toReturn->scaleAlphaWithLight = scaleAlphaWithLight;

	textureSRV.CopyTo(toReturn->textureSRV.GetAddressOf());

	return (Component*)toReturn;
}

void Drawable::HUD_DrawTextCenter(std::string text, SimpleMath::Vector2 offset, DirectX::SimpleMath::Color color)
{
	SimpleMath::Vector2 pos = g_GameManager->mainCamera->WorldToScreenPos(
		transformComp->CurrentPos(),
		g_Renderer->GetWidth(), g_Renderer->GetHeight()
	);

	pos.x -= XMVectorGetX(g_Renderer->GetSpriteFont()->MeasureString(text.c_str(), false)) / 2.0f;
	pos.y -= XMVectorGetY(g_Renderer->GetSpriteFont()->MeasureString(text.c_str(), false)) / 2.0f;


	g_Renderer->GetSpriteFont()->DrawString(g_Renderer->GetSpriteBatch(), text.c_str(), pos + offset, color);
}

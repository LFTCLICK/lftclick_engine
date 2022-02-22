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
#include "../InputManager.h"
#include "../EventManager.h"
#include "../Graphics.h"


using json = nlohmann::json;
class Drawable : public Component
{
public:
	Drawable();
	//Drawable(json j, GameObject* parent);
	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;
	virtual Component* Clone(GameObject* newParent);
	virtual void Start();
	virtual void Update();
	virtual void Draw();
	virtual int getCompId();
	~Drawable() = default;
	float xOffset, yOffset;
	float xFlip;
	float alphaOverride;
private:
	int drawSize;
	bool useTextures;
	bool hasRasterizer = false;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuf;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Resource> texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	DirectX::XMMATRIX pTransformationMatrix;
	struct Vertex
	{
		float cordX;
		float cordY;
		float cordZ;
		float textureX;
		float textureY;
	};
	float speed;
	GameObject* parent;
};
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
	struct  VS_cbPerObject
	{
		DirectX::XMMATRIX transform;
		DirectX::XMFLOAT2 Offset;
		DirectX::XMFLOAT2 Scale;

		float flipX;
	};
	static_assert(sizeof(VS_cbPerObject) % 16 == 0, "Not 16-bytes aligned");

	struct PS_cbPerObject
	{
		float alphaOverride;
		DirectX::XMFLOAT3 padding;
	};
	static_assert(sizeof(PS_cbPerObject) % 16 == 0, "Not 16-bytes aligned");

	struct Vertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT2 TexCoord;
	};


public:
	Drawable();
	~Drawable() = default;

	virtual void Deserialize(nlohmann::json j, GameObject* componentOwner) override;
	virtual Component* Clone(GameObject* newParent);
	virtual void Start();
	virtual void Update();
	virtual void Draw();
	virtual int getCompId();

	void HUD_DrawTextCenter(std::string text, DirectX::SimpleMath::Vector2 offset = { 0, 0 },
		DirectX::SimpleMath::Color color = DirectX::SimpleMath::Color(1.0f, 0.0f, 0.0f, 1.0f));
public:
	float xOffset, yOffset;
	float xScale, yScale;
	float xFlip;
	float alphaOverride;
	int drawSize;
	bool useTextures;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuf;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

	DirectX::XMMATRIX pTransformationMatrix;

	float speed;

	ConstantBuffer<VS_cbPerObject> VS_cbPerObjectData;
	ConstantBuffer<PS_cbPerObject> PS_cbPerObjectData;

	Transform* transformComp;
};

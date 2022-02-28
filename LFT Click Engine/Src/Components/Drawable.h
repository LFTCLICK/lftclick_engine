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
#include "Graphics.h"
#include "ConstantBuffer.h"

using json = nlohmann::json;
class Drawable : public Component
{
	struct  cbPerObject
	{
		DirectX::XMMATRIX transform;
		DirectX::XMFLOAT2 Offset;
		DirectX::XMFLOAT2 Scale;
		float flipX;
		float alphaOverride;
		DirectX::XMFLOAT2 padding;
	};
	static_assert(sizeof(cbPerObject) % 16 == 0, "Not 16-bytes aligned");
	struct Vertex
	{
		float cordX;
		float cordY;
		float cordZ;
		float textureX;
		float textureY;
	};
public:
	Drawable();
	~Drawable() = default;

	virtual void Deserialize(nlohmann::json j, GameObject* parent) override;
	virtual Component* Clone(GameObject* newParent);
	virtual void Start();
	virtual void Update();
	virtual void Draw();
	virtual int getCompId();

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
	Microsoft::WRL::ComPtr<ID3D11Resource> texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rastState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	DirectX::XMMATRIX pTransformationMatrix;

	float speed;
	GameObject* parent;

	ConstantBuffer<cbPerObject> cbPerObjectData;
};
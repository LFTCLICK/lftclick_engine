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
#include "GameObject.h"
#include "Transform.h"
#include "../InputManager.h"
#include <json.hpp>
#include <DirectXMath.h>
#include <d3d11.h>
#include "../EventManager.h"
#include "../Graphics/Graphics.h"

using json = nlohmann::json;
class Drawable : public Component
{
public:
	Drawable();
	Drawable(json j, GameObject* parent);
	Component* clone(GameObject* newParent);
	void Start();
	void Update();
	void Draw();
	int getCompId();
	~Drawable() = default;
	float xOffset, yOffset;
	float xFlip;
	float alphaOverride;
private:
	int drawSize;
	bool useTextures;
	bool hasRasterizer = false;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Resource> pTextureResource;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterizer;
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;
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
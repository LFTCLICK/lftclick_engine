// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	DebugRenderer.h
// Author			:	Abhijit Zala
// Creation Date	:	2022/02/26
// Purpose			:	Debug drawing using DirectXTK
// History			:
// ---------------------------------------------------------------------------

#pragma once

#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <Effects.h>
#include "CommonStates.h"


class DebugRenderer
{
public:
	DebugRenderer(ID3D11Device* device, ID3D11DeviceContext* context);
	~DebugRenderer();

	void DrawLine(DirectX::SimpleMath::Vector2 a, DirectX::SimpleMath::Vector2 b);

	void DrawQuad(DirectX::SimpleMath::Vector2 a, DirectX::SimpleMath::Vector2 b,
			      DirectX::SimpleMath::Vector2 c, DirectX::SimpleMath::Vector2 d);

	void DrawCircle(DirectX::SimpleMath::Vector2 center, float radius, short numlines);
	
	void Draw(ID3D11DeviceContext* context, int width, int height);

private:
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> primitiveBatch;
	std::unique_ptr<DirectX::BasicEffect> basicEffect;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	std::unique_ptr<DirectX::CommonStates> states;

	std::vector<DirectX::VertexPositionColor> lineVertices;

};

extern std::unique_ptr<DebugRenderer> g_DebugRenderer;

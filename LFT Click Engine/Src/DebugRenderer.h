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

	//Draw a Line from point 'a' to point 'b'
	void DrawLine(DirectX::SimpleMath::Vector2 a, DirectX::SimpleMath::Vector2 b);

	//Draws a Quad by drawing four lines -> a-b, b-c, c-d, and d-a
	void DrawQuad(DirectX::SimpleMath::Vector2 a, DirectX::SimpleMath::Vector2 b,
			      DirectX::SimpleMath::Vector2 c, DirectX::SimpleMath::Vector2 d);

	//Approximates a circle using straight lines 'numlines'
	void DrawCircle(DirectX::SimpleMath::Vector2 center, float radius, short numlines);
	
	//Submits all the batched drawing from the above draw calls to the GPU 
	void Draw(ID3D11DeviceContext* context, int screenwidth, int screenheight);

private:
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> primitiveBatch;
	std::unique_ptr<DirectX::BasicEffect> basicEffect;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	std::unique_ptr<DirectX::CommonStates> states;

	std::vector<DirectX::VertexPositionColor> lineVertices;

};

extern std::unique_ptr<DebugRenderer> g_DebugRenderer;

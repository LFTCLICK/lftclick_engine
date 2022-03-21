// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	DebugRenderer.cpp
// Author			:	Abhijit Zala
// Creation Date	:	2022/02/26
// Purpose			:	Debug drawing using DirectXTK
// History			:
// ---------------------------------------------------------------------------


#include "pch.h"
#include "DebugRenderer.h"
#include <DirectXHelpers.h>

using namespace DirectX;

DebugRenderer::DebugRenderer(ID3D11Device* device, ID3D11DeviceContext* context)
{
	primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);
	basicEffect = std::make_unique<BasicEffect>(device);

	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionColor>( device, basicEffect.get(), 
			inputLayout.ReleaseAndGetAddressOf() ) );

	states = std::make_unique<CommonStates>(device);
}

DebugRenderer::~DebugRenderer()
{
}

void DebugRenderer::DrawLine(SimpleMath::Vector2 a, SimpleMath::Vector2 b)
{
	lineVertices.emplace_back(VertexPositionColor(a, Colors::Red));
	lineVertices.emplace_back(VertexPositionColor(b, Colors::Red));
}


void DebugRenderer::DrawQuad(SimpleMath::Vector2 a, SimpleMath::Vector2 b, SimpleMath::Vector2 c, SimpleMath::Vector2 d)
{
	DrawLine(a, b);
	DrawLine(b, c);
	DrawLine(c, d);
	DrawLine(d, a);
}

void DebugRenderer::DrawCircle(SimpleMath::Vector2 Center, float Radius, short numlines)
{
	float angle = XMConvertToRadians(360.0f / numlines);

	float cosAngle = cos(angle);
	float sinAngle = sin(angle);

	SimpleMath::Vector2 vec(Radius, 0);

	for (short i = 0; i < numlines; ++i)
	{
		SimpleMath::Vector2 rot(cosAngle*vec.x - sinAngle*vec.y, sinAngle*vec.x + cosAngle*vec.y);
		rot += Center;
		vec += Center;
		DrawLine(vec, rot);
		vec = rot - Center;
	}
}

void DebugRenderer::Draw(ID3D11DeviceContext* context, int width, int height)
{
	basicEffect->SetProjection(XMMatrixOrthographicOffCenterRH(0, width, height, 0, 0, 1));

	basicEffect->SetColorAndAlpha(DirectX::Colors::Red);

	context->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(states->DepthNone(), 0);
	context->RSSetState(states->CullNone());

	basicEffect->Apply(context);
	context->IASetInputLayout(inputLayout.Get());

	primitiveBatch->Begin();

	for (UINT i = 0; i < lineVertices.size(); i += 2)
	{
		primitiveBatch->DrawLine(lineVertices[i], lineVertices[i + 1]);
	}

	primitiveBatch->End();

	context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(nullptr, 0);
	context->RSSetState(nullptr);

	lineVertices.clear();
}

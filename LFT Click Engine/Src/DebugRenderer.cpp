#include "pch.h"
#include "DebugRenderer.h"
#include "GameManager.h"
#include "Graphics.h"
#include <DirectXHelpers.h>

using namespace DirectX;

DebugRenderer::DebugRenderer(Graphics* graphics)
{
	primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(graphics->GetContext());
	basicEffect = std::make_unique<BasicEffect>(graphics->GetDevice());

	DX::ThrowIfFailed(
		CreateInputLayoutFromEffect<VertexPositionColor>(graphics->GetDevice(),
			basicEffect.get(),
			inputLayout.ReleaseAndGetAddressOf()));

	states = std::make_unique<DirectX::CommonStates>(graphics->GetDevice());
}

DebugRenderer::~DebugRenderer()
{
}

void DebugRenderer::DrawLine(DirectX::SimpleMath::Vector2 a, DirectX::SimpleMath::Vector2 b)
{
	lineVertices.emplace_back(DirectX::VertexPositionColor(a, DirectX::Colors::Red));
	lineVertices.emplace_back(DirectX::VertexPositionColor(b, DirectX::Colors::Red));
}

void DebugRenderer::DrawQuad(DirectX::SimpleMath::Vector2 a, DirectX::SimpleMath::Vector2 b, DirectX::SimpleMath::Vector2 c, DirectX::SimpleMath::Vector2 d)
{
	DrawLine(a, b);
	DrawLine(b, c);
	DrawLine(c, d);
	DrawLine(d, a);
}

void DebugRenderer::Draw(Graphics* graphics)
{
	basicEffect->SetWorld(XMMatrixIdentity());
	basicEffect->SetView(XMMatrixIdentity());
	basicEffect->SetProjection(XMMatrixOrthographicOffCenterRH(0,
		graphics->GetWidth(), graphics->GetHeight(), 0, 0, 1));

	basicEffect->SetColorAndAlpha(DirectX::Colors::Red);

	graphics->GetContext()->OMSetBlendState(states->Opaque(), nullptr, 0xFFFFFFFF);
	graphics->GetContext()->OMSetDepthStencilState(states->DepthNone(), 0);
	graphics->GetContext()->RSSetState(states->CullNone());

	basicEffect->Apply(graphics->GetContext());
	graphics->GetContext()->IASetInputLayout(inputLayout.Get());

	primitiveBatch->Begin();

	for (UINT i = 0; i < lineVertices.size(); i += 2)
	{
		primitiveBatch->DrawLine(lineVertices[i], lineVertices[i + 1]);
	}

	primitiveBatch->End();

	graphics->GetContext()->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	graphics->GetContext()->OMSetDepthStencilState(nullptr, 0);
	graphics->GetContext()->RSSetState(nullptr);

	lineVertices.clear();
}

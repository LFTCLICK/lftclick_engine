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
	/*quadVertices.emplace_back(DirectX::VertexPositionColor(a, DirectX::Colors::Red));
	quadVertices.emplace_back(DirectX::VertexPositionColor(b, DirectX::Colors::Red));
	quadVertices.emplace_back(DirectX::VertexPositionColor(c, DirectX::Colors::Red));
	quadVertices.emplace_back(DirectX::VertexPositionColor(d, DirectX::Colors::Red));*/
	DrawLine(a, b);
	DrawLine(b, c);
	DrawLine(c, d);
	DrawLine(d, a);
}

void DebugRenderer::Draw(Graphics* graphics)
{
	basicEffect->SetProjection(XMMatrixOrthographicOffCenterRH(0,
		graphics->GetWidth(), graphics->GetHeight(), 0, 0, 1));

	basicEffect->SetVertexColorEnabled(true);
	basicEffect->SetDiffuseColor(DirectX::Colors::Blue);

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

	for (UINT i = 0; i < quadVertices.size(); i += 4)
	{
		primitiveBatch->DrawQuad(quadVertices[i], quadVertices[i + 1], quadVertices[i + 2], quadVertices[i + 3]);
	}

	primitiveBatch->End();

	graphics->GetContext()->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	graphics->GetContext()->OMSetDepthStencilState(nullptr, 0);
	graphics->GetContext()->RSSetState(nullptr);

	lineVertices.clear();
	quadVertices.clear();
}

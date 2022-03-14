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

namespace
{
#include "Shaders/Compiled/Compiled_PS.h"
#include "Shaders/Compiled/Compiled_VS.h"
}

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
}

void Drawable::Deserialize(nlohmann::json j, GameObject* parent)
{
	xScale = j["xScale"];
	yScale = j["yScale"];
	if (j["onlyUVOverrides"])
		return;

	alphaOverride = 1;
	xOffset = yOffset = 0;
	xFlip = 1;
	this->parent = parent;

	useTextures = true;
	std::vector<Vertex> vertexVector;
	std::vector<unsigned short> indices;
	json vertexData = j["vertices"];

	for (json::iterator vertex = vertexData.begin(); vertex != vertexData.end(); ++vertex)//process data or overrides
	{
		Vertex temp = {};
		temp.cordY = vertex.value()["pos"][1];
		temp.cordX = vertex.value()["pos"][0];
		temp.cordZ = vertex.value()["pos"][2];
		temp.textureX = vertex.value()["uv"][0];
		temp.textureY = vertex.value()["uv"][1];
		vertexVector.push_back(temp);

	}
	json indicies = j["indicies"];
	drawSize = 0;
	for (json::iterator index = indicies.begin(); index != indicies.end(); ++index)//process data or overrides
	{
		indices.push_back(index.value());
		++drawSize;
	}
	D3D11_BUFFER_DESC vertBufDes = {};
	vertBufDes.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertBufDes.Usage = D3D11_USAGE_DEFAULT;
	vertBufDes.CPUAccessFlags = 0;
	vertBufDes.MiscFlags = 0;
	vertBufDes.ByteWidth = vertexVector.size() * sizeof(Vertex);
	vertBufDes.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = &(vertexVector[0]);
	g_Renderer->GetDevice()->CreateBuffer(&vertBufDes, &sd, &vertBuf);

	D3D11_BUFFER_DESC indexBufDes = {};
	indexBufDes.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDes.Usage = D3D11_USAGE_DEFAULT;
	indexBufDes.CPUAccessFlags = 0;
	indexBufDes.MiscFlags = 0;
	indexBufDes.ByteWidth = indices.size() * sizeof(unsigned short);
	indexBufDes.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA sdIndex = {};
	sdIndex.pSysMem = &(indices[0]);
	g_Renderer->GetDevice()->CreateBuffer(&indexBufDes, &sdIndex, &indexBuf);


	g_Renderer->GetDevice()->CreatePixelShader(g_CompiledPS, sizeof(g_CompiledPS), nullptr, &pixelShader);
	g_Renderer->GetDevice()->CreateVertexShader(g_CompiledVS, sizeof(g_CompiledVS), nullptr, &vertShader);

	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	g_Renderer->GetDevice()->CreateInputLayout(
		ied, (UINT)std::size(ied),
		g_CompiledVS,
		sizeof(g_CompiledVS),
		&inputLayout
	);

	std::wstring sprite = utf8_decode(j["image"]);
	//force DTK to not load the texture as srgb
	DirectX::CreateWICTextureFromFileEx(g_Renderer->GetDevice(), sprite.c_str(), 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, DirectX::WIC_LOADER_IGNORE_SRGB, nullptr, &shaderResourceView);
	//	DirectX::CreateWICTextureFromFile(g_Renderer->GetDevice(), sprite.c_str(), &texture, &shaderResourceView);

	D3D11_SAMPLER_DESC sampDes = {};
	sampDes.Filter = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
	sampDes.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDes.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDes.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	g_Renderer->GetDevice()->CreateSamplerState(&sampDes, &sampState);

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	g_Renderer->GetDevice()->CreateBlendState(&blendDesc, &blendState);
}

Component* Drawable::Clone(GameObject* newParent)
{
	Drawable* toReturn = new Drawable();

	toReturn->xScale = xScale;
	toReturn->yScale = yScale;
	toReturn->drawSize = drawSize;
	toReturn->useTextures = useTextures;
	toReturn->speed = speed;
	toReturn->parent = newParent;
	toReturn->pTransformationMatrix = pTransformationMatrix;


	vertBuf.CopyTo(toReturn->vertBuf.GetAddressOf());
	indexBuf.CopyTo(toReturn->indexBuf.GetAddressOf());
	pixelShader.CopyTo(toReturn->pixelShader.GetAddressOf());
	inputLayout.CopyTo(toReturn->inputLayout.GetAddressOf());
	
	vertShader.CopyTo(toReturn->vertShader.GetAddressOf());
	shaderResourceView.CopyTo(toReturn->shaderResourceView.GetAddressOf());
	sampState.CopyTo(toReturn->sampState.GetAddressOf());
	rastState.CopyTo(toReturn->rastState.GetAddressOf());
	blendState.CopyTo(toReturn->blendState.GetAddressOf());
	toReturn->alphaOverride = alphaOverride;
	return (Component*)toReturn;
}

void Drawable::Start()
{
	cbPerObjectData.Create(g_Renderer->GetDevice());
}

void Drawable::Update()
{
}

void Drawable::Draw()
{
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0;
	g_Renderer->GetContext()->IASetVertexBuffers(0, 1, vertBuf.GetAddressOf(), &stride, &offset);
	g_Renderer->GetContext()->IASetIndexBuffer(indexBuf.Get(), DXGI_FORMAT_R16_UINT, 0);
	g_Renderer->GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);
	g_Renderer->GetContext()->VSSetShader(vertShader.Get(), nullptr, 0);
	g_Renderer->GetContext()->IASetInputLayout(inputLayout.Get());
	g_Renderer->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DirectX::XMFLOAT4X4 mat = parent->getComponent<Transform>()->GetXMMatrix();

	const cbPerObject cb =
	{
		{
			DirectX::XMLoadFloat4x4(&mat) * GameManager::getInstance().mainCamera->GetProjectionMatrix()
		},
		XMFLOAT2(xOffset, yOffset),
		XMFLOAT2(xScale, yScale),
		xFlip,
		alphaOverride
	};
	cbPerObjectData.SetData(g_Renderer->GetContext(), cb);

	g_Renderer->GetContext()->IASetVertexBuffers(0, 1, vertBuf.GetAddressOf(), &stride, &offset);
	g_Renderer->GetContext()->IASetIndexBuffer(indexBuf.Get(), DXGI_FORMAT_R16_UINT, 0);
	g_Renderer->GetContext()->PSSetShader(pixelShader.Get(), nullptr, 0);
	g_Renderer->GetContext()->VSSetShader(vertShader.Get(), nullptr, 0);
	g_Renderer->GetContext()->IASetInputLayout(inputLayout.Get());
	g_Renderer->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_Renderer->GetContext()->VSSetConstantBuffers(0, 1, cbPerObjectData.GetAddressOf());
	g_Renderer->GetContext()->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
	g_Renderer->GetContext()->PSSetSamplers(0, 1, sampState.GetAddressOf());
	g_Renderer->GetContext()->OMSetBlendState(blendState.Get(), nullptr, 0xFFFFFFFFu);
	g_Renderer->GetContext()->RSSetState(rastState.Get());

	g_Renderer->GetContext()->DrawIndexed(drawSize, 0, 0);
}

int Drawable::getCompId()
{
	return Component::DRAWABLE;
}

// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Drawable.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/11/14
// Purpose			:	DX11 drawing info and draw function
// History			:
// ---------------------------------------------------------------------------
#define no_init_all
#include "Drawable.h"
#include <d3dcompiler.h>
#include <WICTextureLoader.h>
#include "Camera.h"
#include "../GameManager.h"


namespace wrl = Microsoft::WRL;
namespace dx = DirectX;


Drawable::Drawable()
{
	xFlip = 1;
	xOffset = yOffset = 0;
}

Drawable::Drawable(json j, GameObject * parent)
{
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
	//DX11 Setup
}

Component * Drawable::clone(GameObject * newParent)
{
	Drawable* toReturn = new Drawable();

	toReturn->drawSize = drawSize;
	toReturn->useTextures = useTextures;
	toReturn->hasRasterizer = hasRasterizer;
	toReturn->speed = speed;
	toReturn->parent = newParent;
	toReturn->pTransformationMatrix = pTransformationMatrix;


	pVertexBuffer.CopyTo(toReturn->pVertexBuffer.GetAddressOf());
	pIndexBuffer.CopyTo(toReturn->pIndexBuffer.GetAddressOf());
	pPixelShader.CopyTo(toReturn->pPixelShader.GetAddressOf());
	pInputLayout.CopyTo(toReturn->pInputLayout.GetAddressOf());
	
	pVertexShader.CopyTo(toReturn->pVertexShader.GetAddressOf());
	pTextureResource.CopyTo(toReturn->pTextureResource.GetAddressOf());
	pShaderResourceView.CopyTo(toReturn->pShaderResourceView.GetAddressOf());
	pSampler.CopyTo(toReturn->pSampler.GetAddressOf());
	pRasterizer.CopyTo(toReturn->pRasterizer.GetAddressOf());
	pBlender.CopyTo(toReturn->pBlender.GetAddressOf());
	toReturn->alphaOverride = alphaOverride;
	return (Component*)toReturn;
}

void Drawable::Start()
{
}

void Drawable::Update()
{
}

void Drawable::Draw()
{
	
}

int Drawable::getCompId()
{
	return Component::DRAWABLE;
}

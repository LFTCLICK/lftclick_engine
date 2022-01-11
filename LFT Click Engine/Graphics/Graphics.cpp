// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Graphics.cpp
// Author			:	Vance Howald
// Creation Date	:	2021/11/14
// Purpose			:	DX11 Rendering stuff
// History			:
// 2021/12/01		-	Fixed color space issue
// ---------------------------------------------------------------------------
#include "Graphics.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <WICTextureLoader.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

Graphics::Graphics()
{
}

void Graphics::init(HWND hWnd, int width, int height)
{
	this->width = width;
	this->height = height;
	//setup devices

}

void Graphics::Draw()
{

}

void Graphics::EndFrame()
{
	HRESULT hr;
	if (FAILED(hr = pSwap->Present(0u, 0u)))
	{
		//throw hr
	}

}

int Graphics::getWidth()
{
	return width;
}

int Graphics::getHeight()
{
	return height;
}

ID3D11DeviceContext * Graphics::GetContext()
{
	return pContext.Get();
}

ID3D11Device * Graphics::GetDevice()
{
	return pDevice.Get();
}

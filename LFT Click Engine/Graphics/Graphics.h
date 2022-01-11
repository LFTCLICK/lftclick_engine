// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Graphics.h
// Author			:	Vance Howald
// Creation Date	:	2021/11/14
// Purpose			:	DX11 Rendering stuff
// History			:
// ---------------------------------------------------------------------------
#pragma once
#include "Windows.h"
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>

class Graphics
{
public:

	static Graphics& getInstance()
	{
		static Graphics instance;
		return instance;
	}
	Graphics();
	void init(HWND hWnd, int width, int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;
	void DoTransform(DirectX::XMMATRIX t);
	void EndFrame();

	void ClearBuffer(UINT32 hexCode) noexcept
	{
		int red = (hexCode & 0xFF0000) >> 16;
		int green = (hexCode & 0x00FF00) >> 8;
		int blue = hexCode & 0x0000FF;
		ClearBuffer((float)red / 255, (float)green / 255, (float)blue / 255);
	}
	void ClearBuffer(float red, float green, float blue) noexcept
	{
		const float color[] = { red,green,blue,1.0f };
		pContext->ClearRenderTargetView(pTarget.Get(), color);
		pContext->ClearDepthStencilView(pDSV.Get(), D3D11_CLEAR_DEPTH, 1, 0);
	}
	void Draw();
	int getWidth();
	int getHeight();
	ID3D11DeviceContext* GetContext();
	ID3D11Device* GetDevice();


private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
	int width, height;
};
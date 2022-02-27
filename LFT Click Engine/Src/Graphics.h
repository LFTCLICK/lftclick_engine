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
#include "DebugRenderer.h"

class Graphics
{
public:

	static Graphics& getInstance()
	{
		static Graphics instance;
		return instance;
	}

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
		immediateContext->ClearRenderTargetView(renderTargetView.Get(), color);
		immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1, 0);
	}
public:
	Graphics();
	~Graphics() = default;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;

	void Initialize(HWND hWnd, int initWidth, int initHeight);
	void EndFrame();

	void Draw();
	int GetWidth();
	int GetHeight();
	ID3D11DeviceContext* GetContext();
	ID3D11Device* GetDevice();
	void OnResize(int newWidth, int newHeight);

private:
	void UpdateClientSizeVars();

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	int width, height;

	DXGI_FORMAT backBufferFormat;
	DXGI_FORMAT depthStencilBufferFormat;
	DXGI_FORMAT depthStencilViewFormat;

	D3D11_VIEWPORT screenViewport;

	UINT msaaQuality;
	UINT msaaSampleCount;

};
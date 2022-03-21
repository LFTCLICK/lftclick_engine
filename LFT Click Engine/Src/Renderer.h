// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Renderer.h
// Author			:	Vance Howald, Abhijit Zala
// Creation Date	:	2021/11/14
// Purpose			:	DX11 Rendering stuff
// History			:
// ---------------------------------------------------------------------------
#pragma once
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include "ConstantBuffer.h"
#include "CommonStates.h"

class Renderer
{
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;

	DXGI_FORMAT backBufferFormat;
	DXGI_FORMAT depthStencilBufferFormat;
	DXGI_FORMAT depthStencilViewFormat;

	D3D11_VIEWPORT screenViewport;

	UINT msaaQuality;
	UINT msaaSampleCount;

	int width, height;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	struct  VS_cbPerObject
	{
		DirectX::XMMATRIX transform;
		DirectX::XMFLOAT2 Offset;
		DirectX::XMFLOAT2 Scale;

		float flipX;
	};
	static_assert(sizeof(VS_cbPerObject) % 16 == 0, "Not 16-bytes aligned");

	struct PS_cbPerObject
	{
		float alphaOverride;
		DirectX::XMFLOAT3 padding;
	};
	static_assert(sizeof(PS_cbPerObject) % 16 == 0, "Not 16-bytes aligned");

	struct VertexType
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT2 TexCoord;
	};

	ConstantBuffer<VS_cbPerObject> VS_cbPerObjectData;
	ConstantBuffer<PS_cbPerObject> PS_cbPerObjectData;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertBuf;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuf;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	std::unique_ptr<DirectX::CommonStates> states;

public:
	Renderer();
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void Initialize(HWND hWnd, int initWidth, int initHeight);
	void InitImGui(SDL_Window* pWindow);

	void PrepareForRendering();
	void Draw();
	void PresentFrame();
	void OnResize(int newWidth, int newHeight);

	ID3D11DeviceContext* GetContext() const { return immediateContext.Get(); }
	ID3D11Device* GetDevice() const { return device.Get(); }
	DirectX::SpriteBatch* GetSpriteBatch() const { return spriteBatch.get(); }
	DirectX::SpriteFont* GetSpriteFont() const { return spriteFont.get(); }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
private:
	void UpdateClientSizeVars();
	void CreateDeviceDependentResources();
};

extern std::unique_ptr<Renderer> g_Renderer;

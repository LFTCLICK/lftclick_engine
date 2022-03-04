// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Graphics.cpp
// Author			:	Vance Howald, Abhijit Zala
// Creation Date	:	2021/11/14
// Purpose			:	DX11 Rendering stuff
// History			:
// 2021/12/01		-	Fixed color space issue
// ---------------------------------------------------------------------------
#include "pch.h"
#include "Graphics.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <WICTextureLoader.h>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;


Graphics::Graphics() :
	width(0), height(0),
	backBufferFormat(DXGI_FORMAT_B8G8R8A8_UNORM),
	depthStencilBufferFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
	depthStencilViewFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
	msaaQuality(0),
	msaaSampleCount(4)
{
}

void Graphics::Initialize(HWND hWnd, int initWidth, int initHeight)
{
	//setup devices

	UINT flags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDevice(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		device.ReleaseAndGetAddressOf(),
		&featureLevel,
		immediateContext.ReleaseAndGetAddressOf());

	if (FAILED(hr))
		assert(false);

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
		assert(false);


	DXGI_SWAP_CHAIN_DESC sd;

	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = backBufferFormat;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	DX::ThrowIfFailed(device->CheckMultisampleQualityLevels(backBufferFormat, msaaSampleCount, &msaaQuality));
	assert(msaaQuality > 0);

	sd.SampleDesc.Count = msaaSampleCount;
	sd.SampleDesc.Quality = msaaQuality - 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	wrl::ComPtr<IDXGIDevice> dxgiDevice;
	if FAILED(device.As(&dxgiDevice))
		assert(false);


	wrl::ComPtr<IDXGIAdapter> dxgiAdapter;
	if FAILED(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()))
		assert(false);


	wrl::ComPtr<IDXGIFactory1> dxgiFactory;
	if FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), &dxgiFactory))
		assert(false);

	if FAILED(dxgiFactory->CreateSwapChain(device.Get(), &sd, swapChain.ReleaseAndGetAddressOf()))
		assert(false);


	//disable ALT-enter fullscreen
	dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES);

	OnResize(initWidth, initHeight);

}

void Graphics::PrepareForRendering()
{
	immediateContext->ClearRenderTargetView(renderTargetView.Get(), DirectX::Colors::Red);
	immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Graphics::UpdateClientSizeVars()
{
	//TODO -> Get current width/height from resized window
}

void Graphics::OnResize(int newWidth, int newHeight)
{
	this->width = newWidth;
	this->height = newHeight;
	UpdateClientSizeVars();

	renderTargetView.Reset();
	depthStencilView.Reset();
	depthStencilBuffer.Reset();

	DX::ThrowIfFailed(swapChain->ResizeBuffers(1, width, height, backBufferFormat, 0));

	wrl::ComPtr<ID3D11Texture2D> backBuffer;
	DX::ThrowIfFailed(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())));
	DX::ThrowIfFailed(device->CreateRenderTargetView(backBuffer.Get(), 0, renderTargetView.ReleaseAndGetAddressOf()));

	backBuffer.Reset();

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.Format = depthStencilBufferFormat;
	depthStencilDesc.SampleDesc.Count = msaaSampleCount;
	depthStencilDesc.SampleDesc.Quality = msaaQuality - 1;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	DX::ThrowIfFailed(device->CreateTexture2D(&depthStencilDesc, 0, depthStencilBuffer.ReleaseAndGetAddressOf()));


	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC(
		depthStencilBuffer.Get(),
		msaaSampleCount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D,
		depthStencilViewFormat);

	DX::ThrowIfFailed(device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.ReleaseAndGetAddressOf()));

	immediateContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

	screenViewport.TopLeftX = 0.0f;
	screenViewport.TopLeftY = 0.0f;
	screenViewport.MinDepth = 0.0f;
	screenViewport.MaxDepth = 1.0f;
	screenViewport.Width = static_cast<float>(width);
	screenViewport.Height = static_cast<float>(height);

	immediateContext->RSSetViewports(1, &screenViewport);
}

void Graphics::Draw()
{

}

void Graphics::PresentFrame()
{
	DX::ThrowIfFailed(swapChain->Present(0, 0));
}

int Graphics::GetWidth()
{
	return this->width;
}

int Graphics::GetHeight()
{
	return this->height;
}

ID3D11DeviceContext * Graphics::GetContext()
{
	return immediateContext.Get();
}

ID3D11Device * Graphics::GetDevice()
{
	return device.Get();
}

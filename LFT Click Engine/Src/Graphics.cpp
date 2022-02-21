// ---------------------------------------------------------------------------
// Project Name		:	LFTClick Engine
// File Name		:	Graphics.cpp
// Author			:	Vance Howald
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
	m_BackBufferFormat(DXGI_FORMAT_B8G8R8A8_UNORM),
	m_DepthStencilBufferFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
	m_DepthStencilViewFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
	m_MSAAQuality(0),
	M_MSAASampleCount(4)
{
}

void Graphics::init(HWND hWnd, int initWidth, int initHeight)
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
		pDevice.ReleaseAndGetAddressOf(),
		&featureLevel,
		pContext.ReleaseAndGetAddressOf());

	if (FAILED(hr))
		assert(false);

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
		assert(false);


	DXGI_SWAP_CHAIN_DESC sd;

	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = m_BackBufferFormat;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

	DX::ThrowIfFailed(pDevice->CheckMultisampleQualityLevels(m_BackBufferFormat, M_MSAASampleCount, &m_MSAAQuality));
	assert(m_MSAAQuality > 0);

	sd.SampleDesc.Count = M_MSAASampleCount;
	sd.SampleDesc.Quality = m_MSAAQuality - 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	wrl::ComPtr<IDXGIDevice> dxgiDevice;
	if FAILED(pDevice.As(&dxgiDevice))
		assert(false);


	wrl::ComPtr<IDXGIAdapter> dxgiAdapter;
	if FAILED(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()))
		assert(false);


	wrl::ComPtr<IDXGIFactory1> dxgiFactory;
	if FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), &dxgiFactory))
		assert(false);

	if FAILED(dxgiFactory->CreateSwapChain(pDevice.Get(), &sd, pSwap.ReleaseAndGetAddressOf()))
		assert(false);


	//disable ALT-enter fullscreen
	dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES);

	OnResize(initWidth, initHeight);
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

	pRTV.Reset();
	pDSV.Reset();
	pDSBuffer.Reset();

	DX::ThrowIfFailed(pSwap->ResizeBuffers(1, width, height, m_BackBufferFormat, 0));

	wrl::ComPtr<ID3D11Texture2D> backBuffer;
	DX::ThrowIfFailed(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf())));
	DX::ThrowIfFailed(pDevice->CreateRenderTargetView(backBuffer.Get(), 0, pRTV.ReleaseAndGetAddressOf()));

	backBuffer.Reset();

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.Format = m_DepthStencilBufferFormat;
	depthStencilDesc.SampleDesc.Count = M_MSAASampleCount;
	depthStencilDesc.SampleDesc.Quality = m_MSAAQuality - 1;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	DX::ThrowIfFailed(pDevice->CreateTexture2D(&depthStencilDesc, 0, pDSBuffer.ReleaseAndGetAddressOf()));


	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = CD3D11_DEPTH_STENCIL_VIEW_DESC(
		pDSBuffer.Get(),
		M_MSAASampleCount > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D,
		m_DepthStencilViewFormat);

	DX::ThrowIfFailed(pDevice->CreateDepthStencilView(pDSBuffer.Get(), &depthStencilViewDesc, pDSV.ReleaseAndGetAddressOf()));

	pContext->OMSetRenderTargets(1, pRTV.GetAddressOf(), pDSV.Get());

	m_ScreenViewport.TopLeftX = 0.0f;
	m_ScreenViewport.TopLeftY = 0.0f;
	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;
	m_ScreenViewport.Width = static_cast<float>(width);
	m_ScreenViewport.Height = static_cast<float>(height);

	pContext->RSSetViewports(1, &m_ScreenViewport);
}

void Graphics::Draw()
{

}

void Graphics::EndFrame()
{
	HRESULT hr;
	if (FAILED(hr = pSwap->Present(0, 0)))
	{
		//throw hr
	}

}

int Graphics::getWidth()
{
	return this->width;
}

int Graphics::getHeight()
{
	return this->height;
}

ID3D11DeviceContext * Graphics::GetContext()
{
	return pContext.Get();
}

ID3D11Device * Graphics::GetDevice()
{
	return pDevice.Get();
}

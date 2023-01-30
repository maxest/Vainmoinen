#ifdef RENDERER_D3D11

#include <renderer_d3d11/renderer_d3d11.hpp>
#include <common/logger.hpp>
#include <math/vector.hpp>
#include <math/matrix.hpp>



CRenderer Renderer;



CRenderer::CRenderer()
{
	D3D11SwapChain = NULL;
	D3D11Device = NULL;
	D3D11DeviceContext = NULL;

	D3D11BackBufferRTV = NULL;

	D3D11BackBufferDepthStencilTexture = NULL;
	D3D11BackBufferDSV = NULL;
}



void CRenderer::create(int screenWidth, int screenHeight, bool fullScreen, bool vSync)
{
	this->vSync = vSync;

    UINT createDeviceFlags = 0;
	#if defined(DEBUG) | defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	D3D_FEATURE_LEVEL featureLevels[1] = { D3D_FEATURE_LEVEL_11_0 };

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = GetActiveWindow();
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = !fullScreen;

	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevels, 1, D3D11_SDK_VERSION, &swapChainDesc, &D3D11SwapChain, &D3D11Device, NULL, &D3D11DeviceContext);

	// back-buffer render target view
	{
		ID3D11Texture2D* D3D11BackBufferRenderTargetTexture;

		D3D11SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&D3D11BackBufferRenderTargetTexture);

		D3D11Device->CreateRenderTargetView(D3D11BackBufferRenderTargetTexture, NULL, &D3D11BackBufferRTV);

		D3D11BackBufferRenderTargetTexture->Release();
	}

	// back-buffer depth-stencil view
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.Width = screenWidth;
		textureDesc.Height = screenHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		D3D11Device->CreateTexture2D(&textureDesc, NULL, &D3D11BackBufferDepthStencilTexture);

		//

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		depthStencilViewDesc.Format = textureDesc.Format;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		D3D11Device->CreateDepthStencilView(D3D11BackBufferDepthStencilTexture, &depthStencilViewDesc, &D3D11BackBufferDSV);
	}

    D3D11DeviceContext->OMSetRenderTargets(1, &D3D11BackBufferRTV, D3D11BackBufferDSV);
}



void CRenderer::destroy()
{
	if (D3D11BackBufferDSV != NULL)
	{
		D3D11BackBufferDSV->Release();
		D3D11BackBufferDSV = NULL;
	}

	if (D3D11BackBufferDepthStencilTexture != NULL)
	{
		D3D11BackBufferDepthStencilTexture->Release();
		D3D11BackBufferDepthStencilTexture = NULL;
	}

	if (D3D11BackBufferRTV != NULL)
	{
		D3D11BackBufferRTV->Release();
		D3D11BackBufferRTV = NULL;
	}

	if (D3D11DeviceContext != NULL)
	{
		D3D11DeviceContext->Release();
		D3D11DeviceContext = NULL;
	}

	if (D3D11Device != NULL)
	{
		D3D11Device->Release();
		D3D11Device = NULL;
	}

	if (D3D11SwapChain != NULL)
	{
		D3D11SwapChain->Release();
		D3D11SwapChain = NULL;
	}
}



void CRenderer::beginScene()
{
	// no begin-scene-like call in DX11?! No way!
}



void CRenderer::endScene()
{
	D3D11SwapChain->Present(vSync ? 1 : 0, 0);
}



void CRenderer::clear(bool target, bool depth, bool stencil, const vec3& targetColor)
{
	if (target)
	{
		float color[4] = { targetColor.x, targetColor.y, targetColor.z, 1.0f };
		D3D11DeviceContext->ClearRenderTargetView(Renderer.D3D11BackBufferRTV, color);
	}

	if (depth || stencil)
	{
		uint flags_DX = 0;

		if (depth)
			flags_DX |= D3D11_CLEAR_DEPTH;
		if (stencil)
			flags_DX |= D3D11_CLEAR_STENCIL;

		D3D11DeviceContext->ClearDepthStencilView(Renderer.D3D11BackBufferDSV, flags_DX, 1.0f, 0);
	}
}



void CRenderer::setViewport(int x, int y, int width, int height)
{
	uint i = 1;
    D3D11_VIEWPORT viewport;
	D3D11DeviceContext->RSGetViewports(&i, &viewport);

    viewport.Width = (float)width;
    viewport.Height = (float)height;
    viewport.TopLeftX = (float)x;
    viewport.TopLeftY = (float)y;

    D3D11DeviceContext->RSSetViewports(1, &viewport);
}



void CRenderer::setDepthRange(float minZ, float maxZ)
{
	uint i = 1;
    D3D11_VIEWPORT viewport;
	D3D11DeviceContext->RSGetViewports(&i, &viewport);

	viewport.MinDepth = minZ;
	viewport.MaxDepth = maxZ;

    D3D11DeviceContext->RSSetViewports(1, &viewport);
}



#endif

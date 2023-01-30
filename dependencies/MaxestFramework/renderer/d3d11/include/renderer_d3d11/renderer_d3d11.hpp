#ifndef RENDERER_RENDERER_D3D11_H
#define RENDERER_RENDERER_D3D11_H

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dCompiler.h>



// THIS IS ADDED TEMPORARILY

#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }

inline HRESULT CompileShaderFromFile( LPCSTR fileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut, D3D10_SHADER_MACRO* pDefines = NULL )
{
	HRESULT hr;

    DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DX11CompileFromFileA( fileName, pDefines, NULL, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL );

    if( FAILED(hr) )
    {
        if( pErrorBlob != NULL )
            OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
        SAFE_RELEASE( pErrorBlob );
        return hr;
    }
    SAFE_RELEASE( pErrorBlob );

    return S_OK;
}



class vec3;



class CRenderer
{
public:
	CRenderer();

	void create(int screenWidth, int screenHeight, bool fullScreen, bool vSync);
	void destroy();

	void beginScene();
	void endScene();

	void clear(bool target, bool depth, bool stencil, const vec3& targetColor);

	void setViewport(int x, int y, int width, int height); // (x, y) is an upper-left corner
	void setDepthRange(float minZ, float maxZ);

	ID3D11Device* getD3D11Device() const { return D3D11Device; }
	ID3D11DeviceContext* getD3D11DeviceContext() const { return D3D11DeviceContext; }

	ID3D11RenderTargetView* getD3D11BackBufferRTV() const { return D3D11BackBufferRTV; }

private:
	bool vSync;

	IDXGISwapChain* D3D11SwapChain;
	ID3D11Device* D3D11Device;
	ID3D11DeviceContext* D3D11DeviceContext;

	ID3D11RenderTargetView* D3D11BackBufferRTV;

	ID3D11Texture2D* D3D11BackBufferDepthStencilTexture;
	ID3D11DepthStencilView* D3D11BackBufferDSV;
};
extern CRenderer Renderer;



#endif

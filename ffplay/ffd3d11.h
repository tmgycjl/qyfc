#pragma once

#include <d3d11.h>



#include "d3dx9.h"

typedef struct
{
	D3D_DRIVER_TYPE driverType ;               
	D3D_FEATURE_LEVEL featureLevel ;                
	ID3D11Device *pd3dDevice ;                                  
	ID3D11DeviceContext *pImmediateContext ;                   
	IDXGISwapChain *pSwapChain ;                               
	ID3D11RenderTargetView *pRenderTargetView ;   
	ID3D11Texture2D *pBackBuffer;
	ID3D11DepthStencilView *m_pDepthStencilView;//DX深度模板缓存视图
	ID3D11Texture2D        *m_pDepthStencil;    //深度模板缓存
	HWND m_hWnd;
	DXGI_FORMAT m_Format;
	LPD3DXFONT m_p2Dfont;
	int _imageWidth;
	int pitch;
	int _imageHeight;
	BOOL _initD3D;
	unsigned char *pixel;
	RECT rcDisplay;
}FFD3D11;



#ifdef __cplusplus
extern "C"{
#endif
	int d3d11Create(FFD3D11 *d3d, HWND hWnd, int width, int height);

	int d3d11LockSurface(FFD3D11 *d3d, void **data, int *pitch);

	void d3d11UnlockSurface(FFD3D11 *d3d);

	void d3d11Render(FFD3D11 *d3d, int width, int height, HWND hWnd);

	void d3d11RenderInternal(FFD3D11 *d3d);


	void d3d11Release(FFD3D11 *d3d);

#ifdef __cplusplus
}
#endif


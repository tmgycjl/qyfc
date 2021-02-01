#pragma once

#include "d3dx9.h"

typedef struct
{
	LPDIRECT3D9             m_pD3D;
	LPDIRECT3DDEVICE9       m_pd3dDevice;
	LPDIRECT3DTEXTURE9      m_pTexturesvideo;
	LPDIRECT3DVERTEXBUFFER9 m_pVBvideo;		
	LPDIRECT3DTEXTURE9      m_pTexturesbmp;
	LPDIRECT3DVERTEXBUFFER9 m_pVBbmp;     
	IDirect3DSurface9 * m_pd3dSurface;//D3DªÊÕº”√±‰¡ø
	HWND m_hWnd;
	D3DFORMAT m_Format;
	LPD3DXFONT m_p2Dfont;
	int _imageWidth;
	int _imageHeight;
	int _initD3D;
	//RECT lastRcDisplay;
	//RECT lastRealRcDisplay;
	RECT m_rtViewport;
}FFD3D;



#ifdef __cplusplus
extern "C"{
#endif
	int d3dReCreate(FFD3D *d3d, HWND hWnd, int width, int height);

	int d3dLockSurface(FFD3D *d3d, void **data, int *pitch);

	void d3dUnlockSurface(FFD3D *d3d);

	void d3dRender(FFD3D *d3d, int width, int height, HWND hWnd);

	void d3dRenderInternal(FFD3D *d3d);
#if 0
	void ddrawRenderYUV(FFDDraw *ddraw, HWND hWnd, AVFrame *frame);
#endif

	void d3dRelease(FFD3D *d3d);

#ifdef __cplusplus
}
#endif


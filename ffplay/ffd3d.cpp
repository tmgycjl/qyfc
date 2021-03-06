#include "ffd3d.h"
#include "globaldef.h"

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#pragma comment(lib, "d3d9.lib") 


void d3dRelease(FFD3D *d3d)
{

	SAFE_RELEASE(d3d->m_pd3dSurface);
	SAFE_RELEASE(d3d->_backBufferSurface);
	SAFE_RELEASE(d3d->m_pd3dDevice);
	SAFE_RELEASE(d3d->m_pD3D);
	
	d3d->_initD3D = FALSE;
}

int d3dReCreate(FFD3D *d3d, HWND hWnd, int width, int height)
{
	if (d3d->_initD3D)
	{
		return 0;
	}

	d3dRelease(d3d);

	d3d->m_hWnd = hWnd;
	d3d->_imageWidth = width;
	d3d->_imageHeight = height;

	d3d->m_Format = D3DFMT_X8R8G8B8;

	d3d->m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d->m_pD3D == NULL)
		return -1;

	D3DDISPLAYMODE d3dDisplayMode;
	long  lRet = d3d->m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3dDisplayMode);

	RECT    rtClient;

	GetClientRect(d3d->m_hWnd, &rtClient);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = d3d->m_Format;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = rtClient.right;
	d3dpp.BackBufferHeight = rtClient.bottom;
	//d3dpp.BackBufferWidth = width*2;
	//d3dpp.BackBufferHeight = height*2;
	d3dpp.hDeviceWindow = d3d->m_hWnd;
	//double    dbAspect    = (double)lWidth / lHeight;  
	
	d3d->m_rtViewport = rtClient;
	d3d->m_rtViewport.right = d3dpp.BackBufferWidth;
	d3d->m_rtViewport.bottom = d3dpp.BackBufferHeight;

	if (FAILED(d3d->m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, nullptr,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE, &d3dpp, &d3d->m_pd3dDevice)))
	{
		return -2;
	}

	if (FAILED(d3d->m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &d3d->_backBufferSurface)))
	{
		return -4;
	}

	if (FAILED(d3d->m_pd3dDevice->CreateOffscreenPlainSurface(width, height, d3d->m_Format, D3DPOOL_DEFAULT, &d3d->m_pd3dSurface, NULL)))
	{
		return -3;
	}

	d3d->_initD3D = TRUE;

	//if( FAILED(m_pd3dDevice->CreateOffscreenPlainSurface( lWidth,lHeight, (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),  D3DPOOL_DEFAULT,  &m_pd3dSurface,  NULL)))      
	//return;

	return 0;
}

int d3dLockSurface(FFD3D *d3d, void **data, int *pitch)
{
	if (!d3d->_initD3D)
	{
		return -1;
	}

	D3DLOCKED_RECT d3d_rect;
	if (FAILED(d3d->m_pd3dSurface->LockRect(&d3d_rect, NULL, D3DLOCK_NOSYSLOCK)))
		return -1;


	BYTE * const p = (BYTE *)d3d_rect.pBits;
	const int stride = d3d_rect.Pitch;

	data[0] = p;
	data[1] = p + d3d->_imageHeight * stride;
	data[2] = p + 2 * d3d->_imageHeight  * stride;
	data[3] = p + 3 * d3d->_imageHeight  * stride;

	pitch[0] = stride;


	return 0;
}


void d3dUnlockSurface(FFD3D *d3d)
{
	if (FAILED(d3d->m_pd3dSurface->UnlockRect()))
	{
		return;
	}

}

void d3dRenderInternal(FFD3D *d3d)
{
	return d3dRender(d3d, d3d->_imageWidth, d3d->_imageHeight, d3d->m_hWnd);
}

void d3dRender(FFD3D *d3d, int width, int height, HWND hWnd)
{
	RECT rcDisplay;
	GetClientRect(d3d->m_hWnd, &rcDisplay);

#if 1


	if (d3d->m_rtViewport.right - d3d->m_rtViewport.left != rcDisplay.right - rcDisplay.left
		|| d3d->m_rtViewport.bottom - d3d->m_rtViewport.top != rcDisplay.bottom - rcDisplay.top)
	{
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.hDeviceWindow = d3d->m_hWnd;
		d3dpp.BackBufferCount = 1;
		//if (FAILED(d3d->m_pd3dDevice->Reset(&d3dpp)))
		{
			d3d->_initD3D = 0;
			d3dReCreate(d3d, hWnd, width, height);
		}
		
		//

		if (!d3d->_initD3D)
		{
			return;
		}

	}
#endif

	if (d3d->m_pd3dDevice != NULL)
	{
		d3d->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		d3d->m_pd3dDevice->BeginScene();

		if (NULL != d3d->_backBufferSurface)
		{
			d3d->m_pd3dDevice->StretchRect(d3d->m_pd3dSurface, NULL, d3d->_backBufferSurface, &d3d->m_rtViewport, D3DTEXF_LINEAR);
		}
		
		d3d->m_pd3dDevice->EndScene();
		d3d->m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
	}
}

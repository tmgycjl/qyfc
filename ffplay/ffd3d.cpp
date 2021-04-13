#include "ffd3d.h"
#include "globaldef.h"

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#pragma comment(lib, "d3d9.lib") 


#if 0


typedef struct
{
	FLOAT       x, y, z;
	FLOAT       rhw;
	D3DCOLOR    diffuse;
	FLOAT       tu, tv;
} CUSTOMVERTEX;

void calculateD3DDisplayRect(RECT *rect, int width, int height)
{
	float rcW = rect->right - rect->left;
	float rcH = rect->bottom - rect->top;
	float ratioImage = (float)width / (float)height;
	float ratioWnd = (float)rcW / (float)rcH;
	float off = 0;
	float realXY = 0;
	if (ratioImage > ratioWnd)
	{
		realXY = rcW / ratioImage;

		off = (rcH - realXY) / 2;
		rect->top += off;
		rect->bottom = rect->top + realXY;
	}
	else
	{
		realXY = rcH * ratioImage;

		off = (rcW - realXY) / 2;
		rect->left += off;
		rect->right = rect->left + realXY;
	}
}


int d3dReCreate(FFD3D *d3d, HWND hWnd, int width, int height)
{
	if (d3d->_initD3D)
	{
		if (d3d->_imageHeight == height && d3d->_imageWidth == width)
		{
			return 0;
		}

		
	}

	d3dRelease(d3d);

	D3DPRESENT_PARAMETERS d3dpp;
	if (hWnd == NULL)
		return -1;
	d3d->m_hWnd = hWnd;

	d3d->m_Format = D3DFMT_A8R8G8B8;

	if (NULL == (d3d->m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return -2;


	if (0 == width || 0 == height)
	{
		GetClientRect(d3d->m_hWnd, &d3d->lastRealRcDisplay);

		 width = d3d->lastRealRcDisplay.right - d3d->lastRealRcDisplay.left;
		 height = d3d->lastRealRcDisplay.bottom - d3d->lastRealRcDisplay.top;
	}
	
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	//d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
	d3dpp.hDeviceWindow = hWnd;
	d3dpp.Windowed = TRUE;
	d3dpp.EnableAutoDepthStencil = FALSE;
	d3dpp.Flags = D3DPRESENTFLAG_VIDEO;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	d3d->_imageWidth = width;
	d3d->_imageHeight  = height;

	if (FAILED(d3d->m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		d3d->m_hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&d3dpp,
		&d3d->m_pd3dDevice)))
	{
		return -3;
	}

	if (FAILED(d3d->m_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_SOFTWAREPROCESSING,
		D3DFMT_X8R8G8B8,
		D3DPOOL_MANAGED,
		&d3d->m_pTexturesvideo, NULL)))
	{
		return -4;
	}


	// Create Vertex Buffer
	if(FAILED(d3d->m_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &d3d->m_pVBvideo, NULL)))
	{
		return -5;
	}

#if TEXTURE_HALF
	CUSTOMVERTEX vertices[] ={
		{0.0f,		0.0f,		0.0f,	1.0f,D3DCOLOR_ARGB(255, 255, 255, 255),0.0f,0.0f},
		{ lWidth, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.5f, 0.0f },
		{ lWidth, lHeight, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.5f, 1.0f },
		{ 0.0f, lHeight, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0f, 1.0f }
};
#elif TEXTURE_ROTATE
	//Rotate Texture?
	CUSTOMVERTEX vertices[] = {
		{ lWidth / 4, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0f, 0.0f },
		{ lWidth, lHeight / 4, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 0.0f },
		{ lWidth * 3 / 4, lHeight, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 1.0f },
		{ 0.0f, lHeight * 3 / 4, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0f, 1.0f }
	};
#else
	CUSTOMVERTEX vertices[] = {
		{ 0.0f, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0f, 0.0f },
		{ width, 0.0f, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 0.0f },
		{ width, height, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 1.0f, 1.0f },
		{ 0.0f, height, 0.0f, 1.0f, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0f, 1.0f }
	};
#endif


	// Fill Vertex Buffer
	CUSTOMVERTEX *pVertex;
	if(FAILED(d3d->m_pVBvideo->Lock(0, 4 * sizeof(CUSTOMVERTEX), (void**)&pVertex, 0)))
	{
		return -6;
	}
	memcpy(pVertex, vertices, sizeof(vertices));

	d3d->m_pVBvideo->Unlock();

	d3d->_initD3D = 1;

	return 0;
}


void d3dRelease(FFD3D *d3d)
{
	SAFE_RELEASE(d3d->m_pd3dDevice);

	SAFE_RELEASE(d3d->m_pd3dDevice);
	SAFE_RELEASE(d3d->m_pD3D);
	SAFE_RELEASE(d3d->m_p2Dfont);
	SAFE_RELEASE(d3d->m_pVBvideo);
	SAFE_RELEASE(d3d->m_pTexturesvideo);
	SAFE_RELEASE(d3d->m_pVBbmp);
	SAFE_RELEASE(d3d->m_pTexturesbmp);

	d3d->m_hWnd = NULL;
	d3d->m_Format = (D3DFORMAT)0;

	d3d->_initD3D = 0;
}


int d3dLockSurface(FFD3D *d3d, void **data, int *pitch)
{
	if (!d3d->_initD3D)
	{
		return -1;
	}

	HRESULT hr = 0;
	BYTE * pTextureBuffer = NULL;
	D3DLOCKED_RECT d3dlr;
	LONG  lTexturePitch;
	hr = d3d->m_pTexturesvideo->LockRect(0, &d3dlr, 0, 0);
	if (FAILED(hr))
	{
		return -1;
	}

	pTextureBuffer = static_cast<byte *>(d3dlr.pBits);

	data[0] = pTextureBuffer;
	data[1] = pTextureBuffer + d3d->_imageHeight * d3dlr.Pitch;
	data[2] = pTextureBuffer + 2 * d3d->_imageHeight  * d3dlr.Pitch;
	data[3] = pTextureBuffer + 3 * d3d->_imageHeight  * d3dlr.Pitch;

	pitch[0] = d3dlr.Pitch;

	return 0;
}


void d3dUnlockSurface(FFD3D *d3d)
{
	HRESULT hr = d3d->m_pTexturesvideo->UnlockRect(0);
}

void d3dRenderInternal(FFD3D *d3d)
{

	GetClientRect(d3d->m_hWnd, &d3d->lastRealRcDisplay);


#if 1


	if (d3d->lastRcDisplay.right - d3d->lastRcDisplay.left != d3d->lastRealRcDisplay.right - d3d->lastRealRcDisplay.left
		|| d3d->lastRcDisplay.bottom - d3d->lastRcDisplay.top != d3d->lastRealRcDisplay.bottom - d3d->lastRealRcDisplay.top)
	{
		d3d->lastRcDisplay.left = d3d->lastRealRcDisplay.left;
		d3d->lastRcDisplay.top = d3d->lastRealRcDisplay.top;
		d3d->lastRcDisplay.right = d3d->lastRealRcDisplay.right;
		d3d->lastRcDisplay.bottom = d3d->lastRealRcDisplay.bottom;

		d3d->_initD3D = 0;
		
		if (d3dReCreate(d3d, d3d->m_hWnd, d3d->_imageWidth, d3d->_imageHeight))
		{
			return ;
		}
	}
	else
	{
		d3d->lastRcDisplay.left = d3d->lastRealRcDisplay.left;
		d3d->lastRcDisplay.top = d3d->lastRealRcDisplay.top;
		d3d->lastRcDisplay.right = d3d->lastRealRcDisplay.right;
		d3d->lastRcDisplay.bottom = d3d->lastRealRcDisplay.bottom;
	}
#endif
	HRESULT hr = 0;
#if 1


	
	hr = d3d->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET,D3DCOLOR_XRGB(0, 255, 0), 1.0f, 0);

#endif
	hr = d3d->m_pd3dDevice->BeginScene();
	if (FAILED(hr))
	{
		return;
	}



	DWORD lRet = d3d->m_pd3dDevice->SetTexture(0, d3d->m_pTexturesvideo);

	//Binds a vertex buffer to a device data stream.
	d3d->m_pd3dDevice->SetStreamSource(0, d3d->m_pVBvideo,0, sizeof(CUSTOMVERTEX));
	//Sets the current vertex stream declaration.
	lRet = d3d->m_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	//Renders a sequence of nonindexed, geometric primitives of the 
	//specified type from the current set of data input streams.
	d3d->m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);
	d3d->m_pd3dDevice->EndScene();
	d3d->m_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

void d3dRender(FFD3D *d3d, int width, int height, HWND hWnd)
{
	if (!d3d->_initD3D)
	{
		return;
	}

	if (d3d->_imageHeight == height && d3d->_imageWidth == width)
	{
		if (d3dReCreate(d3d, hWnd, width, height))
		{
			return;
		}
	}

	d3dRenderInternal(d3d);

	
}

#else


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
#endif
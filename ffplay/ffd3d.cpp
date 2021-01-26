#include "ffd3d.h"
#include "globaldef.h"

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

#pragma comment(lib, "d3d9.lib") 


struct CUSTOMVERTEX
{
	float x, y, z, rhw;	
	DWORD color;		
	float tu, tv;		

};


int createTexture(FFD3D *d3d)//创建纹理
{
	HRESULT hr = 0;
	D3DSURFACE_DESC ddsd;
	hr = d3d->m_pd3dDevice->CreateTexture(d3d->_imageWidth, d3d->_imageHeight, 1, 0, d3d->m_Format,
		D3DPOOL_MANAGED, &d3d->m_pTexturesvideo, NULL);
	if (FAILED(hr))
	{
		return -1;
	}
	hr = d3d->m_pTexturesvideo->GetLevelDesc(0, &ddsd);//获得加载图片的宽和高及些信息
	if (FAILED(hr))
		return -2;
	if ((ddsd.Format != D3DFMT_A8R8G8B8) && (ddsd.Format != D3DFMT_YUY2))
		return -3;

	return 0;
}

int initGeometry(FFD3D *d3d)
{
	HRESULT hr = 0;
	CUSTOMVERTEX* pVertices = NULL;
	RECT rect;

	rect.top = 0;
	rect.left = 0;
	rect.right = d3d->_imageWidth;
	rect.bottom = d3d->_imageHeight;
	CUSTOMVERTEX vertices[4] =
	{
		{ (float)rect.left, (float)rect.top,
		0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 0.0f },
		{ (float)rect.right, (float)rect.top,
		0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0.0f },
		{ (float)rect.left, (float)rect.bottom,
		0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 0.0f, 1.0f },
		{ (float)rect.right, (float)rect.bottom,
		0.0f, 1.0f, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 1.0f }
	};
	d3d->m_pd3dDevice->CreateVertexBuffer(sizeof(vertices), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &d3d->m_pVBvideo, NULL);
	d3d->m_pVBvideo->Lock(0, sizeof(vertices), (void**)&pVertices, 0);
	memcpy(pVertices, vertices, sizeof(vertices));
	d3d->m_pVBvideo->Unlock();
	return 0;
}



int d3dCreate(FFD3D *d3d, HWND hWnd, int width, int height)
{
	if (d3d->_initD3D)
	{
		if (d3d->_imageHeight == height && d3d->_imageWidth == width)
		{
			return 0;
		}

		d3dRelease(d3d);
	}

	D3DPRESENT_PARAMETERS d3dpp;
	if (hWnd == NULL)
		return -1;
	d3d->m_hWnd = hWnd;

	d3d->m_Format = D3DFMT_A8R8G8B8;

	if (NULL == (d3d->m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return -2;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	//d3dpp.MultiSampleType = D3DMULTISAMPLE_2_SAMPLES;

	d3d->_imageWidth = d3dpp.BackBufferWidth = width;
	d3d->_imageHeight = d3dpp.BackBufferHeight = height;

	if (FAILED(d3d->m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		d3d->m_hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
		&d3dpp,
		&d3d->m_pd3dDevice)))
		return -3;

	//HRESULT hr = d3d->m_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);

	if (0 != createTexture(d3d))
	{
		return -4;
	}

	if (0 != initGeometry(d3d))
	{
		return -5;
	}

	d3d->_initD3D = TRUE;

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

	d3d->_initD3D = FALSE;
}


int d3dLockSurface(FFD3D *d3d, void **data, int *pitch)
{
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
	HRESULT hr = 0;
	hr = d3d->m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	if (FAILED(hr))
	{
		return;
	}
	hr = d3d->m_pd3dDevice->BeginScene();
	if (FAILED(hr))
	{
		return;
	}

	hr = d3d->m_pd3dDevice->SetTexture(0, d3d->m_pTexturesvideo);
	d3d->m_pd3dDevice->SetStreamSource(0, d3d->m_pVBvideo, 0, sizeof(CUSTOMVERTEX));
	hr = d3d->m_pd3dDevice->SetVertexShader(NULL);
	d3d->m_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	d3d->m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	//d3d->m_pd3dDevice->StretchRect(m_pd3dSurface, NULL, pBackBuffer, &m_rtViewport, D3DTEXF_LINEAR);
	d3d->m_pd3dDevice->SetTexture(0, NULL);
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
		if (d3dCreate(d3d, hWnd, width, height))
		{
			return;
		}
	}

	d3dRenderInternal(d3d);

	
}


#include "ffddraw.h"
#include "globaldef.h"



#pragma comment(lib, "ddraw.lib") 
#pragma comment(lib, "dxguid.lib")

void ddrawRelease(FFDDraw *ddraw,BOOL bRecreateDispBuffer)
{
	SAFE_RELEASE(ddraw->m_pddColorCtrl);
	if (bRecreateDispBuffer)
		SAFE_RELEASE(ddraw->m_pddsDispBuffer);
	SAFE_RELEASE(ddraw->m_pddsBackBuffer);
	SAFE_RELEASE(ddraw->m_pddsFrontBuffer);
	SAFE_RELEASE(ddraw->m_pDD);	

	ddraw->m_bInitDD = FALSE;
#if 0


	if (NULL != ddraw->img_convert_ctx)
	{
		sws_freeContext(ddraw->img_convert_ctx);
	}
#endif
}


BOOL ddrawCreate(FFDDraw *ddraw,HWND hWnd, int width, int height, BOOL bRecreateDispBuffer)
{
	if (ddraw->m_bInitDD)
	{
		if (ddraw->m_nImageWidth == width && ddraw->m_nImageHeight == height)
		{
			return TRUE;
		}
		
		ddrawRelease(ddraw, TRUE);
	}


	if(NULL == hWnd) 
		return FALSE;
	if(width <= 0 || height <= 0)
		return FALSE;
	ddraw->m_hWnd = hWnd;
	ddraw->m_nImageWidth = width;
	ddraw->m_nImageHeight = height;

	ddraw->m_rcDispBuffer.left = ddraw->m_rcDispBuffer.top = 0;
	ddraw->m_rcDispBuffer.right = ddraw->m_nImageWidth;
	ddraw->m_rcDispBuffer.bottom = ddraw->m_nImageHeight;


	RECT rect;
	if( !GetWindowRect(ddraw->m_hWnd, &rect) )
	{
		return FALSE;
	}
	ddraw->m_rcBackBuffer.left = ddraw->m_rcBackBuffer.top = 0;
	ddraw->m_rcBackBuffer.right = rect.right - rect.left;
	ddraw->m_rcBackBuffer.bottom = rect.bottom - rect.top;
	
	HRESULT hr;
	DDCAPS ddcap;
	BOOL bBltStretchX = FALSE;
	BOOL bBltFourCC = FALSE;

	hr = DirectDrawCreateEx(NULL, (VOID**)&ddraw->m_pDD, IID_IDirectDraw7, NULL);
	if( FAILED(hr) )
	{
		//log("44444\n");
		hr = DirectDrawCreateEx(NULL, (VOID**)&ddraw->m_pDD, IID_IDirectDraw4, NULL);
		if( FAILED(hr) )
		{
			return FALSE;
		}
	}
	//log("55555\n");
	hr = ddraw->m_pDD->SetCooperativeLevel(ddraw->m_hWnd, DDSCL_NORMAL);
	if(FAILED(hr))
		return FALSE;
	
	ZeroMemory(&ddcap, sizeof(ddcap));
	ddcap.dwSize = sizeof(ddcap);
	hr = ddraw->m_pDD->GetCaps(&ddcap, NULL);
	if(FAILED(hr))
		return FALSE;
	//log("66666\n");
	if((ddcap.dwFXCaps & DDFXCAPS_BLTSTRETCHX) && (ddcap.dwFXCaps & DDFXCAPS_BLTSTRETCHY))
	{
		//caps = 100;
		bBltStretchX = TRUE;
	}
	if(ddcap.dwCaps & DDCAPS_BLTFOURCC)
		bBltFourCC = TRUE;
	
	// Create the primary surface
	DDSURFACEDESC2 ddsd;
	ZeroMemory( &ddsd, sizeof( ddsd ) );
	ddsd.dwSize         = sizeof( ddsd );
	ddsd.dwFlags        = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
 
	if (FAILED(ddraw->m_pDD->CreateSurface(&ddsd, &ddraw->m_pddsFrontBuffer, NULL)))
		return FALSE;
	
	LPDIRECTDRAWCLIPPER pcClipper;
	if (FAILED(hr = ddraw->m_pDD->CreateClipper(0, &pcClipper, NULL)))
		return FALSE;
	if( FAILED( hr = pcClipper->SetHWnd( 0, hWnd ) ) )
	{
		pcClipper->Release();
		return FALSE;
	}
	if (FAILED(hr = ddraw->m_pddsFrontBuffer->SetClipper(pcClipper)))
	{
		pcClipper->Release();
		return FALSE;
	}
	pcClipper->Release();

//获取主表面的象素格式,显卡支持YUV显示
	DDPIXELFORMAT ddpf;
	ZeroMemory(&ddpf, sizeof(ddpf));
	ddpf.dwSize = sizeof(DDPIXELFORMAT);
	ddpf.dwFlags = DDPF_RGB;
	ddraw->m_pddsFrontBuffer->GetPixelFormat(&ddpf);
	if(ddpf.dwRGBBitCount == 32)
	{
		ddraw->m_frontBufFormat = FORMAT_RGB32;
	}
	else if(ddpf.dwRGBBitCount == 24)
	{
		ddraw->m_frontBufFormat = FORMAT_RGB24;
	}
	else if(ddpf.dwRGBBitCount == 16)
	{
		if(ddpf.dwRBitMask == 0xF800)
			ddraw->m_frontBufFormat = FORMAT_RGB565;
		else if(ddpf.dwRBitMask == 0x7C00)
			ddraw->m_frontBufFormat = FORMAT_RGB555;
		else
			ddraw->m_frontBufFormat = FORMAT_NONE;
	}		

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT ;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth =ddraw->m_rcBackBuffer.right;
	//ddsd.dwWidth = 5120;
	ddsd.dwHeight =  ddraw->m_rcBackBuffer.bottom;
	hr = ddraw->m_pDD->CreateSurface(&ddsd, &ddraw->m_pddsBackBuffer, NULL);
	if(hr != DD_OK)
	{
		return FALSE;
	}

	DDSCAPS2 capBackBuffer;
	hr = ddraw->m_pddsBackBuffer->GetCaps(&capBackBuffer);
	if(hr != DD_OK)
	{
		return FALSE;
	}

	if(!bRecreateDispBuffer)
		//return TRUE;

	ZeroMemory(&ddpf, sizeof(DDPIXELFORMAT));
	ddpf.dwSize = sizeof(DDPIXELFORMAT);
	ddpf.dwFlags = DDPF_FOURCC;

	ddpf.dwFourCC = MAKEFOURCC('Y', 'V', '1', '2');//使用YV12表面

#if 0
	ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;     
	if (bBltStretchX)
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
	else
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth = ddraw->m_nImageWidth;
	ddsd.dwHeight = ddraw->m_nImageHeight;

	if (SUCCEEDED(hr = ddraw->m_pDD->CreateSurface(&ddsd, &ddraw->m_pddsDispBuffer, NULL)))
	{
		ZeroMemory(&ddpf, sizeof(ddpf));
		ddpf.dwSize = sizeof(DDPIXELFORMAT);
		ddpf.dwFlags = DDPF_RGB;
		ddraw->m_pddsDispBuffer->GetPixelFormat(&ddpf);
		if (ddpf.dwRGBBitCount == 32)
		{
			ddraw->m_dispBufFormat = FORMAT_RGB32;
		}
		else if (ddpf.dwRGBBitCount == 24)
		{
			ddraw->m_dispBufFormat = FORMAT_RGB24;
		}
		else if (ddpf.dwRGBBitCount == 16)
		{
			if (ddpf.dwRBitMask == 0xF800)
				ddraw->m_dispBufFormat = FORMAT_RGB565;
			else if (ddpf.dwRBitMask == 0x7C00)
				ddraw->m_dispBufFormat = FORMAT_RGB555;
			else
				ddraw->m_dispBufFormat = FORMAT_NONE;
		}

		ddraw->m_bInitDD = TRUE;
		return TRUE;
	}

#endif
//	RGB表面
	
	// 试YUV表面		
	if(bBltFourCC && (capBackBuffer.dwCaps & DDSCAPS_VIDEOMEMORY))
	{
		ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;     
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwWidth        = ddraw->m_nImageWidth;
		ddsd.dwHeight       = ddraw->m_nImageHeight;
		ddsd.ddpfPixelFormat = ddpf;

		if(SUCCEEDED(hr = ddraw->m_pDD->CreateSurface(&ddsd, &ddraw->m_pddsDispBuffer, NULL)))
		{
			ddraw->m_dispBufFormat = FORMAT_YV12;
			ddraw->m_bInitDD = TRUE;
			return TRUE;
		}
	}

	return FALSE;
}


void restoreAllSurfaces(FFDDraw *ddraw)
{
	
	HRESULT hr;

	if ((hr = ddraw->m_pddsFrontBuffer->Restore()) != DD_OK)
	{
		if (hr == DDERR_WRONGMODE)
		{
			ddrawCreate(ddraw,ddraw->m_hWnd, ddraw->m_nImageWidth, ddraw->m_nImageHeight, FALSE);
			return;
		}
	}

	ddraw->m_pddsBackBuffer->Restore();
	ddraw->m_pddsDispBuffer->Restore();
}



BOOL updateScreen(FFDDraw *ddraw)
{
	HRESULT hr;
	RECT rcDisplay;
	GetWindowRect(ddraw->m_hWnd, &rcDisplay);

	if (ddraw->m_pddsBackBuffer == NULL)
	{
		return FALSE;
	}
#if 0


	int rcW = rcDisplay.right - rcDisplay.left;
	int rcH = rcDisplay.bottom - rcDisplay.top;
	float ratioImage = (float)ddraw->m_nImageWidth / (float)ddraw->m_nImageHeight;
	float ratioWnd = (float)rcW / (float)rcH;
	int off = 0;
	if (ratioImage  > ratioWnd)
	{
		 off = (rcH - (rcW / ratioImage)) / 2;
		rcDisplay.top += off;
		rcDisplay.bottom -= off;
	}
	else
	{
		off = (rcW - rcH * ratioImage) / 2;
		rcDisplay.left += off;
		rcDisplay.right -= off;
	}
#endif
	hr = ddraw->m_pddsBackBuffer->Blt(&ddraw->m_rcBackBuffer, ddraw->m_pddsDispBuffer, &ddraw->m_rcDispBuffer, DDBLT_WAIT, NULL);

	if (hr == DDERR_SURFACELOST)
	{
		restoreAllSurfaces(ddraw); 
		return FALSE;
	}

	if (hr != DD_OK)
	{
		return FALSE;
	}

	hr = ddraw->m_pddsFrontBuffer->Blt(&rcDisplay, ddraw->m_pddsBackBuffer, &ddraw->m_rcBackBuffer, DDBLT_WAIT, NULL);


	if (hr == DDERR_SURFACELOST)
	{
		restoreAllSurfaces(ddraw);
		return FALSE;
	}

	return TRUE;
}


int ddrawLockSurface(FFDDraw *ddraw, void **data, int *pitch)
{
	if (!ddraw->m_bInitDD)
	{
		 return -1;
	}


	HRESULT hr;
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_PITCH | DDSD_LPSURFACE;

	if (NULL == ddraw->m_pddsDispBuffer)
	{
		return -1;
	}

	hr = ddraw->m_pddsDispBuffer->Lock(NULL, &ddsd,
		DDLOCK_WRITEONLY | DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR | DDLOCK_NOSYSLOCK, NULL);

	if (FAILED(hr))
	{
		if (hr == DDERR_SURFACELOST)
			restoreAllSurfaces(ddraw);
		return -1;
	}

	data[0] = ddsd.lpSurface;
	data[1] = (char*)ddsd.lpSurface + 5*ddsd.dwHeight * ddsd.lPitch/4;
	data[2] = (char*)ddsd.lpSurface +  ddsd.dwHeight * ddsd.lPitch ;
	//data[3] = (char*)ddsd.lpSurface + 3 * ddsd.dwHeight * ddsd.lPitch ;

	pitch[0] = ddsd.lPitch;

	return 0;
}


void ddrawUnlockSurface(FFDDraw *ddraw)
{
	if (NULL != ddraw->m_pddsDispBuffer)
	{
		ddraw->m_pddsDispBuffer->Unlock(NULL);
	}
	
}

void ddrawRender(FFDDraw *ddraw, int width, int height, HWND hWnd)
{
	if (hWnd == NULL || !IsWindowVisible(hWnd))
	{
		return;
	}

	if (!ddraw->m_bInitDD)
	{
		ddraw->m_bInitDD = ddrawCreate(ddraw, hWnd, width, height, FALSE);
		if (!ddraw->m_bInitDD)
		{
			return;
		}
	}

	if (ddraw->m_nImageWidth != width || ddraw->m_nImageHeight != height)
	{
		ddraw->m_bInitDD = FALSE;
		ddraw->m_bInitDD = ddrawCreate(ddraw, hWnd, width, height, FALSE);
		if (!ddraw->m_bInitDD)
		{
			return;
		}
	}


	updateScreen(ddraw);
}


#if 0


void ddrawRenderYUV(FFDDraw *ddraw, HWND hWnd, AVFrame *frame)
{

	if (hWnd == NULL || !IsWindowVisible(hWnd))
	{
		return;
	}

	
	if (!ddraw->m_bInitDD)
	{
		ddraw->m_bInitDD = ddrawCreate(ddraw, hWnd, frame->width, frame->height, FALSE);
		if (!ddraw->m_bInitDD)
		{
			return;
		}
	}

	if (ddraw->m_nImageWidth != frame->width || ddraw->m_nImageHeight != frame->height)
	{
		ddraw->m_bInitDD = ddrawCreate(ddraw, hWnd, frame->width, frame->height, FALSE);
		if (!ddraw->m_bInitDD)
		{
			return;
		}
	}

	HRESULT hr;
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_PITCH | DDSD_LPSURFACE;

	if (NULL == ddraw->m_pddsDispBuffer)
	{
		ddraw->m_bInitDD = ddrawCreate(ddraw, hWnd, frame->width, frame->height, FALSE);
		if (!ddraw->m_bInitDD)
		{
			return;
		}
	}

	hr = ddraw->m_pddsDispBuffer->Lock(NULL, &ddsd,
		DDLOCK_WRITEONLY | DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR | DDLOCK_NOSYSLOCK, NULL);

	if (FAILED(hr))
	{
		if (hr == DDERR_SURFACELOST)
			restoreAllSurfaces(ddraw);
		return;
	}

	switch (ddraw->m_dispBufFormat)
	{
	case FORMAT_YV12:
		break;
	case FORMAT_RGB32:
	{
		ddraw->img_convert_ctx = sws_getCachedContext(ddraw->img_convert_ctx,
			frame->width, frame->height, (AVPixelFormat)frame->format, frame->width, frame->height,
			AV_PIX_FMT_BGRA, 0, NULL, NULL, NULL);
		if (ddraw->img_convert_ctx != NULL) 
		{
			uint8_t *pixels[4];
			int pitch[4] = {0};
			pitch[0] = ddsd.lPitch;

			sws_scale(ddraw->img_convert_ctx, (const uint8_t * const *)frame->data, frame->linesize, 0, frame->height, (uint8_t* const*)ddsd.lpSurface, pitch);
		}
		else
		{
			return;
		}
	}
		break;
	case FORMAT_RGB24:
		break;
	case FORMAT_RGB565:
		break;
	case FORMAT_RGB555:
		break;
	}

	ddraw->m_pddsDispBuffer->Unlock(NULL);


	updateScreen(ddraw);
}
#endif
#include "ffddraw.h"

void ddrawRelease(FFDDraw *ddraw,BOOL bRecreateDispBuffer)
{
	
}


BOOL ddrawCreate(FFDDraw *ddraw,HWND hWnd, int width, int height, BOOL bRecreateDispBuffer)
{
	ddrawRelease(ddraw,bRecreateDispBuffer);

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
 
	if( FAILED( m_pDD->CreateSurface( &ddsd, &ddraw->m_pddsFrontBuffer, NULL ) ) )
		return FALSE;
	
	LPDIRECTDRAWCLIPPER pcClipper;
	if( FAILED( hr = m_pDD->CreateClipper( 0, &pcClipper, NULL ) ) )
		return FALSE;
	if( FAILED( hr = pcClipper->SetHWnd( 0, hWnd ) ) )
	{
		pcClipper->Release();
		return FALSE;
	}
	if( FAILED( hr = m_pddsFrontBuffer->SetClipper( pcClipper ) ) )
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
		return TRUE;

	ZeroMemory(&ddpf, sizeof(DDPIXELFORMAT));
	ddpf.dwSize = sizeof(DDPIXELFORMAT);
	ddpf.dwFlags = DDPF_FOURCC;

	ddpf.dwFourCC = MAKEFOURCC('Y', 'V', '1', '2');//使用YV12表面

//	RGB表面
	ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;     
	if(bBltStretchX)
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
	else
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth        = ddraw->m_nImageWidth;
	ddsd.dwHeight       = ddraw->m_nImageHeight;

	if(SUCCEEDED(hr = m_pDD->CreateSurface(&ddsd, &ddraw->m_pddsDispBuffer, NULL)))
	{
		ZeroMemory(&ddpf, sizeof(ddpf));
		ddpf.dwSize = sizeof(DDPIXELFORMAT);
		ddpf.dwFlags = DDPF_RGB;
		ddraw->m_pddsDispBuffer->GetPixelFormat(&ddpf);
		if(ddpf.dwRGBBitCount == 32)
		{
			ddraw->m_dispBufFormat = FORMAT_RGB32;
		}
		else if(ddpf.dwRGBBitCount == 24)
		{
			ddraw->m_dispBufFormat = FORMAT_RGB24;
		}
		else if(ddpf.dwRGBBitCount == 16)
		{
			if(ddpf.dwRBitMask == 0xF800)
				ddraw->m_dispBufFormat = FORMAT_RGB565;
			else if(ddpf.dwRBitMask == 0x7C00)
				ddraw->m_dispBufFormat = FORMAT_RGB555;
			else
				ddraw->m_dispBufFormat = FORMAT_NONE;
		}

		return TRUE;
	}

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
			return TRUE;
		}
	}

	return FALSE;
}
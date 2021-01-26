#pragma  once

#include "ddraw.h"


 typedef enum {
		FORMAT_NONE	  = 0,
		FORMAT_YV12   = 1,
		FORMAT_RGB32  = 2,
		FORMAT_RGB24  = 3,
		FORMAT_RGB565 = 4,
		FORMAT_RGB555 = 5,
	}DISP_BUFFER_FORMAT;


typedef struct 
{
    LPDIRECTDRAW7			m_pDD;
	LPDIRECTDRAWSURFACE7		m_pddsFrontBuffer;
	LPDIRECTDRAWSURFACE7		m_pddsBackBuffer;
	LPDIRECTDRAWSURFACE7		m_pddsDispBuffer;
	LPDIRECTDRAWCOLORCONTROL	m_pddColorCtrl;
	RECT					m_rcBackBuffer;
	RECT					m_rcDispBuffer;
	BOOL					m_bInitDD;                    
	HWND					m_hWnd;
	BOOL					m_bTryOverlay;
	BOOL					m_bUseOverlay;
	DWORD					m_dwOverlayColorKey;
	DWORD					m_dwRealOverlayColorKey;
	DISP_BUFFER_FORMAT		m_dispBufFormat;
	DISP_BUFFER_FORMAT		m_frontBufFormat;
    int					m_nImageWidth;
	int					m_nImageHeight;	
	struct SwsContext *img_convert_ctx;

}FFDDraw;


#ifdef __cplusplus
extern "C"{
#endif
	BOOL ddrawCreate(FFDDraw *ddraw, HWND hWnd, int width, int height, BOOL bRecreateDispBuffer);
	
	int ddrawLockSurface(FFDDraw *ddraw, void **data, int *pitch);

	void ddrawUnlockSurface(FFDDraw *ddraw);

	void ddrawRender(FFDDraw *ddraw, int width, int height,HWND hWnd);

#if 0
	void ddrawRenderYUV(FFDDraw *ddraw, HWND hWnd, AVFrame *frame);
#endif

	void ddrawRelease(FFDDraw *ddraw, BOOL bRecreateDispBuffer);

#ifdef __cplusplus
}
#endif


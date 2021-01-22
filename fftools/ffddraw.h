#include <ddraw.h>

#define IS_VALID(p)		(NULL != p)

#define SAFE_DELETE(p)		if(p != NULL){  delete p;	    p = NULL; }

#define SAFE_DELETE_ARRAY(p)		if(p != NULL){  delete[]p;	    p = NULL; }

#define SAFE_RELEASE(p)		if(p != NULL){	p->Release();   p = NULL; }

#define SAFE_CLOSE(p)		if(p != NULL){  ::CloseHandle(p); p = NULL; }

#define SAFE_CLOSE_FILE(f)	if ( f != NULL ){fclose(f);f=NULL;}

#define SAFE_DELETE_PTR(ptr)		if((ptr) != NULL){  delete[] (ptr);	    (ptr) = NULL; }

#define SAFE_CLOSESOCKET(s) if(s != INVALID_SOCKET){closesocket(s);s = INVALID_SOCKET;}

#define USE_VIRTUAL_MEMORY      0

#define MAX(a,b) (a) > (b)?(a):(b)


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
}FFDDraw;



void ddrawRelease(FFDDraw *ddraw,BOOL bRecreateDispBuffer);


BOOL ddrawCreate(FFDDraw *ddraw,HWND hWnd, int width, int height, BOOL bRecreateDispBuffer);
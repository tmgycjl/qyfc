#include "QYWindowShadow.h"
#include "QYDC.h"
#include "QYApp.h"

INT_PTR CALLBACK QYWindowShadow::WindowShadowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	QYWindowShadow *pWindow = (QYWindowShadow*)GetWindowLong(hWnd, GWLP_USERDATA);
	if (NULL != pWindow)
	{
		if ((INT_PTR)TRUE == pWindow->WindowProc(message, wParam, lParam))
		{
			return (INT_PTR)TRUE;
		}
	}

	switch (message)
	{
	case WM_INITDIALOG:
	{
		QYWindowShadow *pDlg = (QYWindowShadow *)lParam;
		if (NULL != pDlg)
		{
			pDlg->m_hWnd = hWnd;

			pDlg->OnInitDialog();
		}
	}
	break;
	}

	return pWindow->m_oldWndProc(hWnd, message, wParam, lParam);
}

QYWindowShadow::QYWindowShadow()
{
	m_nShadowPos = SHADOW_ON_WHOLE;
	m_bSmall = FALSE;

	m_picShadow = nullptr;
}


QYWindowShadow::~QYWindowShadow()
{
	SAFE_DELETE_ARRAY(m_picShadow);
}


BOOL QYWindowShadow::OnInitDialog()
{
	return FALSE;
}

void QYWindowShadow::OnMove(QYRect &rect)
{
	if (nullptr != m_hWnd)
	{
		QYRect rectDes = rect;

		rectDes.bottom += QY_THEME.WINDOW_SHADOW_SIDE_LEN_RIGHT_BOTTOM / (m_bSmall ? 2 : 1);
		rectDes.left -= QY_THEME.WINDOW_SHADOW_SIDE_LEN_LEFT_TOP / (m_bSmall ? 2 : 1);
		rectDes.right += QY_THEME.WINDOW_SHADOW_SIDE_LEN_RIGHT_BOTTOM / (m_bSmall ? 2 : 1);
		rectDes.top -= QY_THEME.WINDOW_SHADOW_SIDE_LEN_LEFT_TOP / (m_bSmall ? 2 : 1);

		::SetWindowPos(m_hWnd, HWND_NOTOPMOST, rectDes.left, rectDes.top, rectDes.Width(), rectDes.Height(), SWP_NOACTIVATE);
	}
	
}

 LRESULT QYWindowShadow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
 {
	 switch (message)
	 {
	 case WM_NCACTIVATE:
	 {
		 return TRUE;
	 }
	 break;
	 case WM_SETFOCUS: return TRUE; break;
	 case WM_NCDESTROY:SAFE_DELETE_ARRAY(m_picShadow);break;
	 case WM_SIZE:OnSize();break;
	 //case WM_MOVE:OnMove(); return TRUE;
	 default: break;
	 }
	 return FALSE;
 }

 void QYWindowShadow::OnSize()
 {
	 QYRect clientRect;
	 GetClientRect(clientRect);
	 HRGN hRgn = CreateRectRgn(0, 0, clientRect.right, clientRect.bottom);
	 HRGN hSubRgn = CreateRectRgn(QY_THEME.WINDOW_SHADOW_SIDE_LEN_LEFT_TOP / (m_bSmall ? 2 : 1),
		 QY_THEME.WINDOW_SHADOW_SIDE_LEN_LEFT_TOP / (m_bSmall ? 2 : 1),
		 clientRect.right - QY_THEME.WINDOW_SHADOW_SIDE_LEN_RIGHT_BOTTOM / (m_bSmall ? 2 : 1),
		 clientRect.bottom - QY_THEME.WINDOW_SHADOW_SIDE_LEN_RIGHT_BOTTOM / (m_bSmall ? 2 : 1));
	 ::CombineRgn(hRgn, hSubRgn, hRgn, RGN_XOR);
	 ::SetWindowRgn(m_hWnd, hRgn, TRUE);

	 DeleteObject(hRgn);
	 DeleteObject(hSubRgn);

	 QYRect rct;
	 GetWindowRect(rct);

	 HDC hdc = ::GetDC(m_hWnd);
	 HDC hdcMemory = ::CreateCompatibleDC(hdc);
	 HBITMAP hBitMap = ::CreateCompatibleBitmap(hdc, rct.Width(), rct.Height());
	 HBITMAP oldbitmap = (HBITMAP)::SelectObject(hdcMemory, hBitMap);

	 if (nullptr != m_picShadow)
	 {
		 if (m_bSmall)
		 {
			 int alignLen = 0;

			 QYPicture *p = m_picShadow + SHADOW_ID_SMALL_LEFT_TOP;

			 int hornLen = p->GetWidth();
			 p->Draw(hdcMemory,
				 0, 0, p->GetWidth(), p->GetHeight(),
				 0, 0, p->GetWidth(), p->GetHeight());

			 p = m_picShadow + SHADOW_ID_SMALL_LEFT_BOTTOM;
			 p->Draw(hdcMemory,
				 0, rct.Height() - p->GetHeight(), p->GetWidth(), p->GetHeight(),
				 0, 0, p->GetWidth(), p->GetHeight());

			 p = m_picShadow + SHADOW_ID_SMALL_RIGHT_TOP;
			 p->Draw(hdcMemory,
				 rct.Width() - p->GetWidth(), 0, p->GetWidth(), p->GetHeight(),
				 0, 0, p->GetWidth(), p->GetHeight());
			 p = m_picShadow + SHADOW_ID_SMALL_RIGHT_BOTTOM;
			 p->Draw(hdcMemory,
				 rct.Width() - p->GetWidth(), rct.Height() - p->GetHeight(), p->GetWidth(), p->GetHeight(),
				 0, 0, p->GetWidth(), p->GetHeight());

			 p = m_picShadow + SHADOW_ID_SMALL_TOP;

			 int left = hornLen;
			 int drawLen = rct.Width() - 2 * hornLen;
			 while (drawLen > 0)
			 {
				 p->Draw(hdcMemory,
					 left, 0, drawLen, p->GetHeight(),
					 0, alignLen, p->GetWidth(), p->GetHeight());

				 drawLen -= p->GetWidth();
				 left += p->GetWidth();
			 }


			 p = m_picShadow + SHADOW_ID_SMALL_BOTTOM;

			 left = hornLen;
			 drawLen = rct.Width() - 2 * hornLen;
			 while (drawLen > 0)
			 {
				 p->Draw(hdcMemory,
					 hornLen, rct.Height() - p->GetHeight() + alignLen, rct.Width() - 2 * hornLen, p->GetHeight(),
					 0, 0, p->GetWidth(), p->GetHeight());

				 drawLen -= p->GetWidth();
				 left += p->GetWidth();
			 }



			 p = m_picShadow + SHADOW_ID_SMALL_LEFT;
			 int top = hornLen;
			 drawLen = rct.Height() - 2 * hornLen;
			 while (drawLen > 0)
			 {
				 p->Draw(hdcMemory,
					 0, hornLen, p->GetWidth(), rct.Height() - 2 * hornLen,
					 alignLen, 0, p->GetWidth(), p->GetHeight());

				 drawLen -= p->GetHeight();
				 top += p->GetHeight();
			 }

			 p = m_picShadow + SHADOW_ID_SMALL_RIGHT;
			 top = hornLen;
			 drawLen = rct.Height() - 2 * hornLen;
			 while (drawLen > 0)
			 {
				 p->Draw(hdcMemory,
					 rct.Width() - p->GetWidth() + alignLen, hornLen, p->GetWidth(), rct.Height() - 2 * hornLen,
					 0, 0, p->GetWidth(), p->GetHeight());

				 drawLen -= p->GetHeight();
				 top += p->GetHeight();
			 }

		 }
		 else
		 {
#if 1
			 int alignLen = 0;

			 QYPicture *p = m_picShadow + SHADOW_ID_LEFT_TOP;

			 int hornLen = p->GetWidth();
			 p->Draw(hdcMemory,
				 0, 0, p->GetWidth(), p->GetHeight(),
				 0, 0, p->GetWidth(), p->GetHeight());

			 p = m_picShadow + SHADOW_ID_LEFT_BOTTOM;
			 p->Draw(hdcMemory,
				 0, rct.Height() - p->GetHeight(), p->GetWidth(), p->GetHeight(),
				 0, 0, p->GetWidth(), p->GetHeight());

			 p = m_picShadow + SHADOW_ID_RIGHT_TOP;
			 p->Draw(hdcMemory,
				 rct.Width() - p->GetWidth(), 0, p->GetWidth(), p->GetHeight(),
				 0, 0, p->GetWidth(), p->GetHeight());
			 p = m_picShadow + SHADOW_ID_RIGHT_BOTTOM;
			 p->Draw(hdcMemory,
				 rct.Width() - p->GetWidth(), rct.Height() - p->GetHeight(), p->GetWidth(), p->GetHeight(),
				 0, 0, p->GetWidth(), p->GetHeight());

			 p = m_picShadow + SHADOW_ID_TOP;

			 int left = hornLen;
			 int drawLen = rct.Width() - 2 * hornLen;
			 while (drawLen > 0)
			 {
				 p->Draw(hdcMemory,
					 left, 0, drawLen , p->GetHeight(),
					 0, alignLen, p->GetWidth(), p->GetHeight());

				 drawLen -= p->GetWidth();
				 left += p->GetWidth();
			 }


			 p = m_picShadow + SHADOW_ID_BOTTOM;

			 left = hornLen;
			 drawLen = rct.Width() - 2 * hornLen;
			 while (drawLen > 0)
			 {
				 p->Draw(hdcMemory,
					 hornLen , rct.Height() - p->GetHeight() + alignLen, rct.Width() - 2 * hornLen, p->GetHeight(),
					 0, 0, p->GetWidth(), p->GetHeight());

				 drawLen -= p->GetWidth();
				 left += p->GetWidth();
			 }



			 p = m_picShadow + SHADOW_ID_LEFT;
			 int top = hornLen;
			 drawLen = rct.Height() - 2 * hornLen;
			 while (drawLen > 0)
			 {
				 p->Draw(hdcMemory,
					 0, hornLen, p->GetWidth(), rct.Height() - 2 * hornLen,
					 alignLen, 0, p->GetWidth(), p->GetHeight());

				 drawLen -= p->GetHeight();
				 top += p->GetHeight();
			 }

			 p = m_picShadow + SHADOW_ID_RIGHT;
			 top = hornLen;
			 drawLen = rct.Height() - 2 * hornLen;
			 while (drawLen > 0)
			 {
				 p->Draw(hdcMemory,
					 rct.Width() - p->GetWidth() + alignLen, hornLen, p->GetWidth(), rct.Height() - 2 * hornLen,
					 0, 0, p->GetWidth(), p->GetHeight());

				 drawLen -= p->GetHeight();
				 top += p->GetHeight();
			 }
#else
			 QYPicture *p = m_picShadow + SHADOW_ID_LEFT_TOP;
			 int hornLen = p->GetWidth();
			 p->Draw(hdcMemory,
				 0, 0, p->GetWidth(), p->GetHeight(),
				 0, 0, p->GetWidth(), p->GetHeight());
			 p = m_picShadow + SHADOW_ID_LEFT_BOTTOM;
			 p->Draw(hdcMemory,
				 0, rct.Height() - p->GetHeight(), p->GetWidth(), p->GetHeight(),
				 0, 0, p->GetWidth(), p->GetHeight());

			 p = m_picShadow + SHADOW_ID_RIGHT_TOP;
			 p->Draw(hdcMemory,
				 rct.Width() - p->GetWidth(), 0, p->GetWidth(), p->GetHeight(),
				 0, 0, p->GetWidth(), p->GetHeight());
			 p = m_picShadow + SHADOW_ID_RIGHT_BOTTOM;
			 p->Draw(hdcMemory,
				 rct.Width() - p->GetWidth(), rct.Height() - p->GetHeight(), p->GetWidth(), p->GetHeight(),
				 0, 0, p->GetWidth(), p->GetHeight());

			 p = m_picShadow + SHADOW_ID_TOP;
			 p->Draw(hdcMemory,
				 hornLen, 0, rct.Width() - 2 * hornLen, p->GetHeight(),
				 0, 0, p->GetWidth(), p->GetHeight());

			 p = m_picShadow + SHADOW_ID_BOTTOM;
			 p->Draw(hdcMemory,
				 hornLen, rct.Height() - p->GetHeight(), rct.Width() - 2 * hornLen, p->GetHeight(),
				 0, 0, p->GetWidth(), p->GetHeight());

			 p = m_picShadow + SHADOW_ID_LEFT;
			 p->Draw(hdcMemory,
				 0, hornLen, p->GetWidth(), rct.Height() - 2 * hornLen,
				 0, 0, p->GetWidth(), p->GetHeight());

			 p = m_picShadow + SHADOW_ID_RIGHT;
			 p->Draw(hdcMemory,
				 rct.Width() - p->GetWidth(), hornLen, p->GetWidth(), rct.Height() - 2 * hornLen,
				 0, 0, p->GetWidth(), p->GetHeight());
#endif
			
		 }

		 SIZE sizeWindow = { rct.Width(), rct.Height() };
		 POINT ptWinPos = { rct.left, rct.top };
		 POINT ptSrc = { 0, 0 };
		 BLENDFUNCTION bf;

		 bf.AlphaFormat = AC_SRC_ALPHA;
		 bf.BlendFlags = 0;
		 bf.BlendOp = AC_SRC_OVER;
		 bf.SourceConstantAlpha = 127;
		 ::UpdateLayeredWindow(m_hWnd, hdc, &ptWinPos, &sizeWindow, hdcMemory, &ptSrc, 0, &bf, 2);

	 }
	 ::SelectObject(hdcMemory, oldbitmap);

	 ::DeleteDC(hdcMemory);

	 ::ReleaseDC(m_hWnd, hdc);

	 ::DeleteObject(hBitMap);

 }


 BOOL QYWindowShadow::OnEraseBkgnd()
 {
	 return TRUE;
 }

 HBITMAP shadowBitmap = nullptr;
 HBRUSH hShadowBrush = nullptr;

 static int aPattern[] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };//阴影位图数组 

 BOOL QYWindowShadow::OnPaint()
 {
	 QYPaintDC dc(this);
	 QYRect Clientrect;

	 HDC hDc = dc.GetDC();

	 GetClientRect(Clientrect);

	 RECT rt;
	 ::GetClientRect(m_hWnd,&rt);
	 QYBrush hClientBrush(RGB(0, 0, 0));

	 hClientBrush.DeleteObject();
	 
	 return TRUE;

	 HDC  hMemDC = ::CreateCompatibleDC(dc.GetDC());

	 HBITMAP bm = ::LoadBitmap(GetModuleHandle(L"QYUI.dll"), MAKEINTRESOURCE(108));
	 BITMAP bminfo;

	 GetObject(bm, sizeof(BITMAP), &bminfo);
	 HBITMAP hOldBm = (HBITMAP)::SelectObject(hMemDC, bm);

	 BLENDFUNCTION bf;

	 bf.AlphaFormat = 0;
	 bf.BlendFlags = 0;
	 bf.BlendOp = AC_SRC_OVER;
	 bf.SourceConstantAlpha = 122;

	 for (int i = 0; i < bminfo.bmHeight; i++)
	 {
		 for (int j = 0; j < bminfo.bmWidth; j++)
		 {
			 COLORREF color2 = RGB(255, 255, 255);
			 COLORREF color = GetPixel(hMemDC, i, j);
			 int r = color & 0x000000ff;
			 int g = color & 0x0000ff00;
			 int b = color & 0x00ff0000;
			 int a = color & 0xff000000;
			 if (color2 == color)
			 {
				 SetPixel(hMemDC, i, j, RGB(0,0,0));
			 }
			 
		 }
	 }
	 StretchBlt(dc.GetDC(), 0, Clientrect.Height() - 5, bminfo.bmWidth, 5, hMemDC,
		 0, bminfo.bmHeight - 5, bminfo.bmWidth, 5, SRCCOPY);

	 return TRUE;
 }


 BOOL QYWindowShadow::Create(UINT resID, QYWindow *pParentWnd, int shadowPos, BOOL bSmall )
 {
	 return Create(resID, pParentWnd->GetHwnd(), shadowPos, bSmall);
 }

 BOOL QYWindowShadow::Create(UINT resID, HWND hParentWnd, int shadowPos, BOOL bSmall)
 {
#if 0
	 BOOL ret = QYWindow::Create(WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_LAYERED,
		 L"QYWindowShadow",
		 L"",
		 WS_CHILD,
		 0, 0, 0, 0,
		 ::GetDesktopWindow());

	 if (!ret)
	 {
		 return FALSE;
	 }
#else
	 if (0 != resID)
	 {
		 if (nullptr == (m_hWnd = CreateDialogParam(QYApp::m_hInstance,
			 MAKEINTRESOURCE(resID),
			 hParentWnd,
			 (DLGPROC)WindowShadowProc,
			 (LPARAM)this)))
		 {
			 return FALSE;
		 }
	 }
	 else
	 {
		 WNDCLASSEX wcex;

		 wcex.cbSize = sizeof(WNDCLASSEX);
		 wcex.style = CS_HREDRAW | CS_VREDRAW;
		 wcex.lpfnWndProc = (WNDPROC)WindowShadowProc;
		 wcex.cbClsExtra = 0;
		 wcex.cbWndExtra = 0;
		 wcex.hInstance = QYApp::m_hInstance;
		 wcex.hIcon = /*LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QYUIDEMO))*/NULL;
		 wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		 wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		 wcex.lpszMenuName = nullptr;
		 wcex.lpszClassName = L"#32770";
		 wcex.hIconSm = /*LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL))*/NULL;

		 m_hWnd = CreateWindowEx(WS_EX_TRANSPARENT  | WS_EX_LAYERED, L"#32770", L"", WS_POPUP,
			 CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParentWnd, NULL, wcex.hInstance, this);
		 int error = GetLastError();
		 if (NULL == m_hWnd)
		 {
			 return false;
		 }

		 UINT style = GetWindowLong(m_hWnd, GWL_STYLE);
		 style &= ~WS_SYSMENU;
		 style &= ~WS_CAPTION;
		 style |= WS_POPUP;
		 SetWindowLong(GetHwnd(), GWL_STYLE, style);

		 UpdateWindow(m_hWnd);
	 }

	 //SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) ^ 0x80000);
#endif
	 m_nShadowPos = shadowPos;
	 m_bSmall = bSmall;

	 SetWindowLong(m_hWnd, GWLP_USERDATA, (LONG)this);

	 m_oldWndProc = (WNDPROC)SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (INT_PTR)WindowShadowProc);
	 m_picShadow = new QYPicture[SHADOW_ID_END];

	 m_picShadow[SHADOW_ID_LEFT_TOP].Load(IDP_SHADOW_LEFT_TOP, QY_IMAGE_PNG);
	 m_picShadow[SHADOW_ID_LEFT_BOTTOM].Load(IDP_SHADOW_LEFT_BOTTOM, QY_IMAGE_PNG);
	 m_picShadow[SHADOW_ID_RIGHT_TOP].Load(IDP_SHADOW_RIGHT_TOP, QY_IMAGE_PNG);
	 m_picShadow[SHADOW_ID_RIGHT_BOTTOM].Load(IDP_SHADOW_RIGHT_BOTTOM, QY_IMAGE_PNG);
	 m_picShadow[SHADOW_ID_TOP].Load(IDP_SHADOW_TOP, QY_IMAGE_PNG);
	 m_picShadow[SHADOW_ID_BOTTOM].Load(IDP_SHADOW_BOTTOM, QY_IMAGE_PNG);
	 m_picShadow[SHADOW_ID_RIGHT].Load(IDP_SHADOW_RIGHT, QY_IMAGE_PNG);
	 m_picShadow[SHADOW_ID_LEFT].Load(IDP_SHADOW_LEFT, QY_IMAGE_PNG);
	
	 m_picShadow[SHADOW_ID_SMALL_LEFT_TOP].Load(IDP_SMALL_SHADOW_LEFT_TOP, QY_IMAGE_PNG);
	 m_picShadow[SHADOW_ID_SMALL_LEFT_BOTTOM].Load(IDP_SMALL_SHADOW_LEFT_BOTTOM, QY_IMAGE_PNG);
	 m_picShadow[SHADOW_ID_SMALL_RIGHT_TOP].Load(IDP_SMALL_SHADOW_RIGHT_TOP, QY_IMAGE_PNG);
	 m_picShadow[SHADOW_ID_SMALL_RIGHT_BOTTOM].Load(IDP_SMALL_SHADOW_RIGHT_BOTTOM, QY_IMAGE_PNG);
	 m_picShadow[SHADOW_ID_SMALL_TOP].Load(IDP_SMALL_SHADOW_TOP, QY_IMAGE_PNG);
	 m_picShadow[SHADOW_ID_SMALL_BOTTOM].Load(IDP_SMALL_SHADOW_BOTTOM, QY_IMAGE_PNG);
	 m_picShadow[SHADOW_ID_SMALL_RIGHT].Load(IDP_SMALL_SHADOW_RIGHT, QY_IMAGE_PNG);
	 m_picShadow[SHADOW_ID_SMALL_LEFT].Load(IDP_SMALL_SHADOW_LEFT, QY_IMAGE_PNG);

	 if (nullptr != hParentWnd)
	 {
		 QYRect WinRC;
		 RECT rect;
		 ::GetWindowRect(hParentWnd, &rect);
		 WinRC = rect;

		 MoveWindow(WinRC.left - QY_THEME.WINDOW_SHADOW_SIDE_LEN_LEFT_TOP,
			 WinRC.top - QY_THEME.WINDOW_SHADOW_SIDE_LEN_LEFT_TOP,
			 WinRC.Width() + 2 * QY_THEME.WINDOW_SHADOW_SIDE_LEN_RIGHT_BOTTOM,
			 WinRC.Height() + 2 * QY_THEME.WINDOW_SHADOW_SIDE_LEN_RIGHT_BOTTOM);
	 }

	 return TRUE;
 }



 //QYDragShadow

 INT_PTR CALLBACK QYDragShadow::DragShadowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
 {
	 UNREFERENCED_PARAMETER(lParam);

	 QYDragShadow *pWindow = (QYDragShadow*)GetWindowLong(hWnd, GWLP_USERDATA);
	 if (NULL != pWindow)
	 {
		 if ((INT_PTR)TRUE == pWindow->WindowProc(message, wParam, lParam))
		 {
			 return (INT_PTR)TRUE;
		 }
	 }

	 switch (message)
	 {
	 case WM_INITDIALOG:
	 {
		 QYDragShadow *pDlg = (QYDragShadow *)lParam;
		 if (NULL != pDlg)
		 {
			 pDlg->m_hWnd = hWnd;

			 pDlg->OnInitDialog();
		 }
	 }
	 break;
	 }

	 return pWindow->m_oldWndProc(hWnd, message, wParam, lParam);
 }

 QYDragShadow::QYDragShadow()
 {
	 m_sizeWnd = QYSize(40,40);
 }


 QYDragShadow::~QYDragShadow()
 {
	 SAFE_DELETE(m_pPic);
 }


 BOOL QYDragShadow::OnInitDialog()
 {
	 return FALSE;
 }

 void QYDragShadow::TrackPop(QYPoint pt)
 {
	 if (nullptr != m_hWnd)
	 {
		 QYRect rectDes = QYRect(pt.x, pt.y, pt.x + m_sizeWnd.cx, pt.y + m_sizeWnd.cy);

		 ::SetWindowPos(m_hWnd, nullptr, rectDes.left, rectDes.top, rectDes.Width(), rectDes.Height(), SWP_NOACTIVATE);
		 Show(SW_SHOW);
	 }
 }

 LRESULT QYDragShadow::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
 {
	 // g_ConsoleLog.Print("message %04x\n", message);
	 switch (message)
	 {
	 case WM_NCHITTEST:
	 {
		 return TRUE;
	 }
	 case WM_SETFOCUS:
	 {
		 return TRUE;
	 }
	 break;
	 case WM_MOUSEMOVE:break;
	 case WM_NCDESTROY:SAFE_DELETE(m_pPic);break;
	 case WM_LBUTTONDOWN:
	 {
		 return TRUE;
	 }
	 break;
	 case WM_LBUTTONUP:
	 {
		 return TRUE;
	 }
	 break;
	 case WM_SIZE:
	 {
		
	 }
	 break;
	
	 case WM_PAINT:OnPaint(); break;
	 default:
		 break;
	 }
	 return FALSE;
 }



 BOOL QYDragShadow::OnEraseBkgnd()
 {
	 return TRUE;
 }

 BOOL QYDragShadow::OnPaint()
 {
	 QYPaintDC dc(this);
	 QYRect Clientrect;

	 HDC hDc = dc.GetDC();

	 GetClientRect(Clientrect);

	 RECT rt;
	 ::GetClientRect(m_hWnd, &rt);
	 QYBrush hClientBrush(RGB(0, 0, 0));

	 dc.FillRect(Clientrect, hClientBrush);

	 hClientBrush.DeleteObject();

	 return TRUE;
 }


 BOOL QYDragShadow::Create(UINT resID, QYWindow *pParentWnd, int shadowPos, BOOL bSmall)
 {
	 return Create(resID, pParentWnd->GetHwnd(), shadowPos, bSmall);
 }

 BOOL QYDragShadow::Create(UINT resID, HWND hParentWnd, int shadowPos, BOOL bSmall)
 {
	 if (0 != resID)
	 {
		 if (nullptr == (m_hWnd = CreateDialogParam(QYApp::m_hInstance,
			 MAKEINTRESOURCE(resID),
			 hParentWnd,
			 (DLGPROC)DragShadowProc,
			 (LPARAM)this)))
		 {
			 return FALSE;
		 }
	 }
	 else
	 {
		 WNDCLASSEX wcex;

		 wcex.cbSize = sizeof(WNDCLASSEX);
		 wcex.style = CS_HREDRAW | CS_VREDRAW;
		 wcex.lpfnWndProc = (WNDPROC)DragShadowProc;
		 wcex.cbClsExtra = 0;
		 wcex.cbWndExtra = 0;
		 wcex.hInstance = QYApp::m_hInstance;
		 wcex.hIcon = /*LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QYUIDEMO))*/NULL;
		 wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		 wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		 wcex.lpszMenuName = nullptr;
		 wcex.lpszClassName = L"#32770";
		 wcex.hIconSm = /*LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL))*/NULL;

		 m_hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, L"#32770", L"", WS_POPUPWINDOW,
			 CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, hParentWnd, NULL, wcex.hInstance, this);
		 int error = GetLastError();
		 if (NULL == m_hWnd)
		 {
			 return false;
		 }

		 UINT style = GetWindowLong(m_hWnd, GWL_STYLE);
		 style &= ~WS_SYSMENU;
		 style &= ~WS_CAPTION;
		 style |= WS_POPUP;
		 SetWindowLong(GetHwnd(), GWL_STYLE, style);
	 }

	 SetWindowLong(m_hWnd, GWLP_USERDATA, (LONG)this);

	 SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) ^ 0x80000);

	 m_oldWndProc = (WNDPROC)SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (INT_PTR)DragShadowProc);

	 return TRUE;
 }
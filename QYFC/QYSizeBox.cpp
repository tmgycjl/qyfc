#include "QYSizeBox.h"


QYSizeBox::QYSizeBox()
{	
	m_bDownBorder = FALSE;
	m_nResizeAction = -1;
	m_nSizeBoxWidth = 8;
	m_minSize = QYSize(0, 0);

	auto cX = GetSystemMetrics(SM_CXSCREEN);
	auto cY = GetSystemMetrics(SM_CYSCREEN);

	m_MaxMinInfo.ptMaxTrackSize.x = cX;
	m_MaxMinInfo.ptMaxTrackSize.y = cY;
	m_MaxMinInfo.ptMinTrackSize.x = 400;
	m_MaxMinInfo.ptMinTrackSize.y = 300;
}


QYSizeBox::~QYSizeBox()
{
}

BOOL QYSizeBox::StartResize(QYWindow *pWnd, QYPoint &point)
{
	QYRect rt, rtWnd, rtParentWnd;

	pWnd->GetWindowRect(rtWnd);

	rt = QYRect(0, 0, rtWnd.Width(), rtWnd.Height());

	m_LastPoint = point;
	pWnd->ClientToScreen(&m_LastPoint);
	pWnd->GetWindowRect(m_lastWndRect);
	if (rt.PtInRect(point))
	{
		QYRect rtSub;
		rtSub = rt + QYRect(m_nSizeBoxWidth, m_nSizeBoxWidth, -m_nSizeBoxWidth, -m_nSizeBoxWidth);

		if (!rtSub.PtInRect(point))
		{
			if (point.x > rtSub.right)
			{
				if (point.y > rtSub.bottom)
				{
					m_hCursor = LoadCursor(NULL, IDC_SIZENWSE);
					m_nResizeAction = RESIZE_RIGHT_DOWN;
				}
				else if (point.y < rtSub.top)
				{
					m_hCursor = LoadCursor(NULL, IDC_SIZENESW);
					m_nResizeAction = RESIZE_RIGHT_UP;
				}
				else
				{
					m_hCursor = LoadCursor(NULL, IDC_SIZEWE);
					m_nResizeAction = RESIZE_RIGHT;
				}
				pWnd->PostMessage(WM_SETCURSOR, 0, 0);
			}
			else if (point.x < rtSub.left)
			{
				if (point.y > rtSub.bottom)
				{
					m_hCursor = LoadCursor(NULL, IDC_SIZENESW);
					m_nResizeAction = RESIZE_LEFT_DOWN;
				}
				else if (point.y < rtSub.top)
				{
					m_hCursor = LoadCursor(NULL, IDC_SIZENWSE);
					m_nResizeAction = RESIZE_LEFT_UP;
				}
				else
				{
					m_hCursor = LoadCursor(NULL, IDC_SIZEWE);
					m_nResizeAction = RESIZE_LEFT;
				}
				pWnd->PostMessage(WM_SETCURSOR, 0, 0);
			}
			else
			{
				if (point.y > rtSub.bottom)
				{
					m_nResizeAction = RESIZE_DOWN;
				}
				else if (point.y < rtSub.top)
				{
					m_nResizeAction = RESIZE_UP;
				}
				m_hCursor = LoadCursor(NULL, IDC_SIZENS);
				pWnd->PostMessage(WM_SETCURSOR, 0, 0);
			}

			m_bDownBorder = TRUE;
			pWnd->SetCapture();
		}

	}
	return TRUE;
}

BOOL QYSizeBox::Resize(QYWindow *pWnd, QYPoint &point)
{
	QYRect rt, rtWnd;

	pWnd->GetWindowRect(rtWnd);

	rt = QYRect(0, 0, rtWnd.Width(), rtWnd.Height());

	if (m_bDownBorder)
	{
		if (-1 == m_nResizeAction)
		{
			return TRUE;
		}

		
		QYRect lastrtWnd = rtWnd;

		//g_ConsoleLog.Print("lParam:%d\n", point.x);
		pWnd->ClientToScreen(&point);
		switch (m_nResizeAction)
		{
		case RESIZE_LEFT:
		{
			if (point.x > 1024)
			{
				return TRUE;
			}
			rtWnd.left = m_lastWndRect.left + point.x - m_LastPoint.x;
		}
			break;
		case RESIZE_UP:
		{
			if (point.y > 1024)
			{
				return TRUE;
			}
			rtWnd.top = m_lastWndRect.top + point.y - m_LastPoint.y;
		}
			break;
		case RESIZE_RIGHT:
			rtWnd.right = m_lastWndRect.right + point.x - m_LastPoint.x;
			break;
		case RESIZE_DOWN:
			rtWnd.bottom = m_lastWndRect.bottom + point.y - m_LastPoint.y;
			break;
		case RESIZE_LEFT_UP:
		{
			if (point.x > 1024)
			{
				return TRUE;
			}
			rtWnd.left = m_lastWndRect.left + point.x - m_LastPoint.x;
			rtWnd.top = m_lastWndRect.top + point.y - m_LastPoint.y;
		}
			break;
		case RESIZE_LEFT_DOWN:
			rtWnd.left = m_lastWndRect.left + point.x - m_LastPoint.x;
			rtWnd.bottom = m_lastWndRect.bottom + point.y - m_LastPoint.y;
			break;
		case RESIZE_RIGHT_UP:
		{
			rtWnd.right = m_lastWndRect.right + point.x - m_LastPoint.x;
			if (point.y > 1024)
			{
				return TRUE;
			}
			rtWnd.top = m_lastWndRect.top + point.y - m_LastPoint.y;
		}
			break;
		case RESIZE_RIGHT_DOWN:
		{
			rtWnd.right = m_lastWndRect.right + point.x - m_LastPoint.x;
			rtWnd.bottom = m_lastWndRect.bottom + point.y - m_LastPoint.y;
		}
			break;

		default:
			return TRUE;
			break;
		}

		if (rtWnd.Width() > m_MaxMinInfo.ptMaxTrackSize.x 
			|| rtWnd.Height() >  m_MaxMinInfo.ptMaxTrackSize.y
			|| rtWnd.Width() < m_MaxMinInfo.ptMinTrackSize.x 
			|| rtWnd.Height() < m_MaxMinInfo.ptMinTrackSize.y)
		{
			return TRUE;
		}

		if (lastrtWnd.Width() != rtWnd.Width()
			|| lastrtWnd.Height() != rtWnd.Height())
		{
			pWnd->MoveWindow(rtWnd);
			pWnd->Invalidate();
		}
	}
	else
	{
		if (rt.PtInRect(point))
		{

			QYRect rtSub;
			rtSub = rt + QYRect(m_nSizeBoxWidth, m_nSizeBoxWidth, -m_nSizeBoxWidth, -m_nSizeBoxWidth);

			if (!rtSub.PtInRect(point))
			{
				if (point.x > rtSub.right)
				{
					if (point.y > rtSub.bottom)
					{
						m_hCursor = LoadCursor(NULL, IDC_SIZENWSE);
					}
					else if (point.y < rtSub.top)
					{
						m_hCursor = LoadCursor(NULL, IDC_SIZENESW);
					}
					else
					{
						m_hCursor = LoadCursor(NULL, IDC_SIZEWE);
					}
					pWnd->PostMessage(WM_SETCURSOR, 0, 0);
				}
				else if (point.x < rtSub.left)
				{
					if (point.y > rtSub.bottom)
					{
						m_hCursor = LoadCursor(NULL, IDC_SIZENESW);
					}
					else if (point.y < rtSub.top)
					{
						m_hCursor = LoadCursor(NULL, IDC_SIZENWSE);
					}
					else
					{
						m_hCursor = LoadCursor(NULL, IDC_SIZEWE);
					}pWnd->PostMessage(WM_SETCURSOR, 0, 0);
				}
				else
				{
					if (point.y > rtSub.bottom)
					{
						m_hCursor = LoadCursor(NULL, IDC_SIZENS);
						 pWnd->PostMessage(WM_SETCURSOR, 0, 0);
					}
					else if (point.y < rtSub.top)
					{
						m_hCursor = LoadCursor(NULL, IDC_SIZENS);
						 pWnd->PostMessage(WM_SETCURSOR, 0, 0);
					}
				}
			}
			else
			{
				m_hCursor = LoadCursor(NULL, IDC_ARROW);
				 pWnd->PostMessage(WM_SETCURSOR, 0, 0);
			}
		}
	}
	return TRUE;
}

BOOL QYSizeBox::EndResize()
{
	m_bDownBorder = FALSE;
	::SetCursor(LoadCursor(NULL, IDC_ARROW));
	ReleaseCapture();

	return TRUE;
}
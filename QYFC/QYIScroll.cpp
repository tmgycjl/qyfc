#include "QYIScroll.h"
#include "QYScrollBar.h"

QYIScroll::QYIScroll()
{
	
	m_bVShowScroll = FALSE;
	m_bHShowScroll = FALSE;
	m_nScrollPos = 0;
	m_nVScrollWidth = 0;
	m_nHScrollHeight = 0;
	m_nHScrollPos = 0;
	m_bOpenTrackPosTime = FALSE;
	m_nHTrackPos = 0;
}

QYIScroll::~QYIScroll()
{

}

bool QYIScroll::onVScroll(QYWindow *pWnd, UINT nSBCode)
{ 
	switch (nSBCode)
	{
	case SB_LINEDOWN:
	case SB_LINEUP:
	case SB_PAGEUP:
	case SB_PAGEDOWN:
	{
		m_nScrollPos = m_VScrollBar.GetPos();
		return true;
	}
	break;
	case SB_THUMBTRACK:
	{
		m_nTrackPos = m_VScrollBar.GetPos();

		if (!m_bOpenTrackPosTime)
		{
			pWnd->SetTimer(2, 10, nullptr);
			m_bOpenTrackPosTime = TRUE;
		}
	}
	break;
	case SB_THUMBPOSITION:
	{
		pWnd->KillTimer(2);
		m_bOpenTrackPosTime = FALSE;
	}
		break;
	default:
		break;
	}

	return false;
}

bool QYIScroll::onHScroll(QYWindow *pWnd, UINT nSBCode)
{
	switch (nSBCode)
	{
	case SB_LINELEFT:
	case SB_LINERIGHT:
	case SB_PAGELEFT:
	case SB_PAGERIGHT:
	case SB_THUMBTRACK:
	{
		m_nHTrackPos = m_HScrollBar.GetPos();

		if (!m_bOpenTrackPosTime)
		{
			pWnd->SetTimer(3, 10, nullptr);
			m_bOpenTrackPosTime = TRUE;
		}
	}
	break;
	case SB_THUMBPOSITION:
	{
		pWnd->KillTimer(3);
		m_bOpenTrackPosTime = FALSE;
	}
		break;
	default:
		break;
	}

	return false;
}


bool QYIScroll::onTimer(QYWindow *pWnd, UINT_PTR nIDEvent)
{
	if (5 <= m_nWheelCount++ && 2 != nIDEvent && 3 != nIDEvent)
	{
		pWnd->KillTimer(1);
		return false;
	}

	if (1 == nIDEvent)
	{
		int offset = 1;
		if (m_nZDelta < 0)
		{
			if (m_nScrollPos >= m_VScrollBar.GetRange())
			{
				return false;
			}

			m_nScrollPos += m_VScrollBar.getStep();
			m_VScrollBar.SetPos(m_nScrollPos);
 			//m_nScrollPos = m_VScrollBar.GetPos();
			return true;
		}
		else
		{
			if (m_nScrollPos <= 0)
			{
				return false;
			}
			m_nScrollPos -= m_VScrollBar.getStep();
			if (0 > m_nScrollPos)
			{
				m_nScrollPos = 0;
			}
			m_VScrollBar.SetPos(m_nScrollPos);
			//m_nScrollPos = m_VScrollBar.GetPos();
			return true;
		}
	}
	else if (2 == nIDEvent)
	{
		int nlastPos = m_nScrollPos;
		m_nScrollPos = m_nTrackPos;
		if (nlastPos != m_nScrollPos)
		{
			return true;
		}
	}
	else if (3 == nIDEvent)
	{
		int nlastPos = m_nHScrollPos;
		m_nHScrollPos = m_nHTrackPos;
		if (nlastPos != m_nHScrollPos)
		{
			return true;
		}
	}

	return false;
}

LRESULT QYIScroll::onMouseWheel(QYWindow *pWnd, UINT nFlags, short zDelta)
{
	if (m_bVShowScroll)
	{
		if (nFlags == 0)
		{
			pWnd->KillTimer(1);
			pWnd->KillTimer(2);
			pWnd->KillTimer(3);
			m_nZDelta = zDelta;
			m_nWheelCount = 0;
			pWnd->SetTimer(1, 20, nullptr);
		}
	}

	return TRUE;
}

void QYIScroll::updateVScrollLayout(QYWindow *pWnd, QYRect &rect, int scrollRange)
{
	if (0 >= scrollRange)
	{
		if (m_bVShowScroll)
		{
			if (nullptr != m_VScrollBar.GetHwnd())
			{
				m_VScrollBar.SetPos(0);
				m_nScrollPos = 0;

				m_VScrollBar.Show(SW_HIDE);
			}

			m_bVShowScroll = FALSE;
			m_nVScrollWidth = 0;
		}
	}
	else
	{
		m_bVShowScroll = TRUE;
		m_nVScrollWidth = QY_THEME.SCROLLBAR_WIDTH;
	}

	if (m_bVShowScroll)
	{
		if (nullptr == m_VScrollBar.GetHwnd())
		{
			if (!m_VScrollBar.Create(0, nullptr, nullptr, WS_CHILD, QYRect(0, 0, 0, 0), pWnd->GetHwnd()))
			{
				return;
			}
		}

		m_nScrollPos = m_VScrollBar.GetPos();

		m_VScrollBar.SetRange(scrollRange);

		int borderWidth = pWnd->getBorderWidth();
		QYRect rectScroll = rect;
		rectScroll.right = rect.right - borderWidth;
		rectScroll.left = rectScroll.right - m_nVScrollWidth;
		rectScroll.top = rect.top + borderWidth;
		rectScroll.bottom = rect.bottom - borderWidth - m_nHScrollHeight;
		m_VScrollBar.MoveWindow(rectScroll);

		m_VScrollBar.SetPos(m_nScrollPos);
		m_VScrollBar.Show(SW_SHOW);
	}
}

void QYIScroll::updateHScrollLayout(QYWindow *pWnd, QYRect &rect, int scrollRange)
{
	if (scrollRange <= 0)
	{
		if (m_bHShowScroll)
		{
			if (nullptr != m_HScrollBar.GetHwnd())
			{
				m_HScrollBar.SetPos(0);
				m_nHScrollPos = 0;

				m_HScrollBar.Show(SW_HIDE);
			}

			m_bHShowScroll = FALSE;
			m_nHScrollHeight = 0;
		}

	}
	else
	{
		m_bHShowScroll = TRUE;
		m_nHScrollHeight = QY_THEME.SCROLLBAR_WIDTH;
	}

	int nScrollRange = 0;

	if (m_bHShowScroll)
	{
		int borderWidth = pWnd->getBorderWidth();

		if (nullptr == m_HScrollBar.GetHwnd())
		{
			if (!m_HScrollBar.Create(0, nullptr, nullptr, WS_CHILD, QYRect(0, 0, 0, 0), pWnd->GetHwnd(), SB_HORZ))
			{
				return;
			}
		}

		m_nHScrollPos = m_HScrollBar.GetPos();

		m_HScrollBar.SetRange(nScrollRange);

		QYRect rectScroll = rect;
		rectScroll.bottom = rect.bottom - borderWidth;
		rectScroll.top = rectScroll.bottom - m_nHScrollHeight;
		rectScroll.left += borderWidth;
		rectScroll.right = rect.right - borderWidth - m_nVScrollWidth;
		m_HScrollBar.MoveWindow(rectScroll);

		m_HScrollBar.SetPos(m_nHScrollPos);
		m_HScrollBar.Show(SW_SHOW);
	}

}
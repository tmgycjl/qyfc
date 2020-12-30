
#include "QYSplitterCtrlManager.h"


QYSplitterCtrlManager::QYSplitterCtrlManager()
{
	m_nSplitterWidth = 1;
	m_RootSplitter = nullptr;
}


QYSplitterCtrlManager::~QYSplitterCtrlManager()
{
	SAFE_DESTROY_WINDOW_PTR(m_RootSplitter);
}


QYSplitterCtrl* QYSplitterCtrlManager::AddSplitter(QYSplitterCtrl* pParentSplitter, QYWindow *pParentWnd, enumSplitterDockFlag dockFlag, float fX, float fY)
{
	QYSplitterCtrl *pCtrl = new QYSplitterCtrl;
	assert(nullptr != pCtrl);

	if (pCtrl->Create(0, nullptr, WS_CHILD | WS_VISIBLE, 0, pParentWnd, dockFlag))
	{
		pCtrl->SetX(fX);
		pCtrl->SetY(fY);

		if (nullptr == pParentSplitter)
		{
			if (nullptr == m_RootSplitter)
			{
				m_RootSplitter = pCtrl;
			}
			else
			{
				SAFE_DESTROY_WINDOW_PTR(pCtrl);
				return nullptr;
			}
		}
		else
		{
			if (SPLITTER_DOCK_LEFT == dockFlag || SPLITTER_DOCK_TOP == dockFlag)
			{
				if (!pParentSplitter->AddFirstSubSplitter(pCtrl))
				{
					SAFE_DESTROY_WINDOW_PTR(pCtrl);
					return nullptr;
				}
			}
			else
			{
				if (!pParentSplitter->AddSecondSubSplitter(pCtrl))
				{
					SAFE_DESTROY_WINDOW_PTR(pCtrl);
					return nullptr;
				}
			}

			
		}

		return pCtrl;
	}

	return nullptr;
}


QYSplitterCtrl* QYSplitterCtrlManager::AddFirstSplitter(QYSplitterCtrl* pParentSplitter, QYWindow *pParentWnd, enumSplitterDockFlag dockFlag, float fX, float fY)
{
	if (nullptr == pParentSplitter)
	{
		return nullptr;
	}

	QYSplitterCtrl *pCtrl = new QYSplitterCtrl;
	assert(nullptr != pCtrl);

	if (pCtrl->Create(0, nullptr, WS_CHILD | WS_VISIBLE, 0, pParentWnd, dockFlag))
	{
		pCtrl->SetX(fX);
		pCtrl->SetY(fY);

			if (!pParentSplitter->AddFirstSubSplitter(pCtrl))
			{
				SAFE_DESTROY_WINDOW_PTR(pCtrl);
				return nullptr;
			}

		return pCtrl;
	}

	return nullptr;
}


QYSplitterCtrl* QYSplitterCtrlManager::AddSecondSplitter(QYSplitterCtrl* pParentSplitter, QYWindow *pParentWnd, enumSplitterDockFlag dockFlag, float fX, float fY)
{
	if (nullptr == pParentSplitter)
	{
		return nullptr;
	}

	QYSplitterCtrl *pCtrl = new QYSplitterCtrl;
	assert(nullptr != pCtrl);

	if (pCtrl->Create(0, nullptr, WS_CHILD | WS_VISIBLE, 0, pParentWnd, dockFlag))
	{
		pCtrl->SetX(fX);
		pCtrl->SetY(fY);
			if (!pParentSplitter->AddSecondSubSplitter(pCtrl))
			{
				SAFE_DESTROY_WINDOW_PTR(pCtrl);
				return nullptr;
			}

		return pCtrl;
	}

	return nullptr;
}
void QYSplitterCtrlManager::UpdateLayout(QYRect &rect)
{
	if (nullptr == m_RootSplitter)
	{
		return;
	}
	QYRect rectSplitter = rect, first = rect, second = rect;
		int type = m_RootSplitter->GetType();
		float fX = m_RootSplitter->GetX();
		float fY = m_RootSplitter->GetY();
		if (SPLITTER_DOCK_LEFT == m_RootSplitter->GetType())
		{
			if (0 < fX)
			{
				if (1 >= fX)
				{
					first.right = rect.left + ((rect.Width() - m_nSplitterWidth)* fX);
				}
				else
				{
					first.right = rect.left + fX;
				}

			}
			else if (0 > fX)
			{
				if (-1 >= fX)
				{
					first.right = rect.right - m_nSplitterWidth + fX;
				}
				else
				{
					first.right = rect.right + (rect.Width() - m_nSplitterWidth)*  fX;
				}
			}
			else
			{
				first.right = rect.right - m_nSplitterWidth;
			}

			rectSplitter.left = first.right;
			rectSplitter.right = rectSplitter.left + m_nSplitterWidth;

			second.left = rectSplitter.right;
		}
		else if(SPLITTER_DOCK_TOP == m_RootSplitter->GetType())
		{
			if (0 < fY)
			{
				if (1 >= fY)
				{
					first.bottom = rect.top + ((rect.Height() - m_nSplitterWidth)* fY);
				}
				else
				{
					first.bottom = rect.top + fY;
				}

			}
			else if (0 > fY)
			{
				if (-1 >= fY)
				{
					first.bottom = rect.bottom - m_nSplitterWidth + fY;
				}
				else
				{
					first.bottom = rect.bottom + (rect.Height() - m_nSplitterWidth)*  fY;
				}
			}
			else
			{
				first.bottom = rect.bottom - m_nSplitterWidth;
			}

			if (0 < fX)
			{
				if (1 >= fX)
				{
					first.right = rect.left + ((rect.Width() - m_nSplitterWidth)* fX);
				}
				else
				{
					first.right = rect.left + fX;
				}

			}
			else if (0 > fX)
			{
				if (-1 >= fX)
				{
					first.right = rect.right - m_nSplitterWidth + fX;
				}
				else
				{
					first.right = rect.right + (rect.Width() - m_nSplitterWidth)*  fX;
				}
			}
			else
			{
				first.right = rect.right - m_nSplitterWidth;
			}

			rectSplitter.top = first.bottom;
			rectSplitter.bottom = rectSplitter.top + m_nSplitterWidth;
			rectSplitter.right = first.right;

			second.top = rectSplitter.bottom;
		}
		else if(SPLITTER_DOCK_RIGHT == m_RootSplitter->GetType())
		{

		}
		else
		{
			
		}
		m_RootSplitter->MoveWindow(rectSplitter);
		m_RootSplitter->MoveView(first, second, m_nSplitterWidth);
}

void QYSplitterCtrlManager::ClearSplitter()
{
	if (nullptr != m_RootSplitter)
	{
		m_RootSplitter->ClearSubSplitter();
		SAFE_DESTROY_WINDOW_PTR(m_RootSplitter);
	}
}
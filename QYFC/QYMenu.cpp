#include "QYMenu.h"
#include "QYDC.h"

#include "QYLanguage.h"

QYMenu::QYMenu(QYWindow *pSounrceWnd) :QYWindow()
{
	m_nItemHeight = 30;
	m_nBorderWidth = 1;
	m_bHover = FALSE;
	m_nSelItem = -1;
	m_pSourceWnd = NULL;
	m_DefaultHeight =0;
	m_pShadow = nullptr;
	m_pSourceWnd = pSounrceWnd;
	m_nSeparateHeight = 0;
	m_padHeight = 10;
	m_padWidth = 30;
	m_pCheckImage = nullptr;
	m_pParentMenu = nullptr;
}

QYMenu::~QYMenu()
{
	Clear();
}

LRESULT QYMenu::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ERASEBKGND:{return TRUE;break;}
	case WM_PAINT:{return OnPaint();}break;
	case WM_SIZE:{OnSize(0, 0, 0);}break;
	case WM_NCPAINT:{return TRUE;break;}
	case WM_NCDESTROY:
	{
		Clear();
		SAFE_DELETE(m_pCheckImage);
		SAFE_DESTROY_WINDOW_PTR(m_pShadow);
	}
		break;
	case WM_MOUSEMOVE:{OnMouseMove(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));break;}
	case WM_MOUSELEAVE:{OnMouseLeave(wParam, lParam);}break;
	case WM_LBUTTONDOWN:{OnLButtonDown(0, QYPoint(LOWORD(lParam), HIWORD(lParam)));}break;
	case WM_LBUTTONUP:{OnLButtonUp(0, QYPoint(LOWORD(lParam), HIWORD(lParam))); }break;
	case WM_SETFOCUS:{}break;
	case WM_KILLFOCUS:{}break;
	case WM_MOVE:{OnMove(LOWORD(lParam), HIWORD(lParam));break;}
	case QY_WM_SUBMENU_HIDED:OnSubMenuHided();break;
	case QY_WM_CLOSE_POP_WINDOW:onCloseWindow(wParam, lParam); break;
	default:{break; }
	}

	return 0L;
}


BOOL QYMenu::Create()
{

	BOOL ret =  QYWindow::Create( WS_EX_TOOLWINDOW,
		L"QYMenu",
		nullptr,
		WS_CHILD | WS_CLIPCHILDREN,
		0,
		0,
		0,
		0,
		::GetDesktopWindow());

	if (!ret )
	{
		return FALSE;
	}

	m_pCheckImage = new QYPicture;
	if (!m_pCheckImage->Load(IDP_CHECK_SELECTED, QY_IMAGE_PNG))
	{
		SAFE_DELETE(m_pCheckImage);
	}

 	m_pShadow = new QYWindowShadow;
	m_pShadow->Create(0, this, SHADOW_ON_WHOLE);
 	m_pShadow->Show(SW_SHOW);

	return ret;
}

QYWindow* QYMenu::SetParentMenu(QYWindow* pParentMenu)
{
	QYWindow *pLastParentMenu = m_pParentMenu;

	m_pParentMenu = pParentMenu;

	return pLastParentMenu;
}

void QYMenu::onCloseWindow(WPARAM wParam, LPARAM lParam)
{
	ReleaseCapture();
	Show(SW_HIDE);
	(nullptr != m_pParentMenu) ? ((QYMenu*)m_pParentMenu)->PostMessage(QY_WM_SUBMENU_HIDED) : 0;
}

LRESULT QYMenu::OnMouseMove(UINT nFlags, QYPoint point)
{
	if (!m_bHover)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 10;
		tme.hwndTrack = m_hWnd;
		::TrackMouseEvent(&tme);
		m_bHover = TRUE;
		Invalidate();
	}


	{
		QYRect rect;
		GetClientRect(rect);

		int lastSelItem = m_nSelItem;
		int nY = point.y - m_nBorderWidth - m_padHeight;
		int selItem = -1;
		
		
		if (m_nBorderWidth <= point.x && point.x <= rect.right - m_nBorderWidth)
		{
			if (nY >= 0)
			{
				selItem = nY / m_nItemHeight;
			}
		}

		if (0 <= selItem && selItem < m_vectorItem.size())
		{
			m_nSelItem = selItem;
			if (lastSelItem != m_nSelItem)
			{
				auto it = m_vectorItem.begin();
				if (-1 != lastSelItem && lastSelItem < m_vectorItem.size())
				{
					auto it = m_vectorItem.begin();
					std::advance(it, lastSelItem);
					if (nullptr != (*it)->pSubMenu)
					{
						((QYMenu*)(*it)->pSubMenu)->Show(SW_HIDE);
					}
				}

				it = m_vectorItem.begin();
				std::advance(it, m_nSelItem);
				if (nullptr != (*it)->pSubMenu)
				{
					QYPoint ptPop;
					ptPop.y = m_nBorderWidth + m_padHeight + (selItem * m_nItemHeight);
					ptPop.x = rect.right;
					ClientToScreen(&ptPop);
					((QYMenu*)(*it)->pSubMenu)->TrackPopup(ptPop, 0, TPMENU_RIGHT_BOTTOM, m_pSourceWnd, true);
				}

				if (nullptr == QYApp::m_hPopWindow && nullptr == QYApp::m_hPopWndMouseHook)
				{
					QYApp::m_hPopWindow = m_hWnd;
					QYApp::m_hPopWndMouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)QYApp::QYPopWndMouseHookProc, QYApp::m_hInstance, 0);
				}

				Invalidate();
			}
			
		}
		else
		{
			if (-1 != lastSelItem && lastSelItem < m_vectorItem.size())
			{
				auto it = m_vectorItem.begin();
				std::advance(it, lastSelItem);
				if (nullptr != (*it)->pSubMenu)
				{
					((QYMenu*)(*it)->pSubMenu)->Show(SW_HIDE);
				}
			}

			m_nSelItem = -1;

			QYApp::StartPopWndMouseHook(m_hWnd);

			Invalidate();
		}
	}

	return TRUE;
	//CWnd::OnMouseMove(nFlags, point);
}

LRESULT QYMenu::OnMouseLeave(WPARAM, LPARAM)
{
	if (m_bHover)
	{
		m_bHover = FALSE;
		m_nSelItem = -1;
		Invalidate();
	}
	return TRUE;
}

// CDropList 消息处理程序
int QYMenu::AddItem(UINT id, LPCTSTR strText, DWORD_PTR data)
{
	//std::vector<QYDropListItem> m_vectorIem;
	QYMenuItem *item = new QYMenuItem;
	item->id = id;
	item->strText = strText;
	item->data = data;
	m_vectorItem.push_back(item);

	return m_vectorItem.size();
}

int QYMenu::addItem(UINT id, const char* text, DWORD_PTR data)
{
	//std::vector<QYDropListItem> m_vectorIem;
	QYMenuItem *item = new QYMenuItem;
	item->id = id;
	const char *srcText = QYLanguage::instance()->getText(text);
	SET_TEXT(item->strText, srcText)
	item->data = data;
	m_vectorItem.push_back(item);

	return m_vectorItem.size();
}

int QYMenu::addItem(std::string key, const char* text)
{
	QYMenuItem *item = new QYMenuItem;
	item->key = key;
	const char *srcText = QYLanguage::instance()->getText(text);
	SET_TEXT(item->strText, srcText)
	item->data = 0;
	m_vectorItem.push_back(item);

	return m_vectorItem.size();
}

bool QYMenu::EnableItem(UINT id,bool enable)
{
	for (QYMenuItem *pItem : m_vectorItem)
	{
		if (pItem->id == id)
		{
			pItem->enable = enable;
			return true;
		}
	}

	return false;
}

bool QYMenu::enableItem(std::string key, bool enable)
{
	for (QYMenuItem *pItem : m_vectorItem)
	{
		if (pItem->key == key)
		{
			pItem->enable = enable;
			return true;
		}
	}

	return false;
}



void QYMenu::SetItemText(UINT id, const TCHAR* strText)
{
	for (QYMenuItem *pItem : m_vectorItem)
	{
		if (pItem->id == id)
		{
			pItem->strText = strText;
			return;
		}
	}

}

void QYMenu::setItemText(UINT id, const char* text)
{
	for (QYMenuItem *pItem : m_vectorItem)
	{
		if (pItem->id == id)
		{
			const char *srcText = QYLanguage::instance()->getText(text);
			SET_TEXT(pItem->strText, srcText)
			return;
		}
	}
}

void QYMenu::setItemText(std::string key, const char* text)
{
	for (QYMenuItem *pItem : m_vectorItem)
	{
		if (pItem->key == key)
		{
			const char *srcText = QYLanguage::instance()->getText(text);
			SET_TEXT(pItem->strText, srcText)
				return;
		}
	}
}


bool QYMenu::CheckItem(UINT id, bool check)
{
	for (QYMenuItem *pItem : m_vectorItem)
	{
		if (pItem->id == id)
		{
			pItem->check = check;
			return true;
		}
	}

	return false;
}

bool QYMenu::checkItem(std::string key, bool check)
{
	for (QYMenuItem *pItem : m_vectorItem)
	{
		if (pItem->key == key)
		{
			pItem->check = check;
			return true;
		}
	}

	return false;
}

QYWindow* QYMenu::getSubMenu(std::string key)
{
	for (QYMenuItem *pItem : m_vectorItem)
	{
		if (pItem->key == key)
		{
			return pItem->pSubMenu;
		}
	}

	return nullptr;
}

QYWindow* QYMenu::addSubMenu(std::string key)
{
	for (QYMenuItem *pItem : m_vectorItem)
	{
		if (pItem->key == key)
		{
			if (nullptr != pItem->pSubMenu)
			{
				return pItem->pSubMenu;
			}
			else
			{
				pItem->pSubMenu = new QYMenu;

				if (!((QYMenu*)pItem->pSubMenu)->Create())
				{
					SAFE_DELETE(pItem->pSubMenu);
					return nullptr;
				}

				((QYMenu*)pItem->pSubMenu)->SetParentMenu(this);
				return pItem->pSubMenu;
			}

		}
	}

	return nullptr;
}

QYWindow* QYMenu::AddSubMenu(UINT id)
{
	for (QYMenuItem *pItem : m_vectorItem)
	{
		if (pItem->id == id)
		{
			if (nullptr != pItem->pSubMenu)
			{
				return pItem->pSubMenu;
			}
			else
			{
				pItem->pSubMenu = new QYMenu;
				
				if (!((QYMenu*)pItem->pSubMenu)->Create())
				{
					SAFE_DELETE(pItem->pSubMenu);
					return nullptr;
				}

				((QYMenu*)pItem->pSubMenu)->SetParentMenu(this);
				return pItem->pSubMenu;
			}
			
		}
	}

	return nullptr;
}

void QYMenu::SetItemHeight(int itemHeight)
{
	m_nItemHeight = itemHeight;
}


void QYMenu::Clear()
{
	for (QYMenuItem *pItem : m_vectorItem)
	{
		if (nullptr != pItem)
		{
			if (nullptr != pItem->pSubMenu)
			{
				SAFE_DESTROY_WINDOW_PTR(pItem->pSubMenu);
			}
			SAFE_DELETE(pItem);
		}
		
	}

	m_vectorItem.clear();
}


void QYMenu::TrackPopup(QYPoint point, int width, int pos, QYWindow *pWnd, bool show)
{
	if (NULL == pWnd)
	{
		return;
	}

	m_pSourceWnd = pWnd;

	auto cY = GetSystemMetrics(SM_CYSCREEN);
	auto cX = GetSystemMetrics(SM_CXSCREEN);

	int realHeight = m_nItemHeight * m_vectorItem.size() + 2 * m_nBorderWidth + 2 * m_padHeight;


	QYPaintDC dc(this);

	HFONT OldFont = (HFONT)dc.SelectObject(m_pFont);

	int realWidth = 0;

	for (auto it = m_vectorItem.begin(); it != m_vectorItem.end(); it++)
	{
		realWidth = std::fmax(realWidth, dc.GetTextExtent((*it)->strText).cx + 2 * m_padWidth + 2 * m_nBorderWidth);
	}

	dc.SelectObject(OldFont);

	realWidth = std::fmax(realWidth, width);

	if (nullptr != m_pCheckImage)
	{
		realWidth += m_pCheckImage->GetWidth();
		m_padWidth = std::fmax(m_padWidth, m_pCheckImage->GetWidth());
	}


	if (point.x + realWidth > cX)
	{
		point.x -= realWidth;
		QYWindow *pParentMenu = this->GetParentMenu();
		if (nullptr != pParentMenu)
		{
			QYRect rectWnd;
			pParentMenu->GetWindowRect(rectWnd);

			point.x -= rectWnd.Width();
		}

		point.x += 2;
	}
	else
	{
		point.x -= 2;
	}

	if (point.y + realHeight > cY)
	{
		point.y -= realHeight;
		if (nullptr != this->GetParentMenu())
		{
			point.y += m_nItemHeight;
		}
		else
		{
			point.y -= m_nItemHeight;
		}
		
	}

	QYRect rectWnd(point.x,
		point.y ,
		point.x + realWidth,
		point.y + realHeight);


	::SetWindowPos(m_hWnd, HWND_TOPMOST, point.x, point.y, realWidth, realHeight, SWP_DRAWFRAME);

	m_nSelItem = -1;

	if (nullptr != QYApp::m_hPopWndMouseHook)
	{
		//g_ConsoleLog.Print("UnhookWindowsHookEx g_hMouseHook\n");
		::UnhookWindowsHookEx(QYApp::m_hPopWndMouseHook);
		QYApp::m_hPopWndMouseHook = nullptr;
		QYApp::m_hPopWindow = nullptr;
	}

	show ? Show(SW_SHOW) : 0;

	(nullptr != m_pShadow) ? m_pShadow->OnMove(rectWnd) : 0;
}

void QYMenu::DrawGuideArrow(HDC hDC, QYRect *rect, bool sel)
{
	COLORREF color = QY_THEME.SCROLLBAR_ARROW_COLOR_NORMAL;
	if (sel)
	{
		color = QY_THEME.TEXT_COLOR_SELECT;
	}

	HPEN hBtnPen = CreatePen(PS_SOLID, QY_THEME.LARGE_ARROW_PEN_WIDTH, color);

	RECT rectTemp;
	rectTemp.top = rect->top;
	rectTemp.left = rect->left;
	rectTemp.bottom = rect->bottom;
	rectTemp.right = rect->right;

	RECT rectReal;
	int sideLen = min(rect->Height(), rect->Width()) * 3 / 6;
	sideLen = (0 == (sideLen % 2)) ? sideLen : (sideLen + 1);
	rectReal.top = (rect->Height() - sideLen) / 2 + rect->top;
	rectReal.left = (rect->Width() - sideLen) / 2 + rect->left;
	rectReal.bottom = rectReal.top + sideLen;

	HPEN oldPen = (HPEN)::SelectObject(hDC, hBtnPen);

	POINT pt;
	MoveToEx(hDC, rectReal.left, rectReal.top, &pt);
	LineTo(hDC, rectReal.left + sideLen / 2, rectReal.top + sideLen / 2);
	LineTo(hDC, rectReal.left , rectReal.bottom );

	::SelectObject(hDC, oldPen);
	::DeleteObject(hBtnPen);
}

LRESULT QYMenu::OnPaint()
{
	QYPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	QYRect rect;
	GetClientRect(rect);

	QYMemDC MemDc(&dc, rect);

	HFONT OldFont = (HFONT)MemDc.SelectObject(m_pFont);

	QYBrush Brush(QY_THEME.MENU_BK_COLOR_NORMAL);
	HBRUSH OldBrush;
	
	OldBrush = (HBRUSH)MemDc.SelectObject(&Brush);

	//QYPen PenLine(PS_SOLID, m_nBorderWidth, QY_THEME.LISTBOX_BORDER_COLOR);
	//HPEN OldPen;

	//OldPen = (HPEN)MemDc.SelectObject(&PenLine);

	MemDc.FillSolidRect(rect, QY_THEME.MENU_BK_COLOR_NORMAL);

	//MemDc.SelectObject(OldBrush);
	//MemDc.SelectObject(OldPen);
	//PenLine.DeleteObject();

	MemDc.SetBkMode(TRANSPARENT);

	auto it = m_vectorItem.begin();
	int i = 0;
	int top = rect.top + m_nBorderWidth + m_padHeight;
	
	QYRect rectTemp = QYRect(0, 0,0, 0);
	for (; it != m_vectorItem.end(); it++, i++)
	{
		bool sel = false;
		rectTemp.left = rect.left + m_nBorderWidth;
		rectTemp.top = rect.top + m_padHeight + i * m_nItemHeight;
		rectTemp.right = rect.right - m_nBorderWidth;
		rectTemp.bottom = rectTemp.top + m_nItemHeight;
		if ((*it)->enable)
		{
			MemDc.SetTextColor(QY_THEME.TEXT_COLOR_NORMAL);
			if (m_nSelItem != i)
			{
				MemDc.FillSolidRect(rectTemp, QY_THEME.MENU_BK_COLOR_NORMAL);
			}
			else
			{
				MemDc.FillSolidRect(rectTemp, QY_THEME.MENU_BK_COLOR_SELECT);
				sel = true;
			}
		}
		else
		{
			MemDc.SetTextColor(QY_THEME.TEXT_COLOR_DISABLE);
		}

		if ((*it)->check && nullptr != m_pCheckImage)
		{
			m_pCheckImage->Draw(MemDc.GetDC(),
				rectTemp.left + (rectTemp.Height() - m_pCheckImage->GetHeight()) / 2,
				rectTemp.top + (rectTemp.Height() - m_pCheckImage->GetHeight()) / 2,
				m_pCheckImage->GetWidth(), m_pCheckImage->GetHeight());
		}

		rectTemp.DeflateRect(m_padWidth, 0);
		MemDc.DrawText(&(*it)->strText, (*it)->strText.Length(), &rectTemp, DT_SINGLELINE | DT_VCENTER);

		if (nullptr != (*it)->pSubMenu)
		{
			QYRect rectArrow = QYRect(rectTemp.right, 
				rectTemp.top + rectTemp.Height() / 8, 
				rect.right - m_nBorderWidth - rectTemp.Height() / 8,
				rectTemp.bottom - rectTemp.Height() / 8);
			DrawGuideArrow(MemDc.GetDC(), &rectArrow, sel);
		}
	}

	MemDc.SelectObject(OldFont);
	
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDc, 0, 0, SRCCOPY);

	MemDc.DeleteDC();

	return TRUE;
}

#if 1


BOOL QYMenu::Show(int nCmdShow)
{
	(nullptr != m_pShadow) ? m_pShadow->Show(nCmdShow) : 0;

	if (nCmdShow == SW_HIDE)
	{
		QYApp::StopPopWndMouseHook();
	}
	else
	{
		QYApp::StartPopWndMouseHook(m_hWnd);
	}

	SetFocus();

	return QYWindow::Show(nCmdShow);
}
#endif


void  QYMenu::OnSubMenuHided()
{
	QYRect rect;
	GetClientRect(rect);

	POINT pt;
	::GetCursorPos(&pt);
	::ScreenToClient(m_hWnd, &pt);

	QYPoint point = pt;

	if (rect.PtInRect(point))
	{

		if (point.x < rect.right)
		{
			if (-1 != m_nSelItem)
			{
				m_nCursel = m_nSelItem;
				auto it = m_vectorItem.begin();
				std::advance(it, m_nCursel);
				if (m_vectorItem.end() != it && (*it)->enable && nullptr == (*it)->pSubMenu)
				{
					ReleaseCapture();
					Show(SW_HIDE);

					(nullptr != m_pParentMenu) ? ((QYMenu*)m_pParentMenu)->Show(SW_HIDE) : 0;

					m_pSourceWnd->SendMessage(WM_COMMAND, (*it)->id, 0);
				}
				//m_nSelItem = -1;
			}
		}
		else
		{
			QYApp::m_hPopWindow = m_hWnd;
		}

	}
	else
	{
		ReleaseCapture();
		Show(SW_HIDE);

		//(nullptr != m_pParentMenu) ? ((QYMenu*)m_pParentMenu)->Show(SW_HIDE) : 0;
		(nullptr != m_pParentMenu) ? ((QYMenu*)m_pParentMenu)->PostMessage(QY_WM_SUBMENU_HIDED) : 0;
	}

}

LRESULT QYMenu::OnLButtonDown(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	QYRect rect;
	GetClientRect(rect);

	if (rect.PtInRect(point))
	{
		if (point.x < rect.right)
		{
			if (-1 != m_nSelItem)
			{
				//nothing to do
			}
		}
		else
		{
			QYApp::m_hPopWindow = m_hWnd;
		}

	}
	else
	{
		ReleaseCapture();
		Show(SW_HIDE);

		//(nullptr != m_pParentMenu) ? ((QYMenu*)m_pParentMenu)->Show(SW_HIDE) : 0;
		(nullptr != m_pParentMenu) ? ((QYMenu*)m_pParentMenu)->PostMessage(QY_WM_SUBMENU_HIDED) : 0;
	}


	return TRUE;

	//CWnd::OnLButtonDown(nFlags, point);
}


BOOL QYMenu::OnMove(int cx, int cy)
{
	if (nullptr != m_pShadow )
	{
		QYRect rt;                  // 使用MoveWindow函数的示例
		GetWindowRect(rt);
		m_pShadow->OnMove(rt);
	}

	return FALSE;
}
LRESULT QYMenu::OnSize(UINT nType, int cx, int cy)
{
	//CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	QYRect rect;
	GetClientRect(rect);

	if (nullptr != m_pShadow)
	{
		QYRect rt;                  // 使用MoveWindow函数的示例
		GetWindowRect(rt);
		m_pShadow->OnMove(rt);
	}

	return TRUE;
}



LRESULT QYMenu::OnLButtonUp(UINT nFlags, QYPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	QYRect rect;
	GetClientRect(rect);

	if (rect.PtInRect(point))
	{
		if (point.x < rect.right)
		{
			if (-1 != m_nSelItem)
			{
				m_nCursel = m_nSelItem;
				auto it = m_vectorItem.begin();
				std::advance(it, m_nCursel);
				if (m_vectorItem.end() != it && (*it)->enable && nullptr == (*it)->pSubMenu)
				{
					ReleaseCapture();
					Show(SW_HIDE);

					struct w_funcCB_t *actionCB = nullptr;
					if ((nullptr != m_pParentMenu))
					{
						((QYMenu*)m_pParentMenu)->Show(SW_HIDE);
						actionCB = m_pParentMenu->getCallback(QY_CALLBACK_EVENT);
					}
					else
					{
						actionCB = getCallback(QY_CALLBACK_EVENT);
					}
					
					if (nullptr != actionCB)
					{
						QYPropertyList properties;
						properties.addProperty("id", (*it)->key);
						actionCB->callback(&properties);
					}
					else
					{
						m_pSourceWnd->PostMessage(WM_COMMAND, (*it)->id, 0);
					}
					
				}
				m_nSelItem = -1;
			}
		}
		else
		{
			QYApp::m_hPopWindow = m_hWnd;
		}

	}
	else
	{
		ReleaseCapture();
		Show(SW_HIDE);

		//(nullptr != m_pParentMenu) ? ((QYMenu*)m_pParentMenu)->Show(SW_HIDE) : 0;
		(nullptr != m_pParentMenu) ? ((QYMenu*)m_pParentMenu)->PostMessage(QY_WM_SUBMENU_HIDED) : 0;
	}


	return TRUE;
	//CWnd::OnLButtonUp(nFlags, point);
}


BOOL QYMenu::loadMenu(QYXMLTreeNode *pNode)
{
	auto menuElemList = pNode->getChildrenList();
	for (QYXMLTreeNode* pMenuItem : *menuElemList)
	{
		if ("item" == pMenuItem->name)
		{
			addItem(pMenuItem->getAttribute("id").c_str(), pMenuItem->getAttribute("text").c_str());
			if (0 < pMenuItem->getChildrenList()->size())
			{
				QYMenu *pSubMenu = (QYMenu*)addSubMenu(pMenuItem->getAttribute("id").c_str());
				if (nullptr != pSubMenu)
				{
					pSubMenu->loadMenu(pMenuItem);
				}
			}
		}
	}

	return TRUE;
}

BOOL QYMenu::loadMenu(const char *source, const char* group)
{
	FILE *pFile = nullptr;
	std::string layoutPath = QYApp::m_layoutPath + source;
	QYXML xml;
	if (xml.Load(layoutPath.c_str()))
	{
		auto pRoot = xml.GetRoot();
		auto groupElemList = pRoot->getChildrenList();
		for (auto pGroupElem : *groupElemList)
		{
			if ("UI" == pGroupElem->name)
			{
				groupElemList = pGroupElem->getChildrenList();
				for (auto pGroupElem : *groupElemList)
				{
					if ("group" == pGroupElem->name && group == pGroupElem->getAttribute("id"))
					{
						if (Create())
						{
							return loadMenu(pGroupElem);
						}

						return FALSE;
					}
				}
			}
		}
	}

	return FALSE;
}